#include<fstream>
#include<iostream>
#include<string>
#include<dirent.h>
#include <regex>
#include<vector>
#include<map>
#include<algorithm>
using namespace std;
#ifndef _TOKEN_E
namespace TOKEN_E{
    enum keywords_e{CLASS,METHOD,FUNCTION,CONSTRUCTOR,INT,BOOLEAN,CHAR,VOID,VAR,STATIC,FIELD,LET,DO,IF,ELSE,WHILE,RETURN,TRUE,FALSE,NULL_PTR,THIS};
    enum tokens_e {KEYWORD,SYMBOL,IDENTIFIER,INT_CONST,STRING_CONST,NONE};
}
namespace SYMBOL_E{ 
    enum fields_e{STATIC_F,FIELD,ARG_F,VAR,NONE};
    enum segment_e{ARG_S,LOCAL,STATIC_S,THIS,THAT,POINTER,TEMP,CONSTANT};
    enum arith_e{ADD,SUB,NEG,EQ,GT,LT,AND,OR,NOT,MUL,DIV};
}
#define _TOKEN_E
#endif
class SymbolTable{
    vector<string> name;
    vector<string> type;
    vector<SYMBOL_E::fields_e> kind;
    vector<int> num;
    map<SYMBOL_E::fields_e,int> count;
    public:
    //creates a new symbol table
    SymbolTable();
    //defines a new identifier of the given name,type, and kind,and assigns it a running index.
    //STATIC and FIELD indentifiers have a class scope , while ARG and VAR identifiers have a subroutine scope
    void define(string name,string type,SYMBOL_E::fields_e kind);
    //Returns the number of variables of the given kind already defined in the current scope
    int VarCount(SYMBOL_E::fields_e kind);
    //Returns the kind of the named identifier in the current scope . If the identifier is unknown in the current scope , returns NONE
    SYMBOL_E::fields_e KindOf(string name);
    //Returns the type of the named identifier in the current scope
    string TypeOf(string name);
    //Returns the index assigned to the named identifier
    int IndexOf(string name);
};
class VMWriter{
    ofstream fout;
    public:
    //creates a new output .vm file and prepares it for writing
    VMWriter(string fileName);
    //Writes a VM push command
    void writePush(SYMBOL_E::segment_e segment,int index);
    //Writes a VM pop command
    void writePop(SYMBOL_E::segment_e segment,int index);
    //Writes a VM arithmetic-logical command
    void writeArithmetic(SYMBOL_E::arith_e command);
    //writes a VM label command
    void writeLabel(string label);
    //writes a VM goto command
    void writeGoto(string label);
    //writes a VM if-goto command
    void writeIf(string label);
    //writes a VM call command
    void writeCall(string name,int nArgs);
    //writes a VM function command
    void writeFunction(string name,int nLocals);
    //writes a VM return command
    void writeReturn();
    //closes the output file
    void close();
};
class CompilationEngine{
    ifstream fin;
    ofstream fout;
    VMWriter * vmWrite;
    string spaces;
    string token;
    string current_tag;
    string current_val;
    int label_Lx;
    string className;
    map<string,string> expr;
    SymbolTable *class_level,*subroutine_level;
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
    SYMBOL_E::fields_e string2enum_fields(string kind);
    SYMBOL_E::segment_e enum2enum_fields_segments(SYMBOL_E::fields_e kind);
    SYMBOL_E::arith_e str2enum_arith(string op);
    int getVarIndex(string varName);
    SYMBOL_E::fields_e getVarKind(string varName);
    string getVarType(string varName);
    bool CompileSubroutineCallWrite(string var1,string var2);
    public:
    CompilationEngine();
    CompilationEngine(string file);
    ~CompilationEngine();
    bool CompileClass();
    bool CompileClassVarDec();
    bool CompileSubroutineDec();
    int compileParameterList();
    bool compileSubroutineBody(string fName);
    int compileVarDec();
    bool compileStatements();
    bool CompileSubroutineCall();
    bool CompileExpression();
    bool CompileTerm();
    int CompileExpressionList();
    void throw_exception(string msg);
};