/*****************************************************************************
TITLE: asm                                                                                                                               
AUTHOR: AARYA VARAT JOSHI - 1801CS01
Declaration of Authorship
This cpp file, asm.cpp, is part of the assignment of CS321 at the 
department of Computer Science and Engg, IIT Patna . 
*****************************************************************************/
#include <bits/stdc++.h>
using namespace std;
#define F first
#define S second
#define pa pair<int,int>
#define sz(x) x.size()
#define pb push_back

//Storing the data
struct node{
    string label;
    int cnt_prog;
    int ln_num;
    int opc;
};


int program_counter = 0;                // helps in keeping track of the instructions executed
int line_number = 0;                    // keeps track of the line being executed.
int len;                                // temporary variable to store the length of the string being read.
string str, aux;                        // auxiliary string variables
map <string , pa > instruct_table;      // map to store the opcode and operand type for each instruction.
map <int , string> error_table;         // map to store the errors.
map <string, int> label_table;          // map to store labelnames.
vector <string> original_file;          // storing the original file.
vector <string> errors;                 // storing the errors.
vector <node> labelCallValue;           // stores the called label names
vector <node> labelCallOffset;          // stores the called label names
vector <string> lf_col1(1000);          // first column of .lst file.
vector <string> lf_col2(1000);          // second column of .lst file.
int numberType;                         // helper variable to determine the type of the number
int hex_max_value = 16777215;           // the maxnumber used of finding hex of negative numbers
unsigned long long int hex_max_val8bit=4294967295;
int errCheck[100005];
string current="@aarya";
string previous="@aarya";

void init()                             //initializing the instruction table with opcodes
{
    //0 means no value or offset
    //1 means value or label
    //2 means offset
    instruct_table["data"]     = {-1, 1};
    instruct_table["SET"]      = {-2, 1};
    instruct_table["ldc"]      = {0, 1};
    instruct_table["adc"]      = {1, 1};
    instruct_table["ldl"]      = {2, 2};
    instruct_table["stl"]      = {3, 2};
    instruct_table["ldnl"]     = {4, 2};
    instruct_table["stnl"]     = {5, 2};
    instruct_table["add"]      = {6, 0};
    instruct_table["sub"]      = {7, 0};
    instruct_table["shl"]      = {8, 0};
    instruct_table["shr"]      = {9, 0};
    instruct_table["adj"]      = {10, 1};
    instruct_table["a2sp"]     = {11, 0};
    instruct_table["sp2a"]     = {12, 0};
    instruct_table["call"]     = {13, 2};
    instruct_table["return"]   = {14, 0};
    instruct_table["brz"]      = {15, 2};
    instruct_table["brlz"]     = {16, 2};
    instruct_table["br"]       = {17, 2};
    instruct_table["HALT"]     = {18, 0};
    error_table[0] = "incorrect file format";
    error_table[1] = "extra on end of line";
    error_table[2] = "bogus label name";
    error_table[3] = "duplicate label definition";
    error_table[4] = "bogus mnemonic";
    error_table[5] = "improper operand";
    return;
}

string removeUneccesarySpaces(string tmp)   //function to remove unecessary spaces from each line of code
{
    stringstream ss(tmp);                   //string stream to break the strings into tokens
    vector <string> tmp_vec;                // stores the individual strings
    while(ss>>aux)
        tmp_vec.pb(aux);
    string ret = "";
    int l1 = tmp_vec.size();
    for(int i=0;i<l1;i++)
    {
        ret += tmp_vec[i];
        if(i!=l1-1)
            ret += " ";
    }
    return ret;
}

string removeComment(string str)        //function to remove comments from each line
{  
    string ret = str;                   // output string
    for(int i=0;i<len;i++)
    {
        if(str[i]==';')
        {
            ret = str.substr(0, i);
            break;
        }
    }
    return ret;
}



void error(int tag,int lineNumber)      //function to fill the errors in error container
{
    errCheck[lineNumber]=1;
    string tmp_err = error_table[tag];
    tmp_err += " on line number ";
    tmp_err += to_string(lineNumber+1);
    errors.pb(tmp_err);
    return;
}

