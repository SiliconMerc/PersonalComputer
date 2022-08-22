#include<fstream>
#include<iostream>
#include<string>
#include<dirent.h>
#include <regex>
#include<vector>
#include<map>
using namespace std;
#ifndef _TOKEN_E
namespace TOKEN_E{
    enum keywords_e{CLASS,METHOD,FUNCTION,CONSTRUCTOR,INT,BOOLEAN,CHAR,VOID,VAR,STATIC,FIELD,LET,DO,IF,ELSE,WHILE,RETURN,TRUE,FALSE,NULL_PTR,THIS};
    enum tokens_e {KEYWORD,SYMBOL,IDENTIFIER,INT_CONST,STRING_CONST,NONE};
}
namespace SYMBOL_E{ 
    enum fields_e{STATIC_F,FIELD,ARG_F,VAR,NONE};
    enum segment_e{ARG_S,LOCAL,STATIC_S,THIS,THAT,POINTER,TEMP};
    enum arith_e{ADD,SUB,NEG,EQ,GT,LT,AND,OR,NOT};
}
#define _TOKEN_E
#endif
class JackTokenizer{
    ifstream fin;
    ofstream fout;
    string current_token;
    int  count;
    string str;
    public:
    JackTokenizer(string file);
    ~JackTokenizer();
    string trimString(string s);
    bool hasMoreTokens();
    void advance();
    TOKEN_E::tokens_e tokenType();
    TOKEN_E::keywords_e keyWord();
    char symbol();
    string identifier();
    int intVal();
    string stringVal();
};