#include "include/JackTokenizer.h"
#include "include/JackCompiler.h"
#include "include/JackAnalyzer.h"
int main(int argc, char *argv[]){
    //prompt> JackAnalyzer input
    //Input : finename.jack: name of single source file , or 
    //         directoryName: name of a directory containing one or more .jack source files 
    //Output: if the input is a single file: fileName.xml
    // if the input is a directory: one .xml file for every .jack file. stored in the same directory
    vector<string> files;
    string wpath;
    string path=argv[1];
    if(path.find(".jack")==string::npos){
        // cout<<"Dir";
        //its a directory
        DIR * directory;   // creating pointer of type dirent
        struct dirent *x;   // pointer represent directory stream
        if (( directory= opendir (path.c_str())) != NULL){      // check if directory  open
            while ((x = readdir (directory)) != NULL) {
                string fileName=x->d_name;
                // cout<<fileName<<endl;
                if(fileName.find(".jack")!= string::npos){
                    //found vm file
                    // cout<<"DIR"<<endl;
                    int found=fileName.find(".jack");
                    files.push_back(path+"/"+fileName.substr(0,found));
                }
            }
        }
        closedir (directory); //close directory.... 
    }else{
        // cout<<"File";
        // cout<<path;
        int found=path.find(".jack");
        path=path.substr(0,found);
        // cout<<wpath<<found<<endl;
        files.push_back(path);
    }
    for(int i=0;i<files.size();i++){
        cout<<files[i]<<endl;
        JackTokenizer * tokenize=new JackTokenizer(files[i]);
        while(tokenize->hasMoreTokens()){
            tokenize->advance();
            switch(tokenize->tokenType()){
                case KEYWORD:tokenize->keyWord();break;
                case SYMBOL:tokenize->symbol();break;
                case IDENTIFIER:tokenize->identifier();break;
                case INT_CONST:tokenize->intVal();break;
                case STRING_CONST:tokenize->stringVal();break;
            }
        }
        delete(tokenize);
        //compile class
        CompilationEngine * engine=new CompilationEngine(files[i]);
        engine->CompileClass();
        delete(engine);
    }
}