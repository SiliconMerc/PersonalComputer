#include "include/JackAnalyzer.h"
bool CompilationEngine::is_type(){
    if(current_val=="int" | current_val=="char" | current_val=="boolean"){
        printToken();
        return true;
    } else if(is_className()){
        return true;
    }
    return false;
}
//className: identifier //no markup
bool CompilationEngine::is_className(){
    if(current_tag=="identifier"){
        printToken();
        return true;
    }
    return false;
}
//subroutineName: identifier //no markup
bool CompilationEngine::is_subroutineName(){
    if(current_tag=="identifier"){
        printToken();
        return true;
    }
    return false;
}
//varName: identifier //no markup
bool CompilationEngine::is_varName(){
    if(current_tag=="identifier"){
        // printToken(current_val);
        printToken();
        return true;
    }
    return false;
}
//statement //no markup
//subroutineCall: subroutineName '(' expressionList ')' | (className |varName)'.'subroutineName'('expressionList')' //no markup

//op: '+'|'-'|'*'|'/'|'&'|'|'|'<'|'>'|'=' //symbol
bool CompilationEngine::is_op(){
    regex k("\\+|\\-|\\*|\\/|\\&|\\||\\<|\\>|\\=|\\&lt\\;|\\&gt\\;|\\&quot\\;|\\&amp\\;");
    if(regex_match(current_val, k)){
        printToken();
        return true;
    }
    // printToken(current_val);
    return false;
}
//unaryOp: '-'|'~' //symbol
bool CompilationEngine::is_unaryOp(){
    regex k("\\-|\\~");
    if(regex_match(current_val, k)){
        printToken();
        return true;
    }
    return false;
}
//keywordConstant: 'true'|'false'|'null'|'this' //keyword
bool CompilationEngine::is_keywordConstant(){
    regex k("true|false|null|this");
    if(regex_match(current_val, k)){
        printToken();
        return true;
    }
    return false;
}
void CompilationEngine::getToken(){
    smatch m;
    getline(fin,token);
    cout<<token<<" ";
    regex e ("<(\\w+)>(\.\*)<\\/\\w+>");
    std::regex_search (token,m,e);
    current_tag=m.str(1);
    current_val=m.str(2);
    cout<<current_tag<<"--"<<current_val<<endl; 
}
void CompilationEngine::printToken(){
    fout<<spaces<<token<<endl;
}
void CompilationEngine::printToken(string tok){
    fout<<spaces<<tok<<endl;
}
bool CompilationEngine::match_regex(string val,string reg,bool flag=true){
    regex k(reg);
    if(regex_match(val, k)){
        if(flag){
            printToken();
        }
        return true;
    }
    return false;
}
CompilationEngine::CompilationEngine(string file){
    //Creates a new compilation engine with the given input and output 
    //The next routine calles must be compileClass
    fin.open(file+"T.xml"); 
    fout.open(file+".xml");
    getline(fin,token);
    getToken();
    spaces="";
}
CompilationEngine::~CompilationEngine(){
    fin.close();
    fout.close();
}
bool CompilationEngine::CompileClass(){
    //Compiles a complete class
    //class:'class' className '{' classVarDec* subroutineDec* '}'
    if(!match_regex(current_val,"class",false)) return false;
    printToken("<class>");
    spaces+="\t";
    printToken();
    getToken();
    if(!is_className()) return false;
    getToken();
    if(!match_regex(current_val,"\\{")) return false;
    getToken();
    while(CompileClassVarDec());
    while(CompileSubroutineDec());
    if(!match_regex(current_val,"\\}")) return false;
    getToken();
    spaces.pop_back();
    printToken("</class>");
    return true;
}
bool CompilationEngine::CompileClassVarDec(){
    //Compiles a static variable declaration , or a field declaration
    //classVarDec:('static'|'field') type varName(','varName)* ';'
    if(!match_regex(current_val,"static|field",false)) return false;
    printToken("<classVarDec>");
    spaces+="\t";
    printToken();
    getToken();
    if(!is_type())return false;
    getToken();
    if(!is_varName()) return false;
    getToken();
    while(true){
        if(match_regex(current_val,"\\,")){
            getToken();
            if(!is_varName()) return false;
            getToken();
        }else break;
    }
    if(!match_regex(current_val,"\\;")) return false;        
    getToken();
    spaces.pop_back();
    printToken("</classVarDec>");
    return true;
}
bool CompilationEngine::CompileSubroutineDec(){
    //Compiles a complete method, function, or constructor
    //subroutineDec: ('constructor'|'function'|'method')('void'|type) subroutineName '('parameterList')' subroutineBody
    if(!match_regex(current_val,"constructor|function|method",false)) return false;
    printToken("<subroutineDec>");
    spaces+="\t";
    printToken();
    getToken();
    if(!(match_regex(current_val,"void")|is_type())) return false;
    getToken();
    if(!is_subroutineName()) return false;
    getToken();
    if(!match_regex(current_val,"\\(")) return false;
    getToken();
    if(!compileParameterList()) return false;
    if(!match_regex(current_val,"\\)")) return false;
    getToken();
    if(!compileSubroutineBody()) return false;    
    spaces.pop_back();
    printToken("</subroutineDec>");
    return true;

}
bool CompilationEngine::compileParameterList(){
    //Compiles a (possibly empty) parameter list 
    //Does not handle the enclosing "()"
    //parameterList: ((type varName) (',' type varName)*)?
    printToken("<parameterList>");
    spaces+="\t";
    if(is_type()){
        getToken();
        if(!is_varName())return false;
        getToken();
        while(true){
            if(match_regex(current_val,"\\,")){
                getToken();
                if(!is_type())return false;
                getToken();
                if(!is_varName()) return false;
                getToken();
            }else break;
        }
    }
    spaces.pop_back();
    printToken("</parameterList>");
    return true;
}
bool CompilationEngine::compileSubroutineBody(){
    //Compiles a subroutine's body
    //subroutineBody: '{' varDec* statements '}'
    printToken("<subroutineBody>");
    spaces+="\t";
    if(!match_regex(current_val,"\\{")) return false;
    getToken();
    while(compileVarDec());
    compileStatements();
    if(!match_regex(current_val,"\\}")) return false;
    getToken();                
    spaces.pop_back();
    printToken("</subroutineBody>");
    return true;
}
bool CompilationEngine::compileVarDec(){
    //Compiles a var declaration
    //varDec: 'var' type varName (','varName)*';'
    if(!match_regex(current_val,"var",false)) return false;
    printToken("<varDec>");
    spaces+="\t";
    printToken();
    getToken();
    if(!is_type()) return false;
    getToken();
    if(!is_varName()) return false;
    getToken();
    while(true){
        if(match_regex(current_val,"\\,")){
            getToken();
            if(!is_varName()) return false;
            getToken();
        }else break;
    }
    if(!match_regex(current_val,"\\;")) return false;
    getToken();
    spaces.pop_back();
    printToken("</varDec>");
    return true;
}
bool CompilationEngine::compileStatements(){
    //Compiles a sequence of statements 
    //Does not handle the enclosing "()"
    //statements: statement*
    //statement: letStatement|ifStatement|whileStatement|doStatement|returnStatement
    printToken("<statements>");
    spaces+="\t";
    while(true){
        //letStatement: 'let' varName ('[' expression ']')? '=' expression ';'
        if(match_regex(current_val,"let",false)){
            printToken("<letStatement>");
            spaces+="\t";
            printToken();
            getToken();
            if(!is_varName()) return false;
            getToken();
            if(match_regex(current_val,"\\[")){
                getToken();
                if(!CompileExpression()) return false;
                if(!match_regex(current_val,"\\]")) return false;
                getToken();
            }
            if(!match_regex(current_val,"\\=")) return false;
            getToken();
            if(!CompileExpression()) return false;
            if(!match_regex(current_val,"\\;")) return false;
            getToken();
            spaces.pop_back();
            printToken("</letStatement>");
        //ifStatement: 'if''('expression')''{' statements '}' ('else' '{' statements '}')?
        } else if(match_regex(current_val,"if",false)){
            printToken("<ifStatement>");
            spaces+="\t";
            printToken();
            getToken();
            if(!match_regex(current_val,"\\(")) return false;
            getToken();
            if(!CompileExpression()) return false;
            if(!match_regex(current_val,"\\)")) return false;
            getToken();
            if(!match_regex(current_val,"\\{")) return false;
            getToken();
            compileStatements();
            if(!match_regex(current_val,"\\}")) return false;
            getToken();
            if(match_regex(current_val,"else")){
                getToken();
                if(!match_regex(current_val,"\\{")) return false;
                getToken();
                compileStatements();
                if(!match_regex(current_val,"\\}")) return false;        
                getToken();
            }
            spaces.pop_back();
            printToken("</ifStatement>");
        //whileStatement: 'while''('expression')''{' statements '}'
        } else if(match_regex(current_val,"while",false)){
            printToken("<whileStatement>");
            spaces+="\t";
            printToken();
            getToken();
            if(!match_regex(current_val,"\\(")) return false;
            getToken();
            if(!CompileExpression()) return false;
            if(!match_regex(current_val,"\\)")) return false;
            getToken();
            if(!match_regex(current_val,"\\{")) return false;
            getToken();
            compileStatements();
            if(!match_regex(current_val,"\\}")) return false;
            getToken();
            spaces.pop_back();
            printToken("</whileStatement>");
        //doStatement: 'do' subroutineCall ';'
        } else if(match_regex(current_val,"do",false)){
            printToken("<doStatement>");
            spaces+="\t";
            printToken();
            getToken();
            if(!CompileSubroutineCall())return false;
            if(!match_regex(current_val,"\\;")) return false;
            getToken();
            spaces.pop_back();
            printToken("</doStatement>");
        //returnStatement: 'return' expression?';'
        } else if(match_regex(current_val,"return",false)){
            printToken("<returnStatement>");
            spaces+="\t";
            printToken();
            getToken();
            if(!match_regex(current_val,"\\;",false)) CompileExpression();
            if(!match_regex(current_val,"\\;")) return false;
            getToken();
            spaces.pop_back();
            printToken("</returnStatement>");
        } else {
            break;
        }
    }
    spaces.pop_back();
    printToken("</statements>");
    return true;
}
bool CompilationEngine::CompileSubroutineCall(){
    //subroutineCall: subroutineName '(' expressionList ')' | (className |varName)'.'subroutineName'('expressionList')' //no markup
    if(is_subroutineName()){//is_subroutineName()|is_className()|is_varName() : All have same definition
        getToken();            
        if(match_regex(current_val,"\\.")){
            getToken();
            if(!is_subroutineName()) return false;
            getToken();
        }
    } else return false;
    if(!match_regex(current_val,"\\(")) return false;
    getToken();
    if(!CompileExpressionList()) return false;
    if(!match_regex(current_val,"\\)")) return false;
    getToken();
    return true;
}
bool CompilationEngine::CompileExpression(){
    //Compiles an expression
    //expression: term (op term)*
    printToken("<expression>");
    spaces+="\t";
    CompileTerm();
    while(true){
        if(is_op()){
            getToken();
            CompileTerm();
        }else break;
    }
    spaces.pop_back();
    printToken("</expression>");
    return true;
}
bool CompilationEngine::CompileTerm(){
    //Compiles a term 
    //If the current token is an identifier, the routine must distinguish between a variable, an array entry, or a subroutine call.
    //A single look-ahead token, which may be one of "[","(",or".",suffices to distinguish between the possibilities
    //Any other token is not part of this term and should not be advane over
    //term: integerConstant | stringConstant | keywordConstant | varName | varName'['expression']' | subroutineCall | '(' expression ')' | unaryOp term
    printToken("<term>");
    spaces+="\t";
    if(match_regex(current_tag,"integerConstant")|match_regex(current_tag,"stringConstant")|is_keywordConstant()){
        getToken();
    } else if(is_varName()){//is_subroutineName()|is_className()|is_varName() : All have same definition 
        getToken();
        //subroutineCall: subroutineName '(' expressionList ')' | (className |varName)'.'subroutineName'('expressionList')' //no markup
        if(match_regex(current_val,"\\.")){
            getToken();
            if(!is_subroutineName()) return false;
            getToken();
        }
        if(match_regex(current_val,"\\(")){
            getToken();
            if(!CompileExpressionList()) return false;
            if(!match_regex(current_val,"\\)")) return false;
            getToken();
        } else if(match_regex(current_val,"\\[")){
            getToken();
            CompileExpression();
            if(!match_regex(current_val,"\\]")) return false;
            getToken();  
        }
    } else if(match_regex(current_val,"\\(")){
        getToken();
        CompileExpression();
        if(!match_regex(current_val,"\\)")) return false;
        getToken();
    } else if(is_unaryOp()){
        getToken();
        if(!CompileTerm()) return false;
    } 
    spaces.pop_back();
    printToken("</term>");
    return true;
}
bool CompilationEngine::CompileExpressionList(){
    //Compiles a (possible empty) comma-separated list of expressions
    //expressionList: (expression(','expression)*)?
    printToken("<expressionList>");
    spaces+="\t";
    if(!match_regex(current_val,"\\)",false) && CompileExpression()){
        while(true){
            if(match_regex(current_val,"\\,")){
                getToken();
                if(!CompileExpression()) return false;
            }else break;
        }
    }
    spaces.pop_back();
    printToken("</expressionList>");
    return true;
}
// int main(int argc, char *argv[]){
//     //prompt> JackAnalyzer input
//     //Input : finename.jack: name of single source file , or 
//     //         directoryName: name of a directory containing one or more .jack source files 
//     //Output: if the input is a single file: fileName.xml
//     // if the input is a directory: one .xml file for every .jack file. stored in the same directory
//     vector<string> files;
//     string wpath;
//     string path=argv[1];
//     if(path.find(".jack")==string::npos){
//         // cout<<"Dir";
//         //its a directory
//         DIR * directory;   // creating pointer of type dirent
//         struct dirent *x;   // pointer represent directory stream
//         if (( directory= opendir (path.c_str())) != NULL){      // check if directory  open
//             while ((x = readdir (directory)) != NULL) {
//                 string fileName=x->d_name;
//                 // cout<<fileName<<endl;
//                 if(fileName.find(".jack")!= string::npos){
//                     //found vm file
//                     // cout<<"DIR"<<endl;
//                     int found=fileName.find(".jack");
//                     files.push_back(path+"/"+fileName.substr(0,found));
//                 }
//             }
//         }
//         closedir (directory); //close directory.... 
//     }else{
//         // cout<<"File";
//         // cout<<path;
//         int found=path.find(".jack");
//         path=path.substr(0,found);
//         // cout<<wpath<<found<<endl;
//         files.push_back(path);
//     }
//     for(int i=0;i<files.size();i++){
//         cout<<files[i]<<endl;
//         JackTokenizer * tokenize=new JackTokenizer(files[i]);
//         while(tokenize->hasMoreTokens()){
//             tokenize->advance();
//             switch(tokenize->tokenType()){
//                 case KEYWORD:tokenize->keyWord();break;
//                 case SYMBOL:tokenize->symbol();break;
//                 case IDENTIFIER:tokenize->identifier();break;
//                 case INT_CONST:tokenize->intVal();break;
//                 case STRING_CONST:tokenize->stringVal();break;
//             }
//         }
//         delete(tokenize);
//         //compile class
//         CompilationEngine * engine=new CompilationEngine(files[i]);
//         engine->CompileClass();
//         delete(engine);
//     }
// }