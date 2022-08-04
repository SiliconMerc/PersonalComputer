#include<iostream>
#include<string>
using namespace std;
class Parser{
    //handles parsing of .vm files 
    //Read a VM command , parses the command into its lexical components , and provides convenient access to these components
    //Ignores all white spaces
    Parser(){
        //Opens the input file/stream
    }
    void hasMoreCommands(){
        //are there more commans in the input
    }
    void advance(){
        //Reads the next command
    }
    void commandType(){
        //C_ARITHMETIC , C_PUSH , C_POP , C_LABEL , C_GOTO , C_IF , C_FUNCTION , C_RETURN , C_CALL
        //Returns a constant representing the type fo current command

    }
    string arg1(){
        //Returns the first argument of the current command
        //in cased of C_ARITHMETIC command itself is returned
        //not called if C_RETURN
    }
    int arg2(){
        //Returns second argument of current command
        //Called for C_PUSH , C_POP , C_FUNCTION or C_CALL
    }
};
class CodeWriter{
    CodeWriter(){
        //Output file/stream
    }
    void writeArithmetic(){
        //writes to the output file that assembly code that implements the given arithmetic command
    }
    void writePushPop(){
        //writes output file
        //C_PUSH,C_POP,segment,index
    }
    void close(){

    }
};
int main(){

}