bool isvalid_labelname(string str)      //function to check if the label name is valid or not
{
    int lnght = str.size();
    for(int i=0;i<lnght;i++)
    {
        if(str[i]=='_')
            continue;
        if(i==0)
        {
            if((str[i]>='A' && str[i]<='Z') || (str[i]>='a' && str[i]<='z'))
                continue;
            return false;
        }
        else
        {
            if((str[i]>='A' && str[i]<='Z') || (str[i]>='a' && str[i]<='z') ||  (str[i]>='0' && str[i]<='9'))
                continue;
            return false;
        }
    }
    return true;
}

string opcode_to_hex_string(unsigned long long int num,int bits)   //function to convert the opcode number to hex
{
    string ret = "";
    for(int i=0;i<bits;i++)
        ret=ret+"0";
    int idx = bits-1;
    while(num!=0)
    {
        int curr = num%16;
        if(curr<=9)
            ret[idx] = (char)(48+curr);
        else
            ret[idx] = (char)(65+curr-10);
        num/=16;
        idx--;
    }
    return ret;
}

bool isDec(string str)                  //function to check if the str is decimal
{
    int lnght = str.size();
    for(int i=0;i<lnght;i++)
    {
        if((str[i]>='0' && str[i]<='9'))
            continue;
        return false;
    }
    return true;
}

bool isOct(string str)                  //function to check if the str is octal
{
    int lnght = str.size();
    for(int i=0;i<lnght;i++)
    {
        if((str[i]>='0' && str[i]<='7'))
            continue;
        return false;
    }
    return true;
}

bool isHex(string str)                  //function to check if the str is hexadecimal
{
    int lnght = str.size();
    if(lnght==0)
        return false;
    for(int i=0;i<lnght;i++)
    {
        if((str[i]>='0' && str[i]<='9') || (str[i]>='A' && str[i]<='F') || (str[i]>='a' && str[i]<='f'))
            continue;
        return false;
    }
    return true;
}

bool isValidValue(string str)           //function to check if str is a valid value or not
{
    int lnght = sz(str);
    int idx = 0;
    if(str[idx]=='-' || str[idx]=='+')
    {
        idx++;
        if(lnght==1)
            return false;
    }
    if(str[idx]=='0')
    {
        idx++;
        numberType=1;
        if(idx==lnght)
        {
            return true;
        }
        if(idx<lnght && str[idx]=='x')
        {
            idx++;
            string str_hex = str.substr(idx, lnght-idx);
            if(isHex(str_hex))
            {
                numberType = 2;
                return true;
            }
            return false;
        }
        else
        {
            string str_oct = str.substr(idx, lnght-idx);
            if(isOct(str_oct))
            {
                numberType = 1;
                return true;
            }
            return false;
        }
    }
    string str_dec = str.substr(idx, lnght-idx);
    if(isDec(str_dec))
    {
        numberType = 0;
        return true;
    }
    return false;
}

bool checkMnemonic(string str)              //function to check if str is valid Mnemonic or not
{
    if(instruct_table.find(str)==instruct_table.end()) 
        return false;
    return true;
}

int DecOctHexToInt(string str,int type)     //function to convert str in Dec or Oct ot Hex to Integer
{
    //For decimal type=10;
    //For octal type=8;
    //For hexadecimal type=16;
    int ret = 0, var1 = 1;
    int lnght = str.size();
    for(int i=lnght-1;i>=0;i--)
    {
        int curr;
        if(str[i]>='0' && str[i]<='9')
            curr = str[i] - '0';
        else if(str[i]>='A' && str[i]<='Z')
            curr = str[i] - 'A' + 10;
        else
            curr = str[i] - 'a' + 10;
        ret += (curr*(var1));
        var1*=type;
    }
    return ret;
}

string value_to_opcode(string str, int type,int par=0)  //function to convert the value str to opcode
{
    int cnt = 0;
    bool ispositive = true;
    int lnght = str.size();
    if(str[0]=='-' or str[0]=='+'){
        cnt++;
        if(str[1]=='0'&&lnght==2)
            return "000000";
        if(str[0]=='-')
            ispositive = false;
    }
    cnt += type;
    string val_str = str.substr(cnt, lnght-cnt);
    unsigned long long int val = 0;
    if(type==0)
        val = DecOctHexToInt(val_str,10);
    else if(type==1)
        val = DecOctHexToInt(val_str,8);
    else if(type==2)
        val = DecOctHexToInt(val_str,16);
    if(par==1)
    {
        if(!ispositive)
            val = hex_max_val8bit - val + 1;
        string ret;
        ret=opcode_to_hex_string(val,8);
        return ret;
    }
    if(!ispositive)
        val = hex_max_value - val + 1;
    string ret;
    ret=opcode_to_hex_string(val,6);
    return ret;
}

