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
//	val=0
//else
//	val=-1
//for i =0 ; i <= 8191 ; i++
//	RAM[screen+i]=val
	
(KBDLOOP)
@8192
D=A
@n
M=D//n=8192
@i
M=0
@val//val=0
M=0

@KBD
D=M
@LOOP
D;JEQ
@val
M=-1//val=-1
(LOOP)
@SCREEN
D=A
@i
D=M+D
@addr
M=D
@val
D=M
@addr
A=M
M=D//RAM[screen+i]=val
@i
M=M+1
@n
D=M
@i
D=D-M
@KBDLOOP
D;JEQ
@LOOP
0;JMP
