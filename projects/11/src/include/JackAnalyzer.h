#include<fstream>
#include<iostream>
#include<string>
#include<dirent.h>
#include <regex>
#include<vector>
#include<map>
using namespace std;
#ifndef _TOKEN_E
enum keywords_e{CLASS,METHOD,FUNCTION,CONSTRUCTOR,INT,BOOLEAN,CHAR,VOID,VAR,STATIC,FIELD,LET,DO,IF,ELSE,WHILE,RETURN,TRUE,FALSE,NULL_PTR,THIS};
enum tokens_e {KEYWORD,SYMBOL,IDENTIFIER,INT_CONST,STRING_CONST,NONE};
#define _TOKEN_E
#endif
class CompilationEngine{
    ifstream fin;
    ofstream fout;
    string spaces;
    string token;
    string current_tag;
    string current_val;
    map<string,string> expr;
    //type: 'int'|'char'|'boolean'|className //no markup
    bool is_type();
    //className: identifier //no markup
    bool is_className();
    //subroutineName: identifier //no markup
    bool is_subroutineName();
    //varName: identifier //no markup
    bool is_varName();
    //statement //no markup
    //subroutineCall: subroutineName '(' expressionList ')' | (className |varName)'.'subroutineName'('expressionList')' //no markup
    
    //op: '+'|'-'|'*'|'/'|'&'|'|'|'<'|'>'|'=' //symbol
    bool is_op();
    //unaryOp: '-'|'~' //symbol
    bool is_unaryOp();
    //keywordConstant: 'true'|'false'|'null'|'this' //keyword
    bool is_keywordConstant();
    void getToken();
    void printToken();
    void printToken(string tok);
    bool match_regex(string val,string reg,bool flag);
    public:
    CompilationEngine(string file);
    ~CompilationEngine();
    bool CompileClass();
    bool CompileClassVarDec();
    bool CompileSubroutineDec();
    bool compileParameterList();
    bool compileSubroutineBody();
    bool compileVarDec();
    bool compileStatements();
    bool CompileSubroutineCall();
    bool CompileExpression();
    bool CompileTerm();
    bool CompileExpressionList();
};