/*****************************************************************************
TITLE: emu                                                                                                                               
AUTHOR: AARYA VARAT JOSHI - 1801CS01
Declaration of Authorship
This cpp file, emu.cpp, is part of the assignment of CS321 at the 
department of Computer Science and Engg, IIT Patna . 
*****************************************************************************/

#include <bits/stdc++.h>
using namespace std;
#define F first
#define S second
#define pa pair<int,int>
#define sz(x) x.size()
#define pb push_back
#define all(v) v.begin(),v.end() 

vector<pair<pair<string,string>,string> > conatiner_instruc;
long long int powr16[15];
int REG_A = 8483536;
int REG_B = 134518156;
int pc = 0;
int sp = 134518166;
int tr = 0;
uint32_t n, size,instruc_cnt;
uint32_t inp_opc[200005];
map<int,int> memory;  //memory default value is def
bool stop_exec=false;
vector <string> container_trace;
vector <string> container_read;
vector <string> container_write;
vector <string> container_before;
vector <string> container_after;

void initialixe() 	//Function to fill the Instruction table
{
    powr16[0]=1;
    powr16[1]=16;
    for(int i=2;i<15;i++)
        powr16[i]=16*powr16[i-1];
    //Initializing Machine Opcode Table
    conatiner_instruc.pb({{"ldc      ","0      "},"value"});
    conatiner_instruc.pb({{"adc      ","1      "},"value"});
    conatiner_instruc.pb({{"ldl      ","2      "},"value"});
    conatiner_instruc.pb({{"stl      ","3      "},"value"});
    conatiner_instruc.pb({{"ldnl     ","4      "},"value"});
    conatiner_instruc.pb({{"stnl     ","5      "},"value"});
    conatiner_instruc.pb({{"add      ","6      "},""});
    conatiner_instruc.pb({{"sub      ","7      "},""});
    conatiner_instruc.pb({{"shl      ","8      "},""});
    conatiner_instruc.pb({{"shr      ","9      "},""});
    conatiner_instruc.pb({{"adj      ","10     "},"value"});
    conatiner_instruc.pb({{"a2sp     ","11     "},""});
    conatiner_instruc.pb({{"sp2a     ","12     "},""});
    conatiner_instruc.pb({{"call     ","13     "},"offset"});
    conatiner_instruc.pb({{"return   ","14     "},""});
    conatiner_instruc.pb({{"brz      ","15     "},"offset"});
    conatiner_instruc.pb({{"brlz     ","16     "},"offset"});
    conatiner_instruc.pb({{"br       ","17     "},"offset"});
    conatiner_instruc.pb({{"HALT     ","18     "},""});
    conatiner_instruc.pb({{"SET      ","       "},"value"});
}

int getHextoDec(char ch)
{
    int ret;
    if(ch>='0'&&ch<='9')
        ret=ch-'0';
    else
        ret=ch-'A'+10;
    return ret;
}

string opcode_to_hex_string(uint32_t num,int bits)   //function to convert the opcode number to hex
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

int hexToIntInst(string str) 	//Function to convert hex instruction to int
{
    int ret=0;
    reverse(all(str));
    int lngth=sz(str);
    for(int i=0;i<lngth;i++)
        ret+=getHextoDec(str[i])*powr16[i];
    return ret;
}

