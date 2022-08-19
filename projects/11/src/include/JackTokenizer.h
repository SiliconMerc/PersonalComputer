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
    tokens_e tokenType();
    keywords_e keyWord();
    char symbol();
    string identifier();
    int intVal();
    string stringVal();
};