void Token_2(vector <string> &str_tok)      //Function to check instructions if number of tokens are 2
{
    string mnemonic = str_tok[0];
    if(checkMnemonic(mnemonic))
    {
        int opcode_mnemonic = instruct_table[mnemonic].F;
        if(opcode_mnemonic==-1)
        {
            string operand = str_tok[1];
            if(isValidValue(operand))
            {
                string set_opc=value_to_opcode(operand,numberType,1);
                lf_col2[line_number]=set_opc;
            }
            else
                error(5,line_number);
            return;
        }
        if(opcode_mnemonic==-2)
        {
            string operand = str_tok[1];
            if(isValidValue(operand))
            {
                string set_opc=value_to_opcode(operand,numberType,1);
                lf_col2[line_number]=set_opc;
                if(current!=previous)
                    label_table[current]=DecOctHexToInt(set_opc,16);
            }
            else
                error(5,line_number);
            return;
        }
        string suff_opcode = opcode_to_hex_string(opcode_mnemonic,2);
        int num_of_operands = instruct_table[mnemonic].S;
        string operand = str_tok[1];

        if(num_of_operands==0)
        {
            error(5,line_number);
            lf_col2[line_number] = "        ";
        }
        else if(num_of_operands==1)
        {
            if(!isValidValue(operand))
                labelCallValue.pb({operand, program_counter, line_number, opcode_mnemonic});
            else
            {
                string pref_opcode = value_to_opcode(operand, numberType);
                lf_col2[line_number] = pref_opcode+suff_opcode;
            }
        }
        else if(num_of_operands==2)
        {
            if(!isValidValue(operand))
                labelCallOffset.pb({operand, program_counter, line_number, opcode_mnemonic});
            else
            {
                string pref_opcode = value_to_opcode(operand, numberType);
                lf_col2[line_number] = pref_opcode+suff_opcode;
            }
        }
    }
    else
    {
        lf_col2[line_number] = "        ";
        error(4,line_number);
    }
    current=previous;
}

void check_instruc(string str)          //function to check and process instructions
{
    vector <string> str_tok;
    int tokens = 0;
    stringstream ss(str);
    while(ss>>aux)
    {
        str_tok.pb(aux);
        tokens++;
    }
    if(tokens>2)
    {
        program_counter++;
        error(1,line_number);
        lf_col2[line_number] = "        ";
        current=previous;
    }
    else if(tokens==2)
    {
        program_counter++;
        Token_2(str_tok);
    }
    else if(tokens==1)
    {
        program_counter++;
        string mnemonic = str_tok[0];
        if(checkMnemonic(mnemonic))
        {
            int opcode_mnemonic = instruct_table[mnemonic].F;
            string suff_opcode = opcode_to_hex_string(opcode_mnemonic,2);
            int num_of_operands = instruct_table[mnemonic].S;
            if(num_of_operands==0)
                lf_col2[line_number] = "000000" + suff_opcode;
            else if(num_of_operands==1)
            {
                lf_col2[line_number] = "        ";
                error(5,line_number);
            }
            else if(num_of_operands==2)
            {
                lf_col2[line_number] = "        ";
                error(5,line_number);
            }
        }
        else
        {
            lf_col2[line_number] = "        ";
            error(4,line_number);
        }
        current=previous;
    }
    else
        lf_col2[line_number] = "        ";
}
void Pass1(char** argv)                 //Pass 1 of the assembler
{
    string tmp=argv[1];
    int lnght=sz(tmp);
    string fname=(tmp).substr(0,lnght-4);
    string inp_file=fname+".asm";
    ifstream input(inp_file);
    current=previous;
    while(getline(input, str))
    {
        original_file.pb(str);                     // storing the original asm code if required for further use.
        len = str.size();
        str = removeComment(str);               // removes comments from each line.
        str = removeUneccesarySpaces(str);      // it removes unnecessary spaces in between.
        len = str.size();
        if(len==0)                              // empty line
        {                             
            lf_col1[line_number] = lf_col2[line_number] = "        ";
            line_number++;
            continue;
        }
        int colon_idx = -1;
        for(int i=0;i<len;i++)                  // checking for the first occurence of colon
        {     
            if(str[i]==':')
            {
                colon_idx = i;
                break;
            }
        }
        if(colon_idx==-1)
        {
            string pc = opcode_to_hex_string(program_counter,8);
            lf_col1[line_number] = pc;
            check_instruc(str);
        }
        else                                    // colon present 
        {                          
            string pc = opcode_to_hex_string(program_counter,8);
            lf_col1[line_number] = pc;
            string label = str.substr(0, colon_idx);

            if(!isvalid_labelname(label))
            {
                error(2,line_number);
            }
            else
            {
                if(label_table.find(label)!=label_table.end())
                    error(3,line_number);
                else
                {
                    current=label;
                    label_table[label] = program_counter;
                }
            }
            string instruc = str.substr(colon_idx+1, len-colon_idx-1);
            check_instruc(instruc);
        }
        line_number++;
    }
}

