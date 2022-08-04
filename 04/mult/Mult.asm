// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Mult.asm

// Multiplies R0 and R1 and stores the result in R2.
// (R0, R1, R2 refer to RAM[0], RAM[1], and RAM[2], respectively.)
//
// This program only needs to handle arguments that satisfy
// R0 >= 0, R1 >= 0, and R0*R1 < 32768.

// Put your code here.

//Psuedo code 
//sum=0
//i=0
//for i=0;i<R0;i++
//	sum=sum+R1
//R2=sum


//sum=0
@sum
M=0
//i=0
@i
M=0

@R0
D=M
@END
D;JEQ
@R1
D=M
@END
D;JEQ
//for i=0;i<R0;i++
//	sum=sum+R1
(LOOP)
@sum
D=M
@R1
D=D+M
@sum
M=D
@i
M=M+1//i++
@i
D=M
@R0
D=D-M
@END
D;JEQ
@LOOP
0;JMP

(END)
//R2=sum
@sum
D=M
@R2
M=D
@END
0;JMP
