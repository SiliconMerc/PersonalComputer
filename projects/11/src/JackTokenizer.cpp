#include "include/JackTokenizer.h"
JackTokenizer::JackTokenizer(string file){
    //opens the input .jack file and gets read to tokenize it 
    fin.open(file+".jack");
    fout.open(file+"T.xml");
    current_token="";
    str="";
    count=0;
    fout<<"<tokens>\n";
}
JackTokenizer::~JackTokenizer(){
    fout<<"</tokens>\n";
    fin.close();
    fout.close();
}
string JackTokenizer::trimString(string s){
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
bool JackTokenizer::hasMoreTokens(){
    //are there more tokens in the input?
    return !fin.eof();
}
void JackTokenizer::advance(){
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
tokens_e JackTokenizer::tokenType(){
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
keywords_e JackTokenizer::keyWord(){
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
char JackTokenizer::symbol(){
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
string JackTokenizer::identifier(){
    //Returns the identifier which is the current token . 
    //Should be called only if tokenType is IDENTIFIER
    string token=current_token;
    current_token="";
    fout<<"<identifier>"<<token<<"</identifier>\n";
    return token;
}
int JackTokenizer::intVal(){
    //Returns the integer value of the current token . 
    //Should be called only if tokenType is INT_CONST
    string token=current_token;
    current_token="";
    fout<<"<integerConstant>"<<token<<"</integerConstant>\n";
    return stoi(token);
}
string JackTokenizer::stringVal(){
    //Returns the string value of the current token , without the two enclosing doublr quotes
    //Should be called only if tokenType is STRING_CONST
    string token=current_token;
    current_token="";
    fout<<"<stringConstant>"<<token.substr(1,token.length()-2)<<"</stringConstant>\n";
    return token.substr(1,token.length()-1);
}