int hex_to_int_val(string str) 		//Function to convert hex value to decimal value
{
    int lngth = str.size();
    string binary="";
    int val=0;
    for(int i=0;i<lngth;i++)
    {
        int curr = getHextoDec(str[i]);
        string curr_bin = "0000";
        int var1 = curr;
        int idx = 3;
        while(var1!=0)
        {
            if(var1%2)
                curr_bin[idx] = '1';
            idx--;
            var1/=2;
        }
        binary += curr_bin;
    }
    if(binary[0]=='1')          // here we need to take the complement
    {                 
        int counter=-1;
        reverse(all(binary));
        for(int i=0;i<binary.size();i++){
            if(binary[i]=='1'){
                counter=i;
                break;
            }
        }
        for(int i=counter+1;i<binary.size();i++){
            if(binary[i]=='1')
                 binary[i]='0';
            else
                binary[i]='1';
        }
        for(int i=0;i<binary.size();i++)
            val+= (binary[i]-'0')*pow(2,i);
        return -1*val;
    }
    reverse(all(binary));
    for(int i=0;i<binary.size();i++)
        val+= (binary[i]-'0')*pow(2,i);
    return val;
}
string curr_trace="",num_hex_str;
void Functions(int inst,int val) 		//Function to implement all instructions
{
	if(inst==0)					//ldc function
	{
		curr_trace+="ldc ";
		curr_trace+=num_hex_str;
		REG_B=REG_A;
		REG_A=val;
	}
	else if(inst==1)			//adc function
	{
		curr_trace+="adc ";
		curr_trace+=num_hex_str;
		REG_A=REG_A+val;
	}
	else if(inst==2) 			//ldl function
	{
		curr_trace+="ldl ";
		curr_trace+=num_hex_str;	
		REG_B=REG_A;
		string curr_read = "Reading memory[";
		curr_read += opcode_to_hex_string(sp + val,8);
		curr_read += "]";
		curr_read += " finds ";
		if(memory.find(sp+val)!=memory.end())
			REG_A=memory[sp+val];
		else
			REG_A=0;
		curr_read += opcode_to_hex_string(REG_A,8);
		container_read.push_back(curr_read);
	}	
	else if(inst==3) 			//stl function
	{
		curr_trace+="stl ";
		curr_trace+=num_hex_str;
		string curr_write = "Writing memory[";
		curr_write += opcode_to_hex_string(sp + val,8);
		curr_write += "] was ";
		if(memory.find(sp+val)==memory.end())
    		curr_write += opcode_to_hex_string(0,8);
	    else
	        curr_write += opcode_to_hex_string(memory[sp + val],8);
		memory[sp+val]=REG_A;
		REG_A=REG_B;
		curr_write += " now ";
			curr_write += opcode_to_hex_string(memory[sp + val],8);
		container_write.push_back(curr_write);
	}
	else if(inst==4) 			//ldnl function
	{
		curr_trace+="ldnl ";
		curr_trace+=num_hex_str;
		string curr_read = "Reading memory[";
	    curr_read += opcode_to_hex_string(REG_A + val,8);
	    curr_read += "]";
	    curr_read += " finds ";
		if(memory.find(REG_A+val)!=memory.end())
			REG_A=memory[REG_A+val];
		else
			REG_A=0;
		curr_read += opcode_to_hex_string(REG_A,8);
		container_read.push_back(curr_read);
	}
	else if(inst==5)			//stnl function
	{
		curr_trace+="stnl ";
		curr_trace+=num_hex_str;
		string curr_write = "Writing memory[";
	    curr_write += opcode_to_hex_string(REG_A + val,8);
	    curr_write += "] was ";
	    if(memory.find(REG_A+val)==memory.end())
	        curr_write += opcode_to_hex_string(0,8);
	    else
	        curr_write += opcode_to_hex_string(memory[REG_A + val],8);
		memory[REG_A+val]=REG_B;
		curr_write += " now ";
	    curr_write += opcode_to_hex_string(memory[REG_A + val],8);
	    container_write.push_back(curr_write);
	}
	else if(inst==6)			//add function
	{
		curr_trace+="add ";
		REG_A=REG_B+REG_A;
	}
	else if(inst==7)			//sub function
	{
		curr_trace+="sub ";
		REG_A=REG_B-REG_A;
	}
	else if(inst==8)			//shl function
	{
		curr_trace+="shl ";
		REG_A=REG_B<<REG_A;
	}
	else if(inst==9)			//shr function
	{
		curr_trace+="shr ";
		REG_A=REG_B>>REG_A;
	}
	else if(inst==10) 			//adj function
	{
		curr_trace+="adj ";
		curr_trace+=num_hex_str;
		sp=sp+val;
	}
	else if(inst==11)			//a2sp function
	{
		curr_trace+="a2sp ";
		sp=REG_A;
		REG_A=REG_B;
	}
	else if(inst==12)			//sp2a function
	{
		curr_trace+="sp2a ";
		REG_B=REG_A;
		REG_A=sp;
	}
	else if(inst==13)			//call function
	{
		curr_trace+="call ";
		curr_trace+=num_hex_str;
		REG_B=REG_A;
		REG_A=pc;
		pc=pc+val;
	}
	else if(inst==14)			//return function
	{
		curr_trace+="return ";
		pc=REG_A;
		REG_A=REG_B;
	}
	else if(inst==15)			//brz function
	{
		curr_trace+="brz ";
		curr_trace+=num_hex_str;
		if(REG_A==0)
			pc=pc+val;
	}
	else if(inst==16)			//brlz function
	{
		curr_trace+="brlz ";
		curr_trace+=num_hex_str;
		if(REG_A<0)
			pc=pc+val;
	}
	else if(inst==17)			//br function
	{
		curr_trace+="br ";
		curr_trace+=num_hex_str;
		pc=pc+val;
	}
	else if(inst==18)			//HALT function
	{
		curr_trace+="HALT ";
     	stop_exec=true;
	}
	else						//SET function
	{
		curr_trace += "SET ";
		curr_trace+=num_hex_str;
	}	
}

void ShowInstructions()
{
	cout<<"usage: emu [options] file.o\n";
    cout<<"-trace  show instruction trace\n";
    cout<<"-read   show memory reads\n";
    cout<<"-write  show memory writes\n";
    cout<<"-before show memory dump before execution\n";
    cout<<"-after  show memory dump after execution\n";
    cout<<"-wipe   wipe written flags before execution\n";
    cout<<"-isa    display ISA\n";
}

