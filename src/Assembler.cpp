/*
 * This file is part of Nand2Tertris.project6.
 *
 * See the COPYRIGHT file at the top-level directory of this distribution
 * for details of code ownership.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include<iostream>
#include<map>
#include<fstream>
#include <string> 
#include <filesystem>
#include<vector>
#include<exception>
using namespace std;
class SymbolTable{
    //preinitialize the table 
    //first pass => add the label symbols 
    //add the var symbols
    map<string,string> table;//table of symbols and values
    int top;//address of top of variable that is available
    public:
    SymbolTable(){
        //add register addresses
        for(int i=0;i<=15;i++){
            table.insert(pair<string,string>("R"+to_string(i),to_string(i)));
        }
        //add pre defined symbols
        table.insert(pair<string,string>("SCREEN",to_string(16384)));
        table.insert(pair<string,string>("KBD",to_string(24576)));
        table.insert(pair<string,string>("SP",to_string(0)));
        table.insert(pair<string,string>("LCL",to_string(1)));
        table.insert(pair<string,string>("ARG",to_string(2)));
        table.insert(pair<string,string>("THIS",to_string(3)));
        table.insert(pair<string,string>("THAT",to_string(4)));
        //address of variables , that is available
        top=16;
    }
    ~SymbolTable(){
        table.clear();
    }
/**
 * Look the key in the table.
 *
 * The symbol table contains the key and value pairs for pre defined variables and user defined
 *
 * @param , key
 * @return , ""=>not found , otherwise value
 */
    string lookup(string key){
        //lookup the key in the table
        if ( table.find(key) == table.end() ) {
        // not found
            return "";
        } else {
        // found
            return table.at(key);
        }
    }
