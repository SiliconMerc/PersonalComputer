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
    // cout<<token<<" ";
    regex e ("<(\\w+)>(.*)<\\/\\w+>");
    std::regex_search (token,m,e);
    current_tag=m.str(1);
    current_val=m.str(2);
    // cout<<current_tag<<"--"<<current_val<<endl; 
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
    vmWrite=new VMWriter(file);
    label_Lx=0;
}
CompilationEngine::~CompilationEngine(){
    fin.close();
    fout.close();
}
bool CompilationEngine::CompileClass(){
    //Compiles a complete class
    //class:'class' className '{' classVarDec* subroutineDec* '}'
    // 'class'
    if(!match_regex(current_val,"class",false)) return false;
    printToken("<class>");
    spaces+="\t";
    printToken();
    class_level=new SymbolTable();
    //className
    getToken();
    if(!is_className()) return false;
    className=current_val;
    //'{'
    getToken();
    if(!match_regex(current_val,"\\{")) return false;
    // classVarDec*
    getToken();
    while(CompileClassVarDec());
    //subroutineDec*
    while(CompileSubroutineDec());
    //'}'
    if(!match_regex(current_val,"\\}")) return false;
    getToken();
    spaces.pop_back();
    printToken("</class>");
    delete(class_level);
    class_level=nullptr;
    return true;
}
SYMBOL_E::fields_e str2enum_fields(string kind){
    if(kind=="static"){
        return SYMBOL_E::STATIC_F;
    }else if(kind=="field"){
        return SYMBOL_E::FIELD;
    }else if(kind=="argument"){
        return SYMBOL_E::ARG_F;
    }else if(kind=="var"){
        return SYMBOL_E::VAR;
    }
    return SYMBOL_E::NONE;
}
bool CompilationEngine::CompileClassVarDec(){
    //Compiles a static variable declaration , or a field declaration
    //classVarDec:('static'|'field') type varName(','varName)* ';'
    if(!match_regex(current_val,"static|field",false)) return false;
    printToken("<classVarDec>");
    spaces+="\t";
    //('static'|'field')
    SYMBOL_E::fields_e kind=str2enum_fields(current_val);
    printToken();getToken();
    //type
    if(!is_type())return false;
    string type=current_val;
    getToken();
    //varName
    if(!is_varName()) return false;
    string varName=current_val;
    class_level->define(varName,type,kind);
    //(','varName)*
    getToken();
    while(true){
        if(match_regex(current_val,"\\,")){
            getToken();
            if(!is_varName()) return false;
            varName=current_val;
            class_level->define(varName,type,kind);
            getToken();
        }else break;
    }
    //';'
    if(!match_regex(current_val,"\\;")) return false;        
    getToken();
    spaces.pop_back();
    printToken("</classVarDec>");
    return true;
}
bool CompilationEngine::CompileSubroutineDec(){
    //Compiles a complete method, function, or constructor
    //subroutineDec: ('constructor'|'function'|'method')('void'|type) subroutineName '('parameterList')' subroutineBody
    //('constructor'|'function'|'method')
    if(!match_regex(current_val,"constructor|function|method",false)) return false;
    printToken("<subroutineDec>");
    spaces+="\t";
    printToken();
    subroutine_level=new SymbolTable();
    string fType=current_val;
    if(fType=="method") subroutine_level->define("this",className,SYMBOL_E::ARG_F);
    getToken();
    //('void'|type)
    if(!(match_regex(current_val,"void")|is_type())) return false;
    getToken();
    //subroutineName
    if(!is_subroutineName()) return false;
    string fName=current_val;
    getToken();
    //'('
    if(!match_regex(current_val,"\\(")) return false;
    getToken();
    //parameterList
    compileParameterList();
    //')' 
    if(!match_regex(current_val,"\\)")) return false;
    getToken();
    //subroutineBody
    if(!compileSubroutineBody(fType,fName)) return false;    
    spaces.pop_back();
    printToken("</subroutineDec>");
    delete(subroutine_level);
    subroutine_level=nullptr;
    return true;

}
int CompilationEngine::compileParameterList(){
    //Compiles a (possibly empty) parameter list 
    //Does not handle the enclosing "()"
    //parameterList: ((type varName) (',' type varName)*)?
    printToken("<parameterList>");
    spaces+="\t";
    int nLocals=0;
    //?
    if(is_type()){
        string type=current_val;
        getToken();
        //(type varName)
        if(!is_varName())return false;
        string varName=current_val;
        SYMBOL_E::fields_e kind=str2enum_fields("argument");
        subroutine_level->define(varName,type,kind);
        nLocals+=1;
        getToken();
        //(',' type varName)*
        while(true){
            if(match_regex(current_val,"\\,")){
                getToken();
                if(!is_type())return false;
                type=current_val;
                getToken();
                if(!is_varName()) return false;
                varName=current_val;
                subroutine_level->define(varName,type,kind);
                nLocals+=1;
                getToken();
            }else break;
        }
    }
    spaces.pop_back();
    printToken("</parameterList>");
    return nLocals;
}
bool CompilationEngine::compileSubroutineBody(string fType,string fName){
    //Compiles a subroutine's body
    //subroutineBody: '{' varDec* statements '}'
    printToken("<subroutineBody>");
    spaces+="\t";
    //'{' 
    if(!match_regex(current_val,"\\{")) return false;
    getToken();
    //varDec* 
    int nLocals=0;
    while(true){
        int count=compileVarDec();
        if(count==-1){
            break;
        }else{
            nLocals+=count;
        }
    };
    vmWrite->writeFunction(className+"."+fName,nLocals);
    if(fType=="constructor"){
        vmWrite->writePush(SYMBOL_E::CONSTANT,class_level->VarCount(SYMBOL_E::FIELD));//figure out the sixe of the object and push the size
        vmWrite->writeCall("Memory.alloc",1);//calls Memory.alloc(n) , Finds a memory block of the required size,and returns its base address
        vmWrite->writePop(SYMBOL_E::POINTER,0);//returns the base address
    }else if(fType=="method") {
        //associate the this memory segement with the object on which the method is called to operate
        vmWrite->writePush(SYMBOL_E::ARG_S,0);
        vmWrite->writePop(SYMBOL_E::POINTER,0);
    }else if(fType=="function"){

    }
    //statements 
    if(!compileStatements()) cout<<"ERROR";
    //'}'
    if(!match_regex(current_val,"\\}")) return false;
    getToken();                
    spaces.pop_back();
    printToken("</subroutineBody>");
    return true;
}
int CompilationEngine::compileVarDec(){
    //Compiles a var declaration
    //varDec: 'var' type varName (','varName)*';'
    //'var' 
    if(!match_regex(current_val,"var",false)) return -1;
    printToken("<varDec>");
    spaces+="\t";
    printToken();
    SYMBOL_E::fields_e kind=str2enum_fields("var");
    getToken();
    //type 
    if(!is_type()) return -1;
    string type=current_val;
    getToken();
    int nLocals=0;
    //varName 
    if(!is_varName()) return -1;
    string varName=current_val;
    subroutine_level->define(varName,type,kind);
    nLocals+=1;
    getToken();
    //(','varName)*
    while(true){
        if(match_regex(current_val,"\\,")){
            getToken();
            if(!is_varName()) return -1;
            varName=current_val;
            subroutine_level->define(varName,type,kind);
            nLocals+=1;
            getToken();
        }else break;
    }
    //';'
    if(!match_regex(current_val,"\\;")) return -1;
    getToken();
    spaces.pop_back();
    printToken("</varDec>");
    return nLocals;
}
string CompilationEngine::getVarType(string varName){
    int index;
    string sym=subroutine_level->TypeOf(varName);
    if(sym.empty()){
        return class_level->TypeOf(varName);
    }else{
        return sym;
    }
}
SYMBOL_E::fields_e CompilationEngine::getVarKind(string varName){
    int index;
    SYMBOL_E::fields_e sym=subroutine_level->KindOf(varName);
    if(sym==SYMBOL_E::NONE){
        return class_level->KindOf(varName);
    }else{
        return sym;
    }
}
int CompilationEngine::getVarIndex(string varName){
    int index;
    SYMBOL_E::fields_e sym=subroutine_level->KindOf(varName);
    if(sym==SYMBOL_E::NONE){
        sym=class_level->KindOf(varName);
        if(sym==SYMBOL_E::NONE){
            // cout<<"REACHED NONE"<<endl;
            return -1;
        }else{
            index=class_level->IndexOf(varName);
        }
    }else{
        index=subroutine_level->IndexOf(varName);
    }
    return index;
}
bool CompilationEngine::compileStatements(){
    //Compiles a sequence of statements 
    //Does not handle the enclosing "()"
    //statements: statement*
    //statement: letStatement|ifStatement|whileStatement|doStatement|returnStatement
    printToken("<statements>");
    spaces+="\t";
    while(true){
        // cout<<current_tag<<" "<<current_val<<endl;
        //letStatement: 'let' varName ('[' expression ']')? '=' expression ';'
        if(match_regex(current_val,"let",false)){
            printToken("<letStatement>");
            spaces+="\t";
            printToken();
            getToken();
            // cout<<"P1";
            if(!is_varName()) return false;
            string varName=current_val;
            getToken();
            bool is_array_access=false;
            if(match_regex(current_val,"\\[")){
                //array access
                is_array_access=true;
                //arr[expression1]=expression2
                //push arr
                SYMBOL_E::fields_e sym=getVarKind(varName);
                if(sym==SYMBOL_E::NONE) return false;
                int index=getVarIndex(varName);
                if(index==-1) return false;
                vmWrite->writePush(enum2enum_fields_segments(sym),index);
                //vm code for computing and pushing the value of expression
                getToken();
                if(!CompileExpression()) return false;
                if(!match_regex(current_val,"\\]")) return false;
                //add
                vmWrite->writeArithmetic(SYMBOL_E::ADD);//top stack value=RAM address of arr[expression1]
                getToken();
            }
            // cout<<"P2";
            if(!match_regex(current_val,"\\=")) return false;
            getToken();
            // cout<<"P3";
            //vm code for comuting and pushing the value of expression2
            if(!CompileExpression()) return false;
            // cout<<"P4";
            if(!match_regex(current_val,"\\;")) return false;
            if(!is_array_access){
                SYMBOL_E::fields_e sym=getVarKind(varName);
                // cout<<"P5";
                // cout<<varName;
                if(sym==SYMBOL_E::NONE) return false;
                int index=getVarIndex(varName);
                // cout<<"P6";
                if(index==-1) return false;
                vmWrite->writePop(enum2enum_fields_segments(sym),index);
            }else{
                vmWrite->writePop(SYMBOL_E::TEMP,0);//temp 0 = the value of expression
                //top stack value = RAM address of arr[expression]
                vmWrite->writePop(SYMBOL_E::POINTER,1);
                vmWrite->writePush(SYMBOL_E::TEMP,0);
                vmWrite->writePop(SYMBOL_E::THAT,0);
            }
            getToken();
            spaces.pop_back();
            printToken("</letStatement>");
        //ifStatement: 'if''('expression')''{' statements '}' ('else' '{' statements '}')?
        } else if(match_regex(current_val,"if",false)){
            printToken("<ifStatement>");
            spaces+="\t";
            string L1="IF_FALSE"+to_string(label_Lx++);
            string L2="IF_TRUE"+to_string(label_Lx++);
            printToken();
            getToken();
            //copiled expression
            if(!match_regex(current_val,"\\(")) return false;
            getToken();
            if(!CompileExpression()) return false;
            vmWrite->writeArithmetic(SYMBOL_E::NOT);//not
            if(!match_regex(current_val,"\\)")) return false;
            vmWrite->writeIf(L1);//if go to L1
            getToken();
            if(!match_regex(current_val,"\\{")) return false;
            getToken();
            compileStatements();//compiled statements
            if(!match_regex(current_val,"\\}")) return false;
            getToken();
            if(match_regex(current_val,"else")){
                vmWrite->writeGoto(L2);//goto L2
                vmWrite->writeLabel(L1);//label L1            
                getToken();
                if(!match_regex(current_val,"\\{")) return false;
                getToken();
                compileStatements();//compiled statements
                if(!match_regex(current_val,"\\}")) return false;        
                getToken();
                vmWrite->writeLabel(L2);//label L2
            }else{
                vmWrite->writeLabel(L1);//label L1
            }
            spaces.pop_back();
            printToken("</ifStatement>");
        //whileStatement: 'while''('expression')''{' statements '}'
        } else if(match_regex(current_val,"while",false)){
            printToken("<whileStatement>");
            spaces+="\t";
            printToken();
            string L1="WHILE_EXP"+to_string(label_Lx++);
            string L2="WHILE_END"+to_string(label_Lx++);
            vmWrite->writeLabel(L1);//label L1
            getToken();
            if(!match_regex(current_val,"\\(")) return false;
            getToken();
            if(!CompileExpression()) return false;//compiled expression
            if(!match_regex(current_val,"\\)")) return false;
            getToken();
            vmWrite->writeArithmetic(SYMBOL_E::NOT);//not
            vmWrite->writeIf(L2);//if go to L2
            if(!match_regex(current_val,"\\{")) return false;
            getToken();
            compileStatements();//compiled statements
            if(!match_regex(current_val,"\\}")) return false;
            vmWrite->writeGoto(L1);//goto L1
            vmWrite->writeLabel(L2);//label L2
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
            // cout<<"P2"<<endl;
            if(!match_regex(current_val,"\\;")) return false;
            vmWrite->writePop(SYMBOL_E::TEMP,0);
            getToken();
            spaces.pop_back();
            printToken("</doStatement>");
        //returnStatement: 'return' expression?';'
        } else if(match_regex(current_val,"return",false)){
            printToken("<returnStatement>");
            spaces+="\t";
            printToken();
            getToken();
            if(match_regex(current_val,"\\;",false)){
                //void return
                vmWrite->writePush(SYMBOL_E::CONSTANT,0);
            }else{
                CompileExpression();
            }
            if(!match_regex(current_val,"\\;")) return false;
            vmWrite->writeReturn();
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
bool CompilationEngine::CompileSubroutineCallWrite(string var1,string var2){
    if(var2!=""){
        //(className |varName)'.'subroutineName'('expressionList')'
        //object function call 
        //push the object on which the method is called to operate
        string type=getVarType(var1);
        if(!type.empty()){
            //its a varName
            SYMBOL_E::fields_e sym=getVarKind(var1);
            if(sym==SYMBOL_E::NONE) return false;
            int index=getVarIndex(var1);
            if(index==-1) return false;
            vmWrite->writePush(enum2enum_fields_segments(sym),index);
        }
    }else{
        // method call
        vmWrite->writePush(SYMBOL_E::POINTER,0);//push this
    }
    //'('expressionList')'
    if(!match_regex(current_val,"\\(")) return false;
    getToken();
    int nArgs=CompileExpressionList();//pushes the method/function arguments
    if(!match_regex(current_val,"\\)")) return false;
    if(var2==""){
        // subroutineName '(' expressionList ')'
        // method call
        vmWrite->writeCall(className+"."+var1,nArgs+1);
    }else{
        //(className |varName)'.'subroutineName'('expressionList')'
        //function call
        string type=getVarType(var1);
        if(type.empty()){
            //its a className
            vmWrite->writeCall(var1+"."+var2,nArgs);
        }else{
            //its a varName
            vmWrite->writeCall(type+"."+var2,nArgs+1);
        }
    }
    getToken();
    return true;
}
bool CompilationEngine::CompileSubroutineCall(){
    //subroutineCall: subroutineName '(' expressionList ')' | (className |varName)'.'subroutineName'('expressionList')' //no markup
    //subroutineName|(className |varName)'.'subroutineName'
    string var1,var2="";
    if(is_subroutineName()){//is_subroutineName()|is_className()|is_varName() : All have same definition
        var1=current_val;
        getToken();            
        if(match_regex(current_val,"\\.")){
            getToken();
            if(!is_subroutineName()) return false;
            var2=current_val;
            getToken();
        }
    } else return false;
    if(!CompileSubroutineCallWrite(var1,var2))return false;
    return true;
}
SYMBOL_E::arith_e CompilationEngine::str2enum_arith(string op){
    if(op=="+") return SYMBOL_E::ADD;
    else if(op=="-")  return SYMBOL_E::SUB;
    else if(op=="*")  return SYMBOL_E::MUL;
    else if(op=="/")  return SYMBOL_E::DIV;
    else if(op=="&")  return SYMBOL_E::AND;
    else if(op=="|")  return SYMBOL_E::OR;
    else if(op=="<")  return SYMBOL_E::LT;
    else if(op==">")  return SYMBOL_E::GT;
    else if(op=="=")  return SYMBOL_E::EQ;
    else if(op=="&lt;")  return SYMBOL_E::LT;
    else if(op=="&gt;")  return SYMBOL_E::GT;
    // else if(op=="&quot;")  return SYMBOL_E::NOT;
    else if(op=="&amp;")  return SYMBOL_E::AND;
}
SYMBOL_E::segment_e CompilationEngine::enum2enum_fields_segments(SYMBOL_E::fields_e kind){
    switch(kind){
        case SYMBOL_E::STATIC_F: return SYMBOL_E::STATIC_S;
        case SYMBOL_E::FIELD: return SYMBOL_E::THIS;
        case SYMBOL_E::ARG_F: return SYMBOL_E::ARG_S;
        case SYMBOL_E::VAR: return SYMBOL_E::LOCAL;
    }
}
bool CompilationEngine::CompileExpression(){
    //Compiles an expression
    //expression: term (op term)*
    printToken("<expression>");
    spaces+="\t";
    CompileTerm();
    while(true){
        if(is_op()){
            string op=current_val;
            getToken();
            CompileTerm();
            // cout<<"SS "<<op<<endl;
            vmWrite->writeArithmetic(str2enum_arith(op));
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
    //Any other token is not part of this term and should not be advance over
    //term: integerConstant | stringConstant | keywordConstant | varName | varName'['expression']' | subroutineCall | '(' expression ')' | unaryOp term
    printToken("<term>");
    spaces+="\t";
    if(match_regex(current_tag,"integerConstant")|match_regex(current_tag,"stringConstant")|is_keywordConstant()){
        try{
            //integer constant
            int x=stoi(current_val);
            vmWrite->writePush(SYMBOL_E::CONSTANT,x);
        }
        catch (...){
            // std::invalid_argument
            //not integer constant
            if(is_keywordConstant()){
                if(current_val=="true"){
                    vmWrite->writePush(SYMBOL_E::CONSTANT,0);
                    vmWrite->writeArithmetic(SYMBOL_E::NOT);
                }else if(current_val=="false"){
                    vmWrite->writePush(SYMBOL_E::CONSTANT,0);
                }else if(current_val=="null"){
                    vmWrite->writePush(SYMBOL_E::CONSTANT,0);
                }else if(current_val=="this"){
                    vmWrite->writePush(SYMBOL_E::POINTER,0);
                }
            }else{
                //string constant
                //make a new string
                //string constant are created using String.new(length)
                vmWrite->writePush(SYMBOL_E::CONSTANT,current_val.length());
                vmWrite->writeCall("String.new",1);
                //for x="cc...c" are handled using a series of calls to String.appendChar(c)
                for(int i=0;i<current_val.length();i++){
                    vmWrite->writePush(SYMBOL_E::CONSTANT,(int)current_val[i]);
                    vmWrite->writeCall("String.appendChar",2);
                }
            }
        }
        
        getToken();
    } else if(is_varName()){//is_subroutineName()|is_className()|is_varName() : All have same definition 
        string varName=current_val;
        string var2="";
        getToken();
        //subroutineCall: subroutineName '(' expressionList ')' | (className |varName)'.'subroutineName'('expressionList')' //no markup
        if(match_regex(current_val,"\\.")){
            getToken();
            if(!is_subroutineName()) return false;
            var2=current_val;
            getToken();
        }
        if(match_regex(current_val,"\\(")){
            //call f
            if(!CompileSubroutineCallWrite(varName,var2))return false;
        } else if(match_regex(current_val,"\\[")){
            //array access
            //b[j]
            //push b
            SYMBOL_E::fields_e sym=getVarKind(varName);
            if(sym==SYMBOL_E::NONE) return false;
            int index=getVarIndex(varName);
            if(index==-1) return false;
            vmWrite->writePush(enum2enum_fields_segments(sym),index);
            //push j
            getToken();
            CompileExpression();
            if(!match_regex(current_val,"\\]")) return false;
            //add
            vmWrite->writeArithmetic(SYMBOL_E::ADD);
            //pop pointer 1
            vmWrite->writePop(SYMBOL_E::POINTER,1);
            //push that 0
            vmWrite->writePush(SYMBOL_E::THAT,0);
            getToken();  
        } else{
            //its just a variable 
            // find in symbol table
            // number : push n
            // variable : push var   
            // cout<<varName;     
            SYMBOL_E::fields_e sym=getVarKind(varName);
            if(sym==SYMBOL_E::NONE) return false;
            int index=getVarIndex(varName);
            if(index==-1) return false;
            vmWrite->writePush(enum2enum_fields_segments(sym),index);
        }
    } else if(match_regex(current_val,"\\(")){
        getToken();
        CompileExpression();
        if(!match_regex(current_val,"\\)")) return false;
        getToken();
    } else if(is_unaryOp()){
        string op=current_val;
        getToken();
        if(!CompileTerm()) return false;
        if(op=="-") vmWrite->writeArithmetic(SYMBOL_E::NEG);
        if(op=="~") vmWrite->writeArithmetic(SYMBOL_E::NOT);
    } 
    spaces.pop_back();
    printToken("</term>");
    return true;
}
int CompilationEngine::CompileExpressionList(){
    //Compiles a (possible empty) comma-separated list of expressions
    //expressionList: (expression(','expression)*)?
    printToken("<expressionList>");
    spaces+="\t";
    int nArgs=0;
    if(!match_regex(current_val,"\\)",false)){
        nArgs+=1; 
        CompileExpression();
        while(true){
            if(match_regex(current_val,"\\,")){
                nArgs+=1;
                getToken();
                if(!CompileExpression()) return false;
            }else break;
        }
    }
    spaces.pop_back();
    printToken("</expressionList>");
    return nArgs;
}

//creates a new symbol table
SymbolTable::SymbolTable(){
    
}
//defines a new identifier of the given name,type, and kind,and assigns it a running index.
//STATIC and FIELD indentifiers have a class scope , while ARG and VAR identifiers have a subroutine scope
void SymbolTable::define(string name,string type,SYMBOL_E::fields_e kind){
    this->name.push_back(name);
    this->type.push_back(type);
    this->kind.push_back(kind);
    this->num.push_back(count[kind]++);
}
//Returns the number of variables of the given kind already defined in the current scope
int SymbolTable::VarCount(SYMBOL_E::fields_e kind){
    return count[kind];
}
//Returns the kind of the named identifier in the current scope . If the identifier is unknown in the current scope , returns NONE
SYMBOL_E::fields_e SymbolTable::KindOf(string name){
    // cout<<name<<endl;
    // for(int i=0;i<this->name.size();i++){
    //     cout<<this->name[i]<<" ";
    // }
    // cout<<endl;
    vector<string>::iterator it=find(this->name.begin(),this->name.end(),name);
    if(it==this->name.end()){
        return SYMBOL_E::NONE;
    }else{
        return kind[it-this->name.begin()];
    } 
}
//Returns the type of the named identifier in the current scope
string SymbolTable::TypeOf(string name){
    vector<string>::iterator it=find(this->name.begin(),this->name.end(),name);
    if(it==this->name.end()){
        string t;
        return t;
    }else{
        return type[it-this->name.begin()];
    }
}
//Returns the index assigned to the named identifier
int SymbolTable::IndexOf(string name){
    vector<string>::iterator it=find(this->name.begin(),this->name.end(),name);
    if(it==this->name.end()){
        return -1;
    }else{
        return num[it-this->name.begin()];
    }
}

//creates a new output .vm file and prepares it for writing
VMWriter::VMWriter(string fileName){
    fout.open(fileName+".vm");
}
//Writes a VM push command
void VMWriter::writePush(SYMBOL_E::segment_e segment,int index){
    string mem;
    switch(segment){
        case SYMBOL_E::ARG_S:mem="argument";break;
        case SYMBOL_E::LOCAL:mem="local";break;
        case SYMBOL_E::STATIC_S:mem="static";break;
        case SYMBOL_E::THIS:mem="this";break;
        case SYMBOL_E::THAT:mem="that";break;
        case SYMBOL_E::POINTER:mem="pointer";break;
        case SYMBOL_E::TEMP:mem="temp";break;
        case SYMBOL_E::CONSTANT:mem="constant";break;
    }
    fout<<"push "<<mem<<" "<<index<<endl;
}
//Writes a VM pop command
void VMWriter::writePop(SYMBOL_E::segment_e segment,int index){
    string mem;
    switch(segment){
        case SYMBOL_E::ARG_S:mem="argument";break;
        case SYMBOL_E::LOCAL:mem="local";break;
        case SYMBOL_E::STATIC_S:mem="static";break;
        case SYMBOL_E::THIS:mem="this";break;
        case SYMBOL_E::THAT:mem="that";break;
        case SYMBOL_E::POINTER:mem="pointer";break;
        case SYMBOL_E::TEMP:mem="temp";break;
    }
    fout<<"pop "<<mem<<" "<<index<<endl;
}
//Writes a VM arithmetic-logical command
void VMWriter::writeArithmetic(SYMBOL_E::arith_e command){
    string cmnd;
    switch(command){
        case SYMBOL_E::ADD:cmnd="add";break;
        case SYMBOL_E::SUB:cmnd="sub";break;
        case SYMBOL_E::NEG:cmnd="neg";break;
        case SYMBOL_E::EQ:cmnd="eq";break;
        case SYMBOL_E::GT:cmnd="gt";break;
        case SYMBOL_E::LT:cmnd="lt";break;
        case SYMBOL_E::AND:cmnd="and";break;
        case SYMBOL_E::OR:cmnd="or";break;
        case SYMBOL_E::NOT:cmnd="not";break;
        case SYMBOL_E::MUL:cmnd="";writeCall("Math.multiply",2); break;
        case SYMBOL_E::DIV:cmnd="";writeCall("Math.divide",2); break;
    }
    // fout<<"SS";
    if(cmnd!="") fout<<cmnd<<endl;
}
//writes a VM label command
void VMWriter::writeLabel(string label){
    fout<<"label "<<label<<endl;
}
//writes a VM goto command
void VMWriter::writeGoto(string label){
    fout<<"goto "<<label<<endl;
}
//writes a VM if-goto command
void VMWriter::writeIf(string label){
    fout<<"if-goto "<<label<<endl;
}
//writes a VM call command
void VMWriter::writeCall(string name,int nArgs){
    fout<<"call "<<name<<" "<<nArgs<<endl;
}
//writes a VM function command
void VMWriter::writeFunction(string name,int nLocals){
    fout<<"function "<<name<<" "<<nLocals<<endl;
}
//writes a VM return command
void VMWriter::writeReturn(){
    fout<<"return"<<endl;
}
//closes the output file
void VMWriter::close(){
    fout.close();
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