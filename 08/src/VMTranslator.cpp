#include<iostream>
#include<string>
#include<fstream>
#include<map>
#include<algorithm>
#include<dirent.h>
#include<vector>
using namespace std;
enum commands_type_e {C_ARITHMETIC , C_PUSH , C_POP , C_LABEL , C_GOTO , C_IF , C_FUNCTION , C_RETURN , C_CALL};
class Parser{
    //handles parsing of .vm files 
    //Read a VM command , parses the command into its lexical components , and provides convenient access to these components
    //Ignores all white spaces
    private:
    ifstream fin;
    string command;
    public:
    Parser(string path){
        //Opens the input file/stream
        fin.open(path);

    }
    ~Parser(){
        fin.close();
    }
    bool hasMoreCommands(){
        //are there more commans in the input
        return !fin.eof();
    }
    bool advance(){
        //Reads the next command
        getline(fin,command);
        //remove comments
        // cout<<command<<command.size()<<endl;
        for(int i=0;i<(int)command.size()-1;i++){
            if(command[i]=='/'&& command[i+1]=='/'){
                command=command.substr(0,i);
                break;
            }
        }

        //remove whitespaces and comments
        //from start
        while(command[0]==' '){
            command.erase(command.begin());
        }
        //from end
        // cout<<(int)command[command.length()-1]<<endl;
        while(command[command.length()-1]==' '| command[command.length()-1]==13){
            command.pop_back();
        }
        
        // cout<<command<<endl;
        if(command.length()==0){
            return false;
        }
        return true;
    }
    commands_type_e commandType(){
        //C_ARITHMETIC , C_PUSH , C_POP , C_LABEL , C_GOTO , C_IF , C_FUNCTION , C_RETURN , C_CALL
        //Returns a constant representing the type of current command
        int found=command.find(" ");
        string arg0=command.substr(0,found);
        map<string,commands_type_e> cvt2enum;
        cvt2enum.insert(pair<string,commands_type_e>("add",C_ARITHMETIC));
        cvt2enum.insert(pair<string,commands_type_e>("sub",C_ARITHMETIC));
        cvt2enum.insert(pair<string,commands_type_e>("neg",C_ARITHMETIC));
        cvt2enum.insert(pair<string,commands_type_e>("eq",C_ARITHMETIC));
        cvt2enum.insert(pair<string,commands_type_e>("gt",C_ARITHMETIC));
        cvt2enum.insert(pair<string,commands_type_e>("lt",C_ARITHMETIC));
        cvt2enum.insert(pair<string,commands_type_e>("and",C_ARITHMETIC));
        cvt2enum.insert(pair<string,commands_type_e>("or",C_ARITHMETIC));
        cvt2enum.insert(pair<string,commands_type_e>("not",C_ARITHMETIC));
        cvt2enum.insert(pair<string,commands_type_e>("pop",C_POP));
        cvt2enum.insert(pair<string,commands_type_e>("push",C_PUSH));
        cvt2enum.insert(pair<string,commands_type_e>("label",C_LABEL));
        cvt2enum.insert(pair<string,commands_type_e>("goto",C_GOTO));
        cvt2enum.insert(pair<string,commands_type_e>("if-goto",C_IF));
        cvt2enum.insert(pair<string,commands_type_e>("function",C_FUNCTION));
        cvt2enum.insert(pair<string,commands_type_e>("call",C_CALL));
        cvt2enum.insert(pair<string,commands_type_e>("return",C_RETURN));
        return cvt2enum[arg0];
    }
    string arg1(commands_type_e type){
        //Returns the first argument of the current command
        //in cased of C_ARITHMETIC command itself is returned
        //not called if C_RETURN
        if(type==C_RETURN){
            return "";
        }
        if(type==C_ARITHMETIC){
            return command;
        }
        int found=command.find(" ");
        int found2=command.find(" ",found+1);
        return command.substr(found+1,found2-found-1);
    }
    int arg2(commands_type_e type){
        //Returns second argument of current command
        //Called for C_PUSH , C_POP , C_FUNCTION or C_CALL
        if(type==C_PUSH | type==C_POP | type==C_FUNCTION | type==C_CALL){
            int found=command.rfind(' ');
            // cout<<command<<" "<<command.substr(found+1)<<endl;
            return stoi(command.substr(found+1));
        }
        else{
            return -1;
        }
    }
};
class CodeWriter{
    ofstream fout;
    int label_count;
    int call_count;
    string fileName;
    string functionName;
    void compare(string op,string command){
        //SP--,D=*SP,SP--,*SP=*SP==D,SP++
            fout<<"@SP"<<endl;//SP--
            fout<<"M=M-1"<<endl;
            fout<<"A=M"<<endl;//D=*SP
            fout<<"D=M"<<endl;
            fout<<"@temp"<<endl;
            fout<<"M=D"<<endl;
            fout<<"@SP"<<endl;//SP--
            fout<<"M=M-1"<<endl;
            fout<<"A=M"<<endl;//*SP-D
            fout<<"D=M"<<endl;
            fout<<"@temp"<<endl;
            fout<<"D=D-M"<<endl;
            fout<<"@"<<op<<"_TRUE_"<<label_count<<endl;
            fout<<"D;"<<command<<endl;
            fout<<"@"<<op<<"_FALSE_"<<label_count<<endl;
            fout<<"0;JMP"<<endl;
            fout<<"("<<op<<"_TRUE_"<<label_count<<")"<<endl;
            fout<<"@SP"<<endl;
            fout<<"A=M"<<endl;
            fout<<"M=-1"<<endl;
            fout<<"@"<<op<<"_END_"<<label_count<<endl;
            fout<<"0;JMP"<<endl;
            fout<<"("<<op<<"_FALSE_"<<label_count<<")"<<endl;
            fout<<"@SP"<<endl;
            fout<<"A=M"<<endl;
            fout<<"M=0"<<endl;
            fout<<"@"<<op<<"_END_"<<label_count<<endl;
            fout<<"0;JMP"<<endl;
            fout<<"("<<op<<"_END_"<<label_count<<")"<<endl;
            fout<<"@SP"<<endl;//SP++
            fout<<"M=M+1"<<endl;
            label_count++;
    }
    void push_D(){
        fout<<"@SP"<<endl;//*SP=D
        fout<<"A=M"<<endl;
        fout<<"M=D"<<endl;
        fout<<"@SP"<<endl;//SP++
        fout<<"M=M+1"<<endl;
    }
    public:
    CodeWriter(string path){
        //Output file/stream
        fout.open(path);
        label_count=0;
        call_count=0;
        functionName="null";
    }
    ~CodeWriter(){
        fout.close();
    }
    void setFileName(string filename){
        //informs the codeWriter that the translation of a new VM file has started
        fileName=filename;
    }
    void writeInit(){
        //writes the assembly instruction that effect the bootstrap code thtat initializes the VM
        //This code must be placed at the beginning of the generated *.asm file
        //SP=256
        //Call Sys.init
        fout<<"@256"<<endl;//SP=256
        fout<<"D=A"<<endl;
        fout<<"@SP"<<endl;
        fout<<"M=D"<<endl;
        writeCall("Sys.init",0);
    }
    void writeLabel(string label){
        //writes assembly code that effects the label command
        //label declaration command
        //functionName$label
        fout<<"("<<functionName<<"$"<<label<<")"<<endl;
    }
    void writeGoto(string label){
        ///writes assembly code that effects the goto command
        //jump to execute the command just after the label
        fout<<"@"<<functionName<<"$"<<label<<endl;
        fout<<"0;JMP"<<endl;
    }
    void writeIf(string label){
        ///writes assembly code that effects the if-goto command
        //cond=pop
        //if cond jump to execute the command just after label
        //SP--,jump
        fout<<"@SP"<<endl;//SP--
        fout<<"M=M-1"<<endl;
        fout<<"A=M"<<endl;//jump
        fout<<"D=M"<<endl;
        fout<<"@"<<functionName<<"$"<<label<<endl;
        fout<<"D;JNE"<<endl;
    }
    void writeFunction(string functionName,int numVars){
        //writes assembly code that effects the function command
        //(functionName) //declares a label for the function
        // cout<<this->functionName<<" "<<functionName<<endl;
        this->functionName=(functionName); 
        fout<<"("<<functionName<<")"<<endl;
        //    repeat nVars times; //nVars = number of local variables
        //    push 0 //initializes the local variables to 0
        for(int i=0;i<numVars;i++){
            fout<<"@SP"<<endl;//*SP=0
            fout<<"A=M"<<endl;
            fout<<"M=0"<<endl;
            fout<<"@SP"<<endl;//SP++
            fout<<"M=M+1"<<endl;
        }
    }
    void writeCall(string functionName,int numVars){
        //writes assembly code that effects the call command
        //push returnAddress //(Using the label declared below)
        string returnAddress=functionName+"$ret."+to_string(call_count);
        fout<<"@"<<returnAddress<<endl;//D=returnAddress
        fout<<"D=A"<<endl;
        push_D();
        //push LCL //Save LCL of the caller
        fout<<"@LCL"<<endl;
        fout<<"D=M"<<endl;
        push_D();
        //push ARG //Save ARG of the caller
        fout<<"@ARG"<<endl;
        fout<<"D=M"<<endl;
        push_D();
        //push THIS //Save THIS of the caller
        fout<<"@THIS"<<endl;
        fout<<"D=M"<<endl;
        push_D();
        //push THAT //Save THAT of the caller
        fout<<"@THAT"<<endl;
        fout<<"D=M"<<endl;
        push_D();
        //ARG = SP-5-nArgs //Repositions ARG
        fout<<"@SP"<<endl;//ARG=SP
        fout<<"D=M"<<endl;
        fout<<"@ARG"<<endl;
        fout<<"M=D"<<endl;
        fout<<"@"<<5+numVars<<endl;//D=5+nArgs
        fout<<"D=A"<<endl;
        fout<<"@ARG"<<endl;//ARG = ARG - D
        fout<<"M=M-D"<<endl;
        //LCL = SP //Repositions LCL
        fout<<"@SP"<<endl;
        fout<<"D=M"<<endl;
        fout<<"@LCL"<<endl;
        fout<<"M=D"<<endl;
        //goto functionName //Transfers control to the called function
        fout<<"@"<<functionName<<endl;
        fout<<"0;JMP"<<endl;
        //(returnAddress) //Declares a label for the return address //functionName$ret.i
        fout<<"("<<returnAddress<<")"<<endl;
        call_count++;
    }
    void writeReturn(){
        //writes assembly code that effects the return command
        //endFrame=LCL //endframe is a temporary variable
        fout<<"@LCL"<<endl;
        fout<<"D=M"<<endl;
        fout<<"@endFrame"<<endl;
        fout<<"M=D"<<endl;
        //retAddr = *(endFrame -5) //gets the return address        
        fout<<"@5"<<endl;
        fout<<"D=D-A"<<endl;
        fout<<"A=D"<<endl;
        fout<<"D=M"<<endl;
        fout<<"@retAddr"<<endl;
        fout<<"M=D"<<endl;
        //*ARG = pop() //Repositions the return value for the caller
        fout<<"@SP"<<endl;//SP--
        fout<<"M=M-1"<<endl;
        fout<<"A=M"<<endl;//*ARG=*SP
        fout<<"D=M"<<endl;
        fout<<"@ARG"<<endl;
        fout<<"A=M"<<endl;
        fout<<"M=D"<<endl;
        //SP=ARG+1 //Respositions SP for the caller
        fout<<"@ARG"<<endl;
        fout<<"D=M+1"<<endl;
        fout<<"@SP"<<endl;
        fout<<"M=D"<<endl;
        //THAT =*(endFrame-1) //Respositions THAT for the caller
        fout<<"@endFrame"<<endl;
        fout<<"M=M-1"<<endl;
        fout<<"A=M"<<endl;
        fout<<"D=M"<<endl;
        fout<<"@THAT"<<endl;
        fout<<"M=D"<<endl;
        //THIS =*(endFrame-2) //Respositions THIS for the caller
        fout<<"@endFrame"<<endl;
        fout<<"M=M-1"<<endl;
        fout<<"A=M"<<endl;
        fout<<"D=M"<<endl;
        fout<<"@THIS"<<endl;
        fout<<"M=D"<<endl;
        //ARG =*(endFrame-3) //Respositions ARG for the caller
        fout<<"@endFrame"<<endl;
        fout<<"M=M-1"<<endl;
        fout<<"A=M"<<endl;
        fout<<"D=M"<<endl;
        fout<<"@ARG"<<endl;
        fout<<"M=D"<<endl;
        //LCL =*(endFrame-4) //Respositions LCL for the caller
        fout<<"@endFrame"<<endl;
        fout<<"M=M-1"<<endl;
        fout<<"A=M"<<endl;
        fout<<"D=M"<<endl;
        fout<<"@LCL"<<endl;
        fout<<"M=D"<<endl;
        //goto retAddr //goes to return address in the caller's code
        fout<<"@retAddr"<<endl;
        fout<<"A=M"<<endl;
        fout<<"0;JMP"<<endl; 
    }
    void writeArithmetic(string arg){
        //writes to the output file that assembly code that implements the given arithmetic command
        if(arg=="add"){
            //add
            //SP--,D=*SP,SP--,*SP=*SP+D,SP++
            fout<<"@SP"<<endl;//SP--
            fout<<"M=M-1"<<endl;
            fout<<"A=M"<<endl;//D=*SP
            fout<<"D=M"<<endl;
            fout<<"@temp"<<endl;
            fout<<"M=D"<<endl;
            fout<<"@SP"<<endl;//SP--
            fout<<"M=M-1"<<endl;
            fout<<"A=M"<<endl;//*SP=*SP+D
            fout<<"D=M"<<endl;
            fout<<"@temp"<<endl;
            fout<<"D=M+D"<<endl;
            fout<<"@SP"<<endl;
            fout<<"A=M"<<endl;
            fout<<"M=D"<<endl;
            fout<<"@SP"<<endl;//SP++
            fout<<"M=M+1"<<endl;
        }else if(arg=="sub"){    
            //sub
            //SP--,D=*SP,SP--,*SP=*SP-D,SP++
            fout<<"@SP"<<endl;//SP--
            fout<<"M=M-1"<<endl;
            fout<<"A=M"<<endl;//D=*SP
            fout<<"D=M"<<endl;
            fout<<"@temp"<<endl;
            fout<<"M=D"<<endl;
            fout<<"@SP"<<endl;//SP--
            fout<<"M=M-1"<<endl;
            fout<<"A=M"<<endl;//*SP=*SP-D
            fout<<"D=M"<<endl;
            fout<<"@temp"<<endl;
            fout<<"D=D-M"<<endl;
            fout<<"@SP"<<endl;
            fout<<"A=M"<<endl;
            fout<<"M=D"<<endl;
            fout<<"@SP"<<endl;//SP++
            fout<<"M=M+1"<<endl;
        }else if(arg=="neg"){
            //neg
            //SP--,*SP=-*SP,SP++
            fout<<"@SP"<<endl;//SP--
            fout<<"M=M-1"<<endl;
            fout<<"A=M"<<endl;//*SP=-*SP
            fout<<"M=-M"<<endl;
            fout<<"@SP"<<endl;//SP++
            fout<<"M=M+1"<<endl;
        }else if(arg=="eq"){
            //eq
            compare("EQ","JEQ");

        }else if(arg=="gt"){
            //get
            compare("GT","JGT");
        }else if(arg=="lt"){
            //lt
            compare("LT","JLT");
        }else if(arg=="and"){
            //and
            //SP--,D=*SP,SP--,*SP=*SP&D,SP++
            fout<<"@SP"<<endl;//SP--
            fout<<"M=M-1"<<endl;
            fout<<"A=M"<<endl;//D=*SP
            fout<<"D=M"<<endl;
            fout<<"@temp"<<endl;
            fout<<"M=D"<<endl;
            fout<<"@SP"<<endl;//SP--
            fout<<"M=M-1"<<endl;
            fout<<"A=M"<<endl;//*SP=*SP&D
            fout<<"D=M"<<endl;
            fout<<"@temp"<<endl;
            fout<<"D=D&M"<<endl;
            fout<<"@SP"<<endl;
            fout<<"A=M"<<endl;
            fout<<"M=D"<<endl;
            fout<<"@SP"<<endl;//SP++
            fout<<"M=M+1"<<endl;
        }else if(arg=="or"){
            //or
            //SP--,D=*SP,SP--,*SP=*SP|D,SP++
            fout<<"@SP"<<endl;//SP--
            fout<<"M=M-1"<<endl;
            fout<<"A=M"<<endl;//D=*SP
            fout<<"D=M"<<endl;
            fout<<"@temp"<<endl;
            fout<<"M=D"<<endl;
            fout<<"@SP"<<endl;//SP--
            fout<<"M=M-1"<<endl;
            fout<<"A=M"<<endl;//*SP=*SP|D
            fout<<"D=M"<<endl;
            fout<<"@temp"<<endl;
            fout<<"D=D|M"<<endl;
            fout<<"@SP"<<endl;
            fout<<"A=M"<<endl;
            fout<<"M=D"<<endl;
            fout<<"@SP"<<endl;//SP++
            fout<<"M=M+1"<<endl;
        }else if(arg=="not"){
            //not
            //SP--,*SP=!*SP,SP++
            fout<<"@SP"<<endl;//SP--
            fout<<"M=M-1"<<endl;
            fout<<"A=M"<<endl;//*SP=!*SP
            fout<<"M=!M"<<endl;
            fout<<"@SP"<<endl;//SP++
            fout<<"M=M+1"<<endl;
        }
    }
    void writePushPop(commands_type_e type,string arg1,int arg2){
        //writes output file
        //C_PUSH,C_POP,segment,index
        if(arg1=="constant"){
            //constant
            //*SP=17,SP++
            if(type==C_PUSH){
                fout<<"@"<<arg2<<endl;//*SP=17
                fout<<"D=A"<<endl;
                fout<<"@SP"<<endl;
                fout<<"A=M"<<endl;
                fout<<"M=D"<<endl;
                fout<<"@SP"<<endl;//SP++
                fout<<"M=M+1"<<endl;
            }else if(type==C_POP){

            }
        }else if(arg1=="local"|arg1=="argument"|arg1=="this"|arg1=="that"){
            //local,argument,this,that
            //addr=LCL+2,SP--,*addr=*SP
            //addr=LCL+2,*SP=*addr,SP++
            map<string,string> cvt2addr;
            cvt2addr.insert(pair<string,string>("local","LCL"));
            cvt2addr.insert(pair<string,string>("argument","ARG"));
            cvt2addr.insert(pair<string,string>("this","THIS"));
            cvt2addr.insert(pair<string,string>("that","THAT"));
            string addr=cvt2addr[arg1];
            if(type==C_PUSH){
                //addr=LCL+2,*SP=*addr,SP++
                fout<<"@"<<arg2<<endl;//addr=LCL+2
                fout<<"D=A"<<endl;
                fout<<"@"<<addr<<endl;
                fout<<"D=D+M"<<endl;
                fout<<"A=D"<<endl;
                fout<<"D=M"<<endl;//*SP=*addr
                fout<<"@SP"<<endl;
                fout<<"A=M"<<endl;
                fout<<"M=D"<<endl;
                fout<<"@SP"<<endl;//SP++
                fout<<"M=M+1"<<endl;
            }else if(type==C_POP){
                //addr=LCL+2,SP--,*addr=*SP
                fout<<"@"<<arg2<<endl;//addr=LCL+2
                fout<<"D=A"<<endl;
                fout<<"@"<<addr<<endl;
                fout<<"D=D+M"<<endl;
                fout<<"@addr"<<endl;
                fout<<"M=D"<<endl;
                fout<<"@SP"<<endl;//SP--
                fout<<"M=M-1"<<endl;
                fout<<"A=M"<<endl;//*addr=*SP
                fout<<"D=M"<<endl;
                fout<<"@addr"<<endl;
                fout<<"A=M"<<endl;
                fout<<"M=D"<<endl;
            }

        }else if(arg1=="static"){
            //static
            //@Filename.i,M=D
            if(type==C_PUSH){
                //@Filename.i,*SP=*addr,SP++
                fout<<"@"<<fileName<<"."<<arg2<<endl;//@Filename.i
                fout<<"D=M"<<endl;//*SP=*addr
                fout<<"@SP"<<endl;
                fout<<"A=M"<<endl;
                fout<<"M=D"<<endl;
                fout<<"@SP"<<endl;//SP++
                fout<<"M=M+1"<<endl;
            }else if(type==C_POP){
                //@Filename.i,SP--,*addr=*SP
                fout<<"@SP"<<endl;//SP--
                fout<<"M=M-1"<<endl;
                fout<<"A=M"<<endl;//*addr=*SP
                fout<<"D=M"<<endl;
                fout<<"@"<<fileName<<"."<<arg2<<endl;//@Filename.i
                fout<<"M=D"<<endl;
            }

        }else if(arg1=="temp"){
            //temp
            //addr=5+2,SP--,*addr=*SP
            //addr=5+2,*addr=*SP,SP++
            if(type==C_PUSH){
                fout<<"@5"<<endl;//addr=5+2
                fout<<"D=A"<<endl;
                fout<<"@"<<arg2<<endl;
                fout<<"D=D+A"<<endl;
                fout<<"A=D"<<endl;
                fout<<"D=M"<<endl;//*SP=*addr
                fout<<"@SP"<<endl;
                fout<<"A=M"<<endl;
                fout<<"M=D"<<endl;
                fout<<"@SP"<<endl;//SP++
                fout<<"M=M+1"<<endl;
            }else if(type==C_POP){
                fout<<"@"<<arg2<<endl;//addr=LCL+2
                fout<<"D=A"<<endl;
                fout<<"@5"<<endl;
                fout<<"D=D+A"<<endl;
                fout<<"@addr"<<endl;
                fout<<"M=D"<<endl;
                fout<<"@SP"<<endl;//SP--
                fout<<"M=M-1"<<endl;
                fout<<"A=M"<<endl;//*addr=*SP
                fout<<"D=M"<<endl;
                fout<<"@addr"<<endl;
                fout<<"A=M"<<endl;
                fout<<"M=D"<<endl;
            }

        }else if(arg1=="pointer"){
            //pointer
            //*SP=THIS/THAT,SP++
            //SP--,THIS/THAT=*SP
            if(type==C_PUSH){
                if(arg2==0){
                    fout<<"@THIS"<<endl;
                }else if(arg2==1){
                    fout<<"@THAT"<<endl;
                }
                fout<<"D=M"<<endl;//*SP=*addr
                fout<<"@SP"<<endl;
                fout<<"A=M"<<endl;
                fout<<"M=D"<<endl;
                fout<<"@SP"<<endl;//SP++
                fout<<"M=M+1"<<endl;
            }else if(type==C_POP){
                fout<<"@SP"<<endl;//SP--
                fout<<"M=M-1"<<endl;
                fout<<"A=M"<<endl;//*addr=*SP
                fout<<"D=M"<<endl;
                if(arg2==0){
                    fout<<"@THIS"<<endl;
                }else if(arg2==1){
                    fout<<"@THAT"<<endl;
                }
                fout<<"M=D"<<endl;
            }

        }
    }
};
int main(int argc, char *argv[]){
    vector<string> files;
    string wpath;
    string path=argv[1];
    if(path.find(".vm")==string::npos){
        // cout<<"Dir";
        //its a directory
        DIR * directory;   // creating pointer of type dirent
        struct dirent *x;   // pointer represent directory stream
        if (( directory= opendir (path.c_str())) != NULL){      // check if directory  open
            while ((x = readdir (directory)) != NULL) {
                string fileName=x->d_name;
                // cout<<fileName<<endl;
                if(fileName.find(".vm")!= string::npos){
                    //found vm file
                    // cout<<"DIR"<<endl;
                    files.push_back(path+"/"+fileName);
                }
            }
        }
        closedir (directory); //close directory....
        
        int found=path.rfind('/');
        wpath=path;
        wpath+="/"+path.substr(found+1)+".asm";
    }else{
        // cout<<"File";
        cout<<path;
        int found=path.find(".vm");
        wpath=path.substr(0,found);
        cout<<wpath<<found<<endl;
        wpath+=".asm";
        files.push_back(path);
    }
    cout<<"Writing to "<<wpath<<endl;
    CodeWriter write(wpath);
    if(files.size()>1){
        write.writeInit();
    }
    for(int i=0;i<files.size();i++){
        string rpath=files[i];
        cout<<"Found file"<<rpath<<endl;
        string fileName;    
        int found=rpath.rfind('/');
        if (found == string::npos) {
            fileName=rpath;
        }else{
            fileName=rpath.substr(found+1);
        }
        fileName=fileName.substr(0,fileName.find(".vm"));
        // cout<<fileName<<endl;
        Parser read(rpath);
        write.setFileName(fileName);
        while(read.hasMoreCommands()){
            if(read.advance()){
                if(read.commandType()==C_ARITHMETIC){
                    write.writeArithmetic(read.arg1(read.commandType()));
                }else if(read.commandType()==C_PUSH | read.commandType()==C_POP){
                    write.writePushPop(read.commandType(),read.arg1(read.commandType()),read.arg2(read.commandType()));
                }else if(read.commandType()==C_LABEL){
                    write.writeLabel(read.arg1(read.commandType()));
                }else if(read.commandType()==C_GOTO){
                    write.writeGoto(read.arg1(read.commandType()));
                }else if(read.commandType()==C_IF){
                    write.writeIf(read.arg1(read.commandType()));
                }else if(read.commandType()==C_FUNCTION){
                    write.writeFunction(read.arg1(read.commandType()),read.arg2(read.commandType()));
                }else if(read.commandType()==C_RETURN){
                    write.writeReturn();
                }else if(read.commandType()==C_CALL){
                    write.writeCall(read.arg1(read.commandType()),read.arg2(read.commandType()));
                }
                // cout<<read.commandType()<<" "<<read.arg1(read.commandType())<<" "<<read.arg2(read.commandType())<<endl;
            }
        }
    }
}