/**
 * Insert new element to table
 *
 * @param , label=> the label in map , address=> -1 is a variable(i.e values start from 16) , otherwise its a fixed value
 * @return , no return
 */
    void insert(string label,int address){
        //insert new labels / variables to table
        // cout<<label<<" "<<address<<endl;
        if (address==-1){
            table.insert(pair<string,string>(label,to_string(top)));
            top++;
        }
        else
            table.insert(pair<string,string>(label,to_string(address)));
    }
};
class TableLookup{
    //comp , destination , jump directive 
    map<string,string> comp,dest,jump;//table and value lookup table 
    public:
    TableLookup(){
        //comp
        //a=0
        comp.insert(pair<string,string>("0","0101010"));
        comp.insert(pair<string,string>("1","0111111"));
        comp.insert(pair<string,string>("-1","0111010"));
        comp.insert(pair<string,string>("D","0001100"));
        comp.insert(pair<string,string>("A","0110000"));
        comp.insert(pair<string,string>("!D","0001101"));
        comp.insert(pair<string,string>("!A","0110001"));
        comp.insert(pair<string,string>("-D","0001111"));
        comp.insert(pair<string,string>("-A","0110011"));
        comp.insert(pair<string,string>("D+1","0011111"));
        comp.insert(pair<string,string>("A+1","0110111"));
        comp.insert(pair<string,string>("D-1","0001110"));
        comp.insert(pair<string,string>("A-1","0110010"));
        comp.insert(pair<string,string>("D+A","0000010"));
        comp.insert(pair<string,string>("D-A","0010011"));
        comp.insert(pair<string,string>("A-D","0000111"));
        comp.insert(pair<string,string>("D&A","0000000"));
        comp.insert(pair<string,string>("D|A","0010101"));
        //a=1
        comp.insert(pair<string,string>("M","1110000"));
        comp.insert(pair<string,string>("!M","1110001"));
        comp.insert(pair<string,string>("-M","1110011"));
        comp.insert(pair<string,string>("M+1","1110111"));
        comp.insert(pair<string,string>("M-1","1110010"));
        comp.insert(pair<string,string>("D+M","1000010"));
        comp.insert(pair<string,string>("D-M","1010011"));
        comp.insert(pair<string,string>("M-D","1000111"));
        comp.insert(pair<string,string>("D&M","1000000"));
        comp.insert(pair<string,string>("D|M","1010101"));

        //dest
        dest.insert(pair<string,string>("","000"));
        dest.insert(pair<string,string>("M","001"));
        dest.insert(pair<string,string>("D","010"));
        dest.insert(pair<string,string>("MD","011"));
        dest.insert(pair<string,string>("A","100"));
        dest.insert(pair<string,string>("AM","101"));
        dest.insert(pair<string,string>("AD","110"));
        dest.insert(pair<string,string>("AMD","111"));

        //jump
        jump.insert(pair<string,string>("","000"));
        jump.insert(pair<string,string>("JGT","001"));
        jump.insert(pair<string,string>("JEQ","000"));
        jump.insert(pair<string,string>("JGE","011"));
        jump.insert(pair<string,string>("JLT","100"));
        jump.insert(pair<string,string>("JNE","101"));
        jump.insert(pair<string,string>("JLE","110"));
        jump.insert(pair<string,string>("JMP","111"));
    }
    ~TableLookup(){
        comp.clear();
        dest.clear();
        jump.clear();
    }
    string lookupComp(string mnemonic){
        return comp.at(mnemonic);
    }
    string lookupDest(string mnemonic){
        return dest.at(mnemonic);
    }
    string lookupJump(string mnemonic){
        return jump.at(mnemonic);
    }
};
class Assembler{
    public:
    SymbolTable symbol;//symbol object to lookup symbols
    TableLookup table;//table lookup to convert string to bit string
/**
 * Show all the values in the vector
 *
 * @param vec , vector of strings containing all the values
 * @return void , no return values
 */
    void showVec(vector<string>& vec){
        //print all the values present in the vector
        for(int i=0;i<vec.size();i++){
            cout<<vec[i]<<" ";
        }
        cout<<endl;
    }
/**
 * Check if the string is a number
 *
 * @param str, the string of number of strings
 * @return bool , true=>the string contains all numbers 
 */
    bool isNumber(const string& str)
    {
        //check if the string is a number
        for (char const &c : str) {
            if (std::isdigit(c) == 0) return false;
        }
        return true;
    }
/**
 * Parser 
 * take source instruction and decompose it into parts
 * ainstruction => 1 value returned
 * cinstruction => 3 values returned
 * 
 * @param , instruction as read from file , without preprocessing . firstPass , lables of location are added
 * @return , string of vectors containing decomposed values i.e basic parts of instruction
 * comment => empty 
 * a instruction => address
 * c instruction => comp , dest and jump
 */
    vector<string> parser(string myText,bool firstPass){
        // take source instruction and decompose it into 3 fields
        vector<string> decomposed;
        //remove the whitespaces and comments
        string instruction=validInstruction(myText);
        int index=0;
        if(instruction.size()){
            if(instruction.at(0)=='@'| instruction.at(0)=='('){
                //a instruction 
                //structure : @valueInBinary
                if(instruction.at(0)=='@'){
                    //if it is a variable
                    decomposed.push_back(instruction.substr(1,instruction.size()-1));
                    // cout<<instruction<<"Found @inst "<<decomposed[0];
                }
                else{
                    //if it is a label
                    if(firstPass){
                        decomposed.push_back(instruction.substr(1,instruction.find_last_of(')')-1));
                        decomposed.push_back("label");
                    }
                }
            }
            else{
                //c instruction
                //dest=comp;jump 
                decomposed.assign(3,"");
                for(string::iterator i=instruction.begin();i<instruction.end();){
                    // cout<<*i<<endl;
                    if(*i=='='){
                        //add dest
                        string dest=instruction.substr(0,i-instruction.begin());
                        i=instruction.erase(instruction.begin(),i+1);
                        decomposed[1]=dest;
                    }
                    else if(*i==';'){
                        //add comp
                        string comp=instruction.substr(0,i-instruction.begin());
                        i=instruction.erase(instruction.begin(),i+1);
                        decomposed[0]=comp;
                        index=2;
                    }
                    else{
                        i++;
                    }
                }
                //add jmp or comp
                string jmp=instruction.substr(0,instruction.end()-instruction.begin());
                decomposed[index]=jmp;
            }
        }
        return decomposed;
    }
/**
 * Convert numerical string to binary
 *
 * @param number , the string containing of numbers
 * @return binary string , containg of 0s and 1s of length 15 bits
 */
    string valueInBinary(string number){
        //convert string number to binary value
        // cout<<number<<" ";
        int num=stoi(number);
        string bin;
        bin.assign(15,'0');
        int index=14;
        while(num!=0){
            bin[index--]='0'+num%2;
            num=num/2;
        }
        // cout<<bin;
        return bin;
    }
/**
 * Compose a instruction bits from basic parts and symbol table
 * 
 * @param decomposed , basic parts of instructions divided into parts
 * @return 16 bit instruction to be printed
 */
    string aInstruction(vector<string> decomposed){
        // showVec(decomposed);
        string result;
        // cout<<"Reached";
        if(isNumber(decomposed[0])){
            //if direct address
            result=decomposed[0];
        }
        else{
            //if variable or label is used
            // cout<<decomposed[0];
            result=symbol.lookup(decomposed[0]);//if label
            // cout<<result.size();
            if(result.size()==0){
                //add variable to lookup table
                symbol.insert(decomposed[0],-1);//if variable , add the new variable to symbol table
                // throw "Label or Varibale Not Found in Symbol Table!";
            }
            result=symbol.lookup(decomposed[0]);//get the variable again
            // cout<<result.size();
            // cout<<decomposed[0]<<" "<<result<<" "<<valueInBinary(result);
        }
        // cout<<result;
        string bin=valueInBinary(result);
        return "0"+bin;
    }
/**
 * Compose c instruction bits from basic parts and symbol table
 *
 * @param decomposed , basic parts of instructions divided into parts
 * @return 16 bit instruction to be printed
 */
    string cInstruction(vector<string>&  decomposed){
        // cout<<decomposed[0]<<" "<<decomposed[1]<<" "<<decomposed[2]<<endl;
        string res="";
        res+=table.lookupComp(decomposed[0]);//get comp bits
        res+=table.lookupDest(decomposed[1]);//get dest bits
        res+=table.lookupJump(decomposed[2]);//get jump directive bits
        // cout<<table.lookupComp(decomposed[0])<<" "<<table.lookupDest(decomposed[1])<<" "<<table.lookupJump(decomposed[2])<<endl;
        return "111"+res;
    }
/**
 * Remove the whitespaces and comments 
 * 
 * @param Instruction as read from the file
 * @return basic instruction required for procesing , comment return empty string
 */
    string validInstruction(string inst){
        // cout<<(int)inst[0];
        // cout<<inst;
        //we receive the string instruction
        //remove comments and whitespaces
        for(string::iterator i=inst.begin();i<inst.end();){
            // cout<<*i<<endl;
            if(*i==' '|*i==13){
                //remove whitespaces
                // cout<<inst<<endl;
                i=inst.erase(i);
                // cout<<inst<<endl;
            }
            else if(*i=='/'){
                if(*++i=='/'){
                    //remove comments
                    // cout<<*i;
                    i=inst.erase(--i, inst.end());
                }
                else{
                    throw "Invalid Syntax . Wrong Comment!";
                    // throw "Invalid Syntax . Wrong Comment!";
                }
            }
            else{
                i++;
            }
        }
        // cout<<inst.size();
        return inst;
    }
/**
 * Execute the first pass on the files and add the labels to symbol table
 * 
 * @param string instruction , current running line number
 * @return new line number after this update
 */
    int firstPass(string& myText,int line){
        //add labels to symbol table
        vector<string> decomposed=parser(myText,true);
        // showVec(decomposed);
        string label;
        if(decomposed.size()==0){
            //its a comment , no new line in binary
            return line;
        }
        else if (decomposed.size()==2){
            //address 
            string variable=decomposed[0];
            if(decomposed[1]=="variable"){
                //@variable , so new line
                line++;
            }
            else if(decomposed[1]=="label"){
                //@label , no new line . Add the label to symbol table
                symbol.insert(variable,line);
            }
            return line;
        }
        else{
            return ++line;//c instruction => new line
        }
    }
/**
 * For the final loop , convert the instruction string to binary
 * 
 * @param instruction string 
 * @return bit string , assembler output
 */
    string secondPass(string myText){
        // cout<<symbol.lookup("INFINITE_LOOP");
        vector<string> decomposed=parser(myText,false);
        // cout<<"Reached";
        // showVec(decomposed);
        // cout<<decomposed.size();
        string bin;
        if(decomposed.size()==0){
            return "";//comments
        }
        else if (decomposed.size()==1){
            bin=aInstruction(decomposed);//a instruction
        }
        else{
            bin=cInstruction(decomposed);//c instruction
        }
        // cout<<bin<<endl;
        return bin;
    }
/**
 * Parent function combining all the previous functions
 * 
 * Executing the first pass and second pass
 * Reading and writing to the file 
 * 
 * @param ifstream and ofstream objects to read and write
 * @return void , no return statements
 */
    // public:
    void assemble(ifstream& MyReadFile,ofstream& MyFile){
        int count=1;
        while(count<=2){
            //read instruction line by line from the file
            string myText;
            int line=0;
            // Use a while loop together with the getline() function to read the file line by line
            while (getline(MyReadFile, myText)) {
                // Output the text from the file
                // myText.substr(0,myText.size()-1);
                // Write to the file
                // MyFile << myText;
                if(count==1){
                    line=firstPass(myText,line);//execute the first pass
                }
                else{
                    myText.erase(myText.end()-1);
                    // cout<<myText.length();
                    // cout<<secondPass(myText);
                    string bin=secondPass(myText);//second pass
                    if(bin!="")
                        MyFile<<bin<<endl;//write the binary to file
                }
            }
            count++;
            MyReadFile.clear();                 // clear fail and eof bits
            MyReadFile.seekg(0, std::ios::beg); // back to the start!
        }
        
    }

};
void testing(ifstream& MyReadFile,ofstream& MyFile){
    Assembler assembler;
    // string inst="M=D";
    // string inst="A=A-1";
    // string inst="M=-1";
    string inst="(LABEL)";
    
    // inst="// This file is part of www.nand2tetris.org";
    // string inst="@2\n";
    
    // Test on string
    // inst="   M=D              // M[2] = D (greatest number)";
    // inst="0;JMP";
    // cout<<(int)inst[0]<<(int)' ';
    
    try{
        // inst=assembler.validInstruction(inst);
        // // cout<<inst<<inst.length();
        vector<string> decomposed=assembler.parser(inst,false);
        assembler.showVec(decomposed);
        cout<<assembler.secondPass(inst);
    }
    catch(char const*ex){
        cout<<ex;
    }


    // //Test on full file
    // // cout<<"Something";
    // string myText;
    // // Use a while loop together with the getline() function to read the file line by line
    // while (getline(MyReadFile, myText)) {
    //     inst=assembler.validInstruction(myText);
    //     if(inst.length()!=0){
    //         // cout<<inst<<inst.length()<<endl;
    //             // vector<string> decomposed=assembler.parser(inst);
    //             // assembler.showVec(decomposed);
    //             cout<<assembler.secondPass(inst)<<endl;
    //         }
    //     // if(inst.length()==1){
    //     //     cout<<(int)inst[0]<<endl;
    //     // }
    // }
}
int main(){
    string path="/home/captain/Documents/Nand2Tetris/nand2tetris/projects/06/add/Add.asm";
    // path="/home/captain/Documents/Nand2Tetris/nand2tetris/projects/06/max/Max.asm";
    // path="/home/captain/Documents/Nand2Tetris/nand2tetris/projects/06/max/MaxL.asm";
    // path="/home/captain/Documents/Nand2Tetris/nand2tetris/projects/06/pong/Pong.asm";
    // path="/home/captain/Documents/Nand2Tetris/nand2tetris/projects/06/pong/PongL.asm";
    path="/home/captain/Documents/Nand2Tetris/nand2tetris/projects/06/rect/Rect.asm";
    // path="/home/captain/Documents/Nand2Tetris/nand2tetris/projects/06/rect/RectL.asm";
    string outFile=path.substr(path.find_last_of('/')+1,path.find_last_of('.')-path.find_last_of('/')-1);
    outFile=path.substr(0,path.find_last_of('/')+1)+outFile+".hack";
    // cout<<outFile;
    ifstream MyReadFile(path);
    ofstream MyFile(outFile);
    
    Assembler assembler;
    assembler.assemble(MyReadFile,MyFile);

    // testing(MyReadFile,MyFile);

    // Close the file
    MyReadFile.close(); 

    // Close the file
    MyFile.close();
    return 0;
    
}