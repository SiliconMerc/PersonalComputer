#include<fstream>
#include<iostream>
#include<string>
#include<dirent.h>
#include <regex>
#include<vector>
#include<map>
using namespace std;
enum keywords_e{CLASS,METHOD,FUNCTION,CONSTRUCTOR,INT,BOOLEAN,CHAR,VOID,VAR,STATIC,FIELD,LET,DO,IF,ELSE,WHILE,RETURN,TRUE,FALSE,NULL_PTR,THIS};
enum tokens_e {KEYWORD,SYMBOL,IDENTIFIER,INT_CONST,STRING_CONST,NONE};
class JackTokenizer{
    ifstream fin;
    ofstream fout;
    string current_token;
    int  count;
    string str;
    public:
    JackTokenizer(string file){
        //opens the input .jack file and gets read to tokenize it 
        fin.open(file+".jack");
        fout.open(file+"T.xml");
        current_token="";
        str="";
        count=0;
        fout<<"<tokens>\n";
    }
    ~JackTokenizer(){
        fout<<"</tokens>\n";
        fin.close();
        fout.close();
    }
    string trimString(string s){
        //from start
        while(s[0]==' ' | s[0] == 9){
            s.erase(s.begin());
        }
        //from end
        // cout<<(int)s[0]<<endl;        
        while(s[s.length()-1]==' '| s[s.length()-1]==13 | s[s.length()-1] == 9){
            s.pop_back();
        }
        return s;
    }
    bool hasMoreTokens(){
        //are there more tokens in the input?
        return !fin.eof();
    }
    void advance(){
        //gets the next token from the input , and makes it the current token 
        //this method should be called only if hasMoreTokens is true
        //initially there is no current token
        // cout<<str<<endl;
        str=trimString(str);
        if(count>=str.size()){
            if(current_token.size()!=0){
                //error
                cout<<"ERROR : unidentified token "<<current_token<<"in line "<<str<<endl;
                current_token="";
            }
            if(count >= str.size()){
                //reached end
                getline(fin,str);
                count=0;
                //remove comments
                // cout<<str<<str.size()<<endl;
                for(int i=0;i<(int)str.size()-1;i++){
                    if(str[i]=='/'&& str[i+1]=='/'){
                        str=str.substr(0,i);
                        break;
                    }
                }
                for(int i=0;i<(int)str.size()-1;i++){
                    if(str[i]=='/'&& str[i+1]=='*'){
                        //multi line comment
                        bool flag=true;
                        int k=i+2;
                        while(flag && hasMoreTokens()){
                            for(int j=k;j<(int)str.size()-1;j++){
                                if(str[j]=='*'&& str[j+1]=='/'){
                                    flag=false;
                                    if(j+2 < str.size() ){
                                        str=str.substr(j+2);
                                    } else {
                                        str="";
                                    }
                                    break;
                                }
                            }
                            k=0;
                            //move to next line
                            getline(fin,str);
                            // cout<<str<<endl;
                            count=0;
                        }
                        break;
                    }
                }
                //remove whitespaces and comments
                str=trimString(str);
            }
        }
        regex symbol("\\{|\\}|\\(|\\)|\\[|\\]|\\.|\\,|\\;|\\+|\\-|\\*|\\/|\\&|\\||\\<|\\>|\\=|\\~");
        if(str[count] == '"' | str[count] == '\''){
            //string constant
            // cout<<"FOUND bracket";
            if(current_token.size()!=0){
                cout<<"ERROR : unidentified token "<<current_token<<"in line "<<str<<endl;
            }
            current_token+=str[count++];
            for(;count<str.size();count++){
                current_token+=str[count];
                if(str[count] == '"' | str[count] == '\''){
                    count++;
                    break;
                }
            }
        } else if(regex_match(string(1,str[count]),symbol)){
            //symbol
            current_token=str[count++];
        } else {
            for(;count<str.size();count++){
                //get the token
                //tokens ends on symbol or whitespace
                if(regex_match(string(1,str[count]),symbol) | str[count]==' ' ){
                    break;
                }else{
                    current_token+=str[count];
                }
            }
        }
        for(;count<str.size();count++){
            if(str[count]==' ') continue;
            else break;
        }
        current_token=trimString(current_token);
        // cout<<current_token<<endl;
    }
    tokens_e tokenType(){
        if(current_token.size()==0){
            return NONE;
        }
        //Returns the type of the current token
        try{
            // cout<<"-"<<current_token<<"-"<<endl;
            regex keyword("class|constructor|function|method|field|static|var|int|char|boolean|void|true|false|null|this|let|do|if|else|while|return");
            regex symbol("\\{|\\}|\\(|\\)|\\[|\\]|\\.|\\,|\\;|\\+|\\-|\\*|\\/|\\&|\\||\\<|\\>|\\=|\\~");
            regex integerConstant("\\d+");
            regex stringConstant("[\"\'][^\'\"]*[\'\"]");
            regex identifier("[a-zA-Z_][a-zA-Z_0-9]*");
            if(regex_match(current_token, keyword)){
                return KEYWORD;
            } else if(regex_match(current_token,symbol)){
                return SYMBOL;
            } else if(regex_match(current_token,integerConstant)){
                return INT_CONST;
            } else if(regex_match(current_token,stringConstant)){
                return STRING_CONST;
            } else if(regex_match(current_token,identifier)){
                return IDENTIFIER;
            } else {
                return NONE;
            }
        }
        catch (const std::regex_error& e) {
            std::cout << "regex_error caught: " << e.what() << '\n';
            if (e.code() == std::regex_constants::error_brack) {
                std::cout << "The code was error_brack\n";
            }
        }
        return NONE;
    }
    keywords_e keyWord(){
        //Retuns the keyworks which is the current token as a constant
        //this method should be called only if tokenType is KEYWORD
        string token=current_token;
        current_token="";
        fout<<"<keyword>"<<token<<"</keyword>\n";
        map<string,keywords_e> keyword;
        keyword.insert(pair<string,keywords_e>());
        keyword.insert(pair<string,keywords_e>("class",CLASS));
        keyword.insert(pair<string,keywords_e>("constructor",CONSTRUCTOR));
        keyword.insert(pair<string,keywords_e>("function",FUNCTION));
        keyword.insert(pair<string,keywords_e>("method",METHOD));
        keyword.insert(pair<string,keywords_e>("field",FIELD));
        keyword.insert(pair<string,keywords_e>("static",STATIC));
        keyword.insert(pair<string,keywords_e>("var",VAR));
        keyword.insert(pair<string,keywords_e>("int",INT));
        keyword.insert(pair<string,keywords_e>("boolean",BOOLEAN));
        keyword.insert(pair<string,keywords_e>("void",VOID));
        keyword.insert(pair<string,keywords_e>("true",TRUE));
        keyword.insert(pair<string,keywords_e>("false",FALSE));
        keyword.insert(pair<string,keywords_e>("null",NULL_PTR));
        keyword.insert(pair<string,keywords_e>("this",THIS));
        keyword.insert(pair<string,keywords_e>("let",LET));
        keyword.insert(pair<string,keywords_e>("do",DO));
        keyword.insert(pair<string,keywords_e>("if",IF));
        keyword.insert(pair<string,keywords_e>("else",ELSE));
        keyword.insert(pair<string,keywords_e>("while",WHILE));
        keyword.insert(pair<string,keywords_e>("return",RETURN));
        return keyword[token];
    }
    char symbol(){
        //Returns the characted which is the current token . 
        //Should be called only if tokenType is SYMBOL.
        string token=current_token;
        string xml_token=current_token;
        current_token="";
        if(xml_token=="<"){
            xml_token="&lt;";
        }else if(xml_token==">"){
            xml_token="&gt;";
        }else if(xml_token=="\""){
            xml_token="&quot;";
        }else if(xml_token=="&"){
            xml_token="&amp;";
        }
        fout<<"<symbol>"<<xml_token<<"</symbol>\n";
        return token[0];
    }
    string identifier(){
        //Returns the identifier which is the current token . 
        //Should be called only if tokenType is IDENTIFIER
        string token=current_token;
        current_token="";
        fout<<"<identifier>"<<token<<"</identifier>\n";
        return token;
    }
    int intVal(){
        //Returns the integer value of the current token . 
        //Should be called only if tokenType is INT_CONST
        string token=current_token;
        current_token="";
        fout<<"<integerConstant>"<<token<<"</integerConstant>\n";
        return stoi(token);
    }
    string stringVal(){
        //Returns the string value of the current token , without the two enclosing doublr quotes
        //Should be called only if tokenType is STRING_CONST
        string token=current_token;
        current_token="";
        fout<<"<stringConstant>"<<token.substr(1,token.length()-2)<<"</stringConstant>\n";
        return token.substr(1,token.length()-1);
    }
};
class CompilationEngine{
    ifstream fin;
    ofstream fout;
    string spaces;
    string token;
    string current_tag;
    string current_val;
    map<string,string> expr;
    //type: 'int'|'char'|'boolean'|className //no markup
    bool is_type(){
        if(current_val=="int" | current_val=="char" | current_val=="boolean"){
            printToken();
            return true;
        } else if(is_className()){
            return true;
        }
        return false;
    }
    //className: identifier //no markup
    bool is_className(){
        if(current_tag=="identifier"){
            printToken();
            return true;
        }
        return false;
    }
    //subroutineName: identifier //no markup
    bool is_subroutineName(){
        if(current_tag=="identifier"){
            printToken();
            return true;
        }
        return false;
    }
    //varName: identifier //no markup
    bool is_varName(){
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
    bool is_op(){
        regex k("\\+|\\-|\\*|\\/|\\&|\\||\\<|\\>|\\=|\\&lt\\;|\\&gt\\;|\\&quot\\;|\\&amp\\;");
        if(regex_match(current_val, k)){
            printToken();
            return true;
        }
        // printToken(current_val);
        return false;
    }
    //unaryOp: '-'|'~' //symbol
    bool is_unaryOp(){
        regex k("\\-|\\~");
        if(regex_match(current_val, k)){
            printToken();
            return true;
        }
        return false;
    }
    //keywordConstant: 'true'|'false'|'null'|'this' //keyword
    bool is_keywordConstant(){
        regex k("true|false|null|this");
        if(regex_match(current_val, k)){
            printToken();
            return true;
        }
        return false;
    }
    void getToken(){
        smatch m;
        getline(fin,token);
        cout<<token<<" ";
        regex e ("<(\\w+)>(\.\*)<\\/\\w+>");
        std::regex_search (token,m,e);
        current_tag=m.str(1);
        current_val=m.str(2);
        cout<<current_tag<<"--"<<current_val<<endl; 
    }
    void printToken(){
        fout<<spaces<<token<<endl;
    }
    void printToken(string tok){
        fout<<spaces<<tok<<endl;
    }
    bool match_regex(string val,string reg,bool flag=true){
        regex k(reg);
        if(regex_match(val, k)){
            if(flag){
                printToken();
            }
            return true;
        }
        return false;
    }
    public:
    CompilationEngine(string file){
        //Creates a new compilation engine with the given input and output 
        //The next routine calles must be compileClass
        fin.open(file+"T.xml"); 
        fout.open(file+".xml");
        getline(fin,token);
        getToken();
        spaces="";
    }
    ~CompilationEngine(){
        fin.close();
        fout.close();
    }
    bool CompileClass(){
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
    bool CompileClassVarDec(){
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
    bool CompileSubroutineDec(){
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
    bool compileParameterList(){
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
    bool compileSubroutineBody(){
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
    bool compileVarDec(){
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
    bool compileStatements(){
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
    bool CompileSubroutineCall(){
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
    bool CompileExpression(){
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
    bool CompileTerm(){
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
    bool CompileExpressionList(){
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
};
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