void Exec_Before() 		//Function to fill before command
{
	string curr_before = "";
    for(int i=0;i<tr;i++)
    {
        if(i%4==0)
        {
            if(curr_before!="")
            {
                container_before.push_back(curr_before);
                curr_before = "";
            }
            curr_before += opcode_to_hex_string(i,8);
            curr_before += " ";
            curr_before += opcode_to_hex_string(memory[i],8);
            curr_before += " ";
        }
        else
        {
            curr_before += opcode_to_hex_string(memory[i],8);
            curr_before += " ";
        }
    }
    if(curr_before!="")
        container_before.push_back(curr_before);
}

void Exec_Trace()
{
	while(!stop_exec)
    {
    	pc++;
    	instruc_cnt++;
    	n = inp_opc[pc-1];
    	string mach = opcode_to_hex_string(n,8);
        string operand = mach.substr(0, 6);
        string instruc = mach.substr(6, 2);
        int num = hex_to_int_val(operand);
        num_hex_str = opcode_to_hex_string(num,8);

        curr_trace = "";
        curr_trace+="PC="; curr_trace += opcode_to_hex_string(pc-1,8); curr_trace += ", ";
        curr_trace+="SP="; curr_trace += opcode_to_hex_string(sp,8); curr_trace += ", ";
        curr_trace+="A="; curr_trace += opcode_to_hex_string(REG_A,8); curr_trace += ", ";
        curr_trace+="B="; curr_trace += opcode_to_hex_string(REG_B,8); curr_trace += "";
        int inst=hexToIntInst(instruc);
        Functions(inst,num);
        curr_trace += "\n";
        container_trace.push_back(curr_trace);
    }
    
}

void Exec_After() 		//Function to fill after command
{
	string curr_after = "";
    for(int i=0;i<tr;i++){
        if(i%4==0){
            if(curr_after!=""){
                container_after.push_back(curr_after);
                curr_after = "";
            }
            curr_after += opcode_to_hex_string(i,8);
            curr_after += " ";
            curr_after += opcode_to_hex_string(memory[i],8);
            curr_after += " ";
        }
        else{
            curr_after += opcode_to_hex_string(memory[i],8);
            curr_after += " ";
        }
    }
    if(curr_after!="")
        container_after.push_back(curr_after);
}

int main(int argc, char** argv) 
{
	string f_src = argv[1];
    if(f_src=="-trace" || f_src=="-read" || f_src=="-write" || f_src=="-before" || f_src=="-after" ||f_src=="-isa")
        f_src = argv[2];
    else
    {
        ShowInstructions();
        return 0;
    }

    initialixe();
    FILE *fp;
    fp = fopen(argv[2], "rb");
    while(fread(&n,sizeof(uint32_t), 1,fp) != 0)
    {
        inp_opc[tr++]=n;
        memory[tr-1]=n;
    }

    int lngth = f_src.size();
    string f_src_name = f_src.substr(0, lngth - 2);
    string f_op = f_src_name + ".txt";
    ofstream fop;
    fop.open(f_op);

    Exec_Before();
    Exec_Trace();
    Exec_After();
    string lastline = to_string(instruc_cnt);
    lastline += " instructions executed";
    
    container_trace.push_back(lastline);
    container_read.push_back(lastline);
    container_write.push_back(lastline);
    container_before.push_back(lastline);
    container_after.push_back(lastline);
    string command=argv[1];

    if(command=="-trace")
    {
    	for(auto it:container_trace)
    	{
    		cout<<it;
    		fop<<it;
    	}
    }
    else if(command=="-read")
    {
        for(auto it:container_read)
    	{
    		fop<<it<<"\n";
    		cout<<it<<endl;
    	}
    }
    else if(command=="-write")
    {
        for(auto it:container_write)
    	{
    		fop<<it<<"\n";
    		cout<<it<<endl;
    	}
    }
    else if(command=="-before")
    {
    	fop<<"Memory Dump before execution\n\n";
    	cout<<"Memory Dump before execution\n\n";
        for(auto it:container_before)
    	{
    		fop<<it<<"\n";
    		cout<<it<<endl;
    	}
    }
    else if(command=="-after")
    {
    	fop<<"Memory Dump after execution\n\n";
    	cout<<"Memory Dump after execution\n\n";
        for(auto it:container_after)
    	{
    		fop<<it<<"\n";
    		cout<<it<<endl;
    	}
    }
    else if(command=="-isa")
    {
    	cout<<"Opcode Mnemonic Operand\n";
    	fop<<"Opcode Mnemonic Operand\n";
    	for(auto it:conatiner_instruc)
    	{
    		cout<<it.F.S<<it.F.F<<it.S<<endl;
    		fop<<it.F.S<<it.F.F<<it.S<<endl;
    	}
    	cout<<endl<<lastline<<endl;
    	fop<<endl<<lastline;
    }
    fop.close();
}