void writeObjFile(string fname)         //Writing into the object file
{
    string obj_file=fname+".o";
    unsigned int machine_code_int[line_number]; //object file contents
    int idx=0;
    for(int i=0;i<line_number;i++)
    {
        if(lf_col2[i][0]==' ')
            continue;
        machine_code_int[idx++]=DecOctHexToInt(lf_col2[i],16);
    }
    FILE* obj_fptr;
    obj_fptr=fopen(obj_file.c_str(),"wb");
    fwrite(machine_code_int,sizeof(int),idx,obj_fptr);
}

void Pass2(char** argv)         //Pass 2 of the assembler
{
    for(auto it:labelCallOffset)
    {
        if(label_table.find(it.label)==label_table.end())
        {
            error(2,it.ln_num);
            continue;
        }
        int address = label_table[it.label] - it.cnt_prog;
        if(address<0)
            address = hex_max_value + address + 1;
        lf_col2[it.ln_num] = opcode_to_hex_string(address,6) + opcode_to_hex_string(it.opc,2);
    }
    for(auto it:labelCallValue)
    {
        if(label_table.find(it.label)==label_table.end())
        {
            error(2,it.ln_num);
            continue;
        }
        int address = label_table[it.label];
        if(address<0)
            address = hex_max_value + address + 1;
        lf_col2[it.ln_num] = opcode_to_hex_string(address,6) + opcode_to_hex_string(it.opc,2);
    }

    string tmp=argv[1];
    int lnght=sz(tmp);
    string fname=(tmp).substr(0,lnght-4);
    string error_file=fname+".log";
    ofstream outfile;
    outfile.open(error_file);
    bool errFlag=0;
    if(int(sz(errors))==0)
        outfile<<"Compiled without any errors"<<endl;
    else
        cout<<"error messages:\n";
    for(auto it:errors)
    {
        outfile<<"ERROR: "<<it<<endl;
        cout<<"ERROR: "<<it<<endl;
        errFlag=1;
    }
    outfile.close();
    string list_file=fname+".lst";
    outfile.open(list_file);
    for(int i=0;i<line_number;i++)
    {
        if(errCheck[i])
            continue;
        string out=lf_col1[i]+" "+lf_col2[i]+" "+original_file[i];
        outfile<<out<<endl;
    }
    outfile.close();
    if(!errFlag)
        writeObjFile(fname);
}
int main(int argc, char** argv)         //Main function of the program
{ 
    init();                 // filling the map with opcodes for instructions.
    string tmp=argv[1];
    string fext="";
    int idx=0,ln=sz(tmp);
    for(int i=0;i<ln;i++)
    {
        if(tmp[i]=='.')
        {
            idx=i;
            break;
        }
    }
    for(int i=idx+1;i<ln;i++)
        fext=fext+tmp[i];
    if(fext!="asm")
    {
        cout<<"Error -- The given file format is not supported\n";
        return 0;
    }

    Pass1(argv);            // Pass 1 of the assembler
    Pass2(argv);            // Pass 2 of the assembler
    return 0;
}