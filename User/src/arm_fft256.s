;遵循ATPCS协议  
	THUMB
    REQUIRE8
    PRESERVE8
    EXPORT FFT_FUN
    IMPORT cosf
	IMPORT sinf
    IMPORT sprintf
	EXTERN __aeabi_ui2f
	EXTERN __aeabi_fadd
	EXTERN __aeabi_fsub
	EXTERN __aeabi_fmul
	EXTERN __aeabi_fdiv
    AREA |.text|, CODE, READONLY, ALIGN=2
	
	;特殊功能寄存器命名
	
	
	;常数定义
num equ 256
;PI应该如何定义和使用？？
	
	
	;功能MACRO定义
	
		MACRO	;生成Rader数组，存在数据段Rader中,注意清零此宏要使用的寄存器 R4，R5，R6
	Rader		;不需要参数,生成的重排序数组首地址在 R4中
	LDR R4,=R	;R4,R5,R6:存储R数组首地址，本次变换得到的数，i
    MOV R6,#0
LOOPA1
    RBIT R5,R6
    LSR R5,#24
    STR R5,[R4,R6,LSL #2]
    ADD R6,#1
    CMP R6,#256
    BNE LOOPA1
	MEND
	
	
		MACRO	;数组重排序宏，按照数组R顺寻排序	参数：input:待排序数组的首地址寄存器
	Reorder $a  ;{R4-R9} used
    LDR R4,=R
    LDR R7,=Temp
    MOV R6,#0

LOOPA2
    LDR R8,[R4,R6,LSL #2]
    LDR R9,[$a,R8,LSL #2]
    STR R9,[R7,R6,LSL #2]
	ADD R6,R6,#1
    CMP R6,#256
    BNE LOOPA2

    MOV R6,#0
LOOPA3
    LDR R8,[R7,R6,LSL #2]
    STR R8,[$a,R6,LSL #2]
	ADD R6,R6,#1
    CMP R6,#256
    BNE LOOPA3
    MEND
	
	
		MACRO			;复数相乘宏
	;input:Rx,Ry  output:Rx    (寄存器)
	;本函数会征用R4寄存器，使用后R4自动恢复到 *a_re
    Cpmul   $x_re,$x_im,$y_re,$y_im
	;		R10		R11   R4	R5
        ;征用R12(j)
    PUSH {R12}

    MOV R0,$x_im
    MOV R1,$y_im
    BL __aeabi_fmul     
    MOV R12,R0           ;R12 = $x_im * $y_im

    MOV R0,$x_re
    MOV R1,$y_re
    BL __aeabi_fmul     ;R0 = $x_re * $y_re

    MOV R1,R12
    BL __aeabi_fsub     ;R0 = R0 - R12

    LDR R12,=Cp_Temp
    STR R0,[R12]         ;Cp_Temp = R0($x_re * $y_re - $x_im * $y_im)

    MOV R0,$x_im
    MOV R1,$y_re
    BL __aeabi_fmul     
    MOV R12,R0           ;R12 = $x_im * $y_re

    MOV R0,$x_re
    MOV R1,$y_im
    BL __aeabi_fmul     ;R0 = $x_re * $y_im

    MOV R1,R12
    BL __aeabi_fadd     ;R0 = R0 + R12 (mul_im)
    MOV $x_im,R0        ;x_im = (x_re*y_re)-(x_im*y_im)

    LDR R12,=Cp_Temp
    LDR $x_re,[R12]      ;x_re = (x_re*y_im)+(x_im*y_re)

	POP {R12}         ;恢复R12 = j
	MEND
	
	
	






;汇编函数FFT
FFT_FUN 

	                ;初始化，存储a_re,a_im地址到内存A_re,A_im
	LDR R4,=A_re
    LDR R5,=A_im
    STR R0,[R4]
    STR R1,[R5]

	
	Rader	        ;第一步：生成Rader重排序数组	使用寄存器R4,R5,R6
			
	MOV R5,R0
	Reorder R5      ;第二步：a_re,a_im数组重排序


                    ;第三步：FFT算数三层循环

    MOV R6,#1       ;初始：    mid=1
LOOPA_MID
    ;(get:   float(mid),PI,PI/mid )
    MOV R0,R6
    BL __aeabi_ui2f
    ;加载更高精度PI到R7（3.141592741/3.14159），先加载FP32 高16bit,移位，加上低16bit
    
    ;MOV R7,#0x4049
    ;LSL R7,#16
    ;ADD R7,#0x0FDB  
    LDR R7,=0x40490FDB  ;R7=PI
    ;考虑用 LDR R7,#0x40490FDB代替上面三条指令
    
    MOV R1,R0
    MOV R0,R7
    BL __aeabi_fdiv
    MOV R10,R0

    ;(get:   W_re = w_re)
    BL cosf         ;R0 is PI/mid now
    LDR R8,=W_re
    STR R0,[R8]

    ;(get:   W_im = w_im)
    MOV R0,R10      ;R0 has been changed by (BL cosf)
    BL sinf
    LDR R9,=W_im
    STR R0,[R9]


    ;/**********************此处嵌入LOOPA_I段********************************/
    MOV R7,#0   ;i = 0
LOOPA_I
    ;Omega_re = float(1),Omega_im = float(0)
    LDR R10,=Omega_re
    LDR R11,=Omega_im

    MOV R8,#0x3F80
    LSL R8,R8,#16   ;R8 = float(1)
    MOV R9,#0x0     ;R9 = float(0)
    
    STR R8,[R10]
    STR R9,[R11]

    MOV R10,R8      ;把R10从Omega_re的地址恢复为Omega_re的值
    MOV R11,R9      ;把R10从Omega_re的地址恢复为Omega_re的值，确保LOOPA_J正常运行


    ;/************************此处嵌入LOOPA_J******************************/
	MOV R12,#0              ;j = 0
LOOPA_J
    ;x_re = a_re[i+j];
    ;x_im = a_im[i+j];
    LDR R4,=A_re
    LDR R4,[R4]             ;R4 = *a_re
    LDR R5,=A_im
    LDR R5,[R5]             ;R5 = *a_im
    ADD R9,R7,R12           ;R9 = i+j
    LDR R8,[R4,R9,LSL #2]
    LDR R9,[R5,R9,LSL #2]


    ADD R2,R7,R12
    ADD R2,R2,R6            ;R2 = mid+i+j
    LDR R4,[R4,R2,LSL #2]   ;R4 = a_re[i+j+mid]
    LDR R5,[R5,R2,LSL #2]   ;R5 = a_re[i+j+mid]
    ;{R10,R11} = {omega_re , omega_im}
    ;cplx_mul(omega_re,omega_im,a_re[i+j+mid],a_im[i+j+mid]) -->(y_re,y_im)
    Cpmul R10,R11,R4,R5     ;{R10,R11} = {y_re , y_im}


    ;恢复：R4 = *a_re, R5 = *a_im
    LDR R4,=A_re
    LDR R4,[R4]             ;R4 = *a_re
    LDR R5,=A_im
    LDR R5,[R5]             ;R5 = *a_im

    ;a_re[i+j] = x_re+y_re;
    MOV R0,R8
    MOV R1,R10
	PUSH {R12}				;J入栈
    BL __aeabi_fadd         ;R0 = R8+R10(浮点加法)
	POP {R12}				;J出栈
    ADD R2,R7,R12           ;R7:i, R12:j
    STR R0,[R4,R2,LSL #2]
	
    ;a_im[i+j] = x_im+y_im;
    MOV R0,R9
    MOV R1,R11
	PUSH {R12}				;J入栈
    BL __aeabi_fadd         ;R0 = R9+R11(浮点加法)
	POP {R12}				;J出栈
    ADD R2,R7,R12           ;R7:i, R12:j
    STR R0,[R5,R2,LSL #2]


    ;a_re[i+j+mid] = x_re-y_re;
    MOV R0,R8
    MOV R1,R10
	PUSH {R12}				;J入栈
    BL __aeabi_fsub         ;R0 = R8-R10(浮点加法)
	POP {R12}				;J出栈
    ADD R2,R7,R12
    ADD R2,R2,R6    ;R2 = mid+i+j
    STR R0,[R4,R2,LSL #2]
	
    ;a_im[i+j+mid] = x_im-y_im;
    MOV R0,R9
    MOV R1,R11
	PUSH {R12}				;J入栈
    BL __aeabi_fsub         ;R0 = R9-R11(浮点加法)		；R12 ip寄存器，fsub会改变
	POP {R12}				;J出栈
    ADD R2,R7,R12
    ADD R2,R2,R6            ;R2 = mid+i+j
    STR R0,[R5,R2,LSL #2]


    ;omega值从内存重新取
    LDR R10,=Omega_re
    LDR R11,=Omega_im
    LDR R10,[R10]
    LDR R11,[R11]
    ;W值从内存重新取
    LDR R8,=W_re
    LDR R9,=W_im
    LDR R8,[R8]
    LDR R9,[R9]
    ;cplx_mul(omega_re,omega_im,w_re,w_im) --> (omega_re,omega_im);
    Cpmul R10,R11,R8,R9
    ;这一步之后要把omega存入内存,R8,R9中W值可以扔掉
    LDR R8,=Omega_re
    LDR R9,=Omega_im    ;{R8,R9} = Omega地址
    STR R10,[R8]
    STR R11,[R9]



	;test sprintf mid --> Test
    LDR R2,=Test
    STR R12,[R2,R12,LSL #2]



    ADD R12,#1       ;j++
    CMP R12,R6       ;j<mid
    BNE LOOPA_J



    ADD R7,R7,R6,LSL #1   ;i=i+mid*2
    CMP R7,#256     ;i<256
    BNE LOOPA_I




    LSL R6,R6,#1    ;mid=mid*2
    CMP R6,#256     ;mid<256
    BNE LOOPA_MID

	NOP
	
	
	
	AREA data1,DATA,READWRITE
R		SPACE 1024		;(int=32bit=4RAM)
Temp	SPACE 1024
Test    SPACE 1024      ;测试用输出数组
A_re		SPACE 4		;存储地址的二阶指针，8*4bit
A_im		SPACE 4		;存储地址的二阶指针，8*4bit
Omega_re	SPACE 4
Omega_im	SPACE 4
W_re		SPACE 4
W_im		SPACE 4
Cp_Temp 	SPACE 4
            END