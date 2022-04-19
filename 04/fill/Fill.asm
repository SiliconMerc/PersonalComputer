// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Fill.asm

// Runs an infinite loop that listens to the keyboard input.
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel;
// the screen should remain fully black as long as the key is pressed. 
// When no key is pressed, the program clears the screen, i.e. writes
// "white" in every pixel;
// the screen should remain fully clear as long as no key is pressed.

// Put your code here.
//if(D==0)
//	val=-1
//else
//	val=0
//for i =0 ; i < 8191 ; i++
//	RAM[screen+i]=val
	
(POSITIVE)
//if(D==0)
@KBD
D=M

//val=0
//if(D!=0)
//	val=-1
@val
M=0
@PRINT
D;JEQ
@val
M=-1

//for i =0 ; i <= 8191 ; i++
//	RAM[screen+i]=val
(PRINT)
@8191
D=A
@n
M=D
@i
M=0

(LOOP)
@i
D=M
@n
D=D-M

@POSITIVE //check again for keyboard
D;JGT // if i>n goto POSITIVE

@i
D=M
@SCREEN
A=D+A
D=A
@addr
M=D //screen+i

@val
D=M 
@addr
A=M
M=D	 //RAM[screen+i]=val

@i //i=i+1
M=M+1

@LOOP
0;JMP
