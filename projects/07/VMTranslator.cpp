#include<iostream>
#include<string>
#include<fstream>
#include<map>
#include<algorithm>
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
    Parser(){}
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
        //remove comments
        // cout<<command<<command.size()<<endl;
        for(int i=0;i<(int)command.size()-1;i++){
            if(command[i]=='/'&& command[i+1]=='/'){
                command=command.substr(0,i);
                break;
            }
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
    public:
    CodeWriter(){}
    CodeWriter(string path){
        //Output file/stream
        fout.open(path);
        label_count=0;
    }
    ~CodeWriter(){
        fout.close();
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
    void writePushPop(commands_type_e type,string arg1,int arg2,string filename){
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
                fout<<"@"<<filename<<"."<<arg2<<endl;//@Filename.i
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
                fout<<"@"<<filename<<"."<<arg2<<endl;//@Filename.i
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
    string rpath=argv[1];
    string filename,wpath;
    
    int found=rpath.rfind('/');
    if (found == string::npos) {
        filename=rpath;
    }else{
        wpath=rpath.substr(0,found+1);
        filename=rpath.substr(found+1);
    }
    filename=filename.substr(0,filename.find("."));
    wpath+=filename+".asm";
    cout<<wpath<<endl;

    Parser read(rpath);
    CodeWriter write(wpath);
    while(read.hasMoreCommands()){
        if(read.advance()){
            if(read.commandType()==C_ARITHMETIC){
                write.writeArithmetic(read.arg1(read.commandType()));
            }else if(read.commandType()==C_PUSH | read.commandType()==C_POP){
                write.writePushPop(read.commandType(),read.arg1(read.commandType()),read.arg2(read.commandType()),filename);
            }
            // cout<<read.commandType()<<" "<<read.arg1(read.commandType())<<" "<<read.arg2(read.commandType())<<endl;
        }
    }
}