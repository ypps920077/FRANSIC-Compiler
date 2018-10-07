//��T�T��_10427101_���Y�� 
//********************************************************************************************/
//FRANCIS COMPILER
//********************************************************************************************/
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdio>
#include <cstring> 
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>

#define TableSize 	100  	//hash table�j�p 

using namespace std;

typedef struct tableUnit	//token
{
	int num;		//token�s�� 
	string cont;	//token���e 
} tUnit;

typedef struct idTable		//identifier table�����e 
{
	int num;			//�s�� 
	string id;			//identifier name
	int sub;			//subroutine
	string subP;		//subroutine name�O�b���ŧi�� 
}idT;

typedef struct tokenUnit{	//�Ω�table 0 
	int table;
	int entry;
	string cont;	
}tU;

typedef struct statmentUnit
{
	string statement;		//�@���ӱԭz 
	vector<tU> tSet;		//�ԭz�����Ҧ�token 
} sUnit;

typedef struct immUnit		//quadruple table(table 6)�����X 
{
	int line;			//��� 
	int oper[2];		//operator
	int oper1[2];		//operand1
	int oper2[2];		//operand2
	int result[2];		//result
	string title;		//�����X���e 
} iU;


bool ReadWriteFile();							//Ū��J�ɨö}�l�g�J��X�� 
void WhichTable(string, vector<tUnit>);			//�ھ�Ū�J��table�ɮש�J�۹�����vector�� 
void InputTable();								//Ū�Jtable�ɮ� 
void CheckTable(string);						//���Rtoken 
void ExtraTable(string);						//��h�Ӭ۳s��token�A��J�Ӧa���ΡB���R 
void GetToken(string);							//��Ū�i�����e�����@�ӭ�token 
bool isDeli(char);								//token�O�_���Ÿ� 
bool isNum(string);								//token�O�_���Ʀr 
int setHashKey(string);							//�p��hash�ȡA�^�Ǧ�insertHashLinearn() 
void insertHashLinear(string, tUnit[], int);	//�u�ʱ����إ������ 
void resetList(tUnit[]);						//��l�ƻݭn�@hash��table 
void resetIDList(idT[]);						//��l�ƻݭn�@hash��table 
string HexToStr(string);

vector<tokenUnit> tSetLab;			//(0)label table
vector<tUnit> tSetDeli;				//(1)delimiter table
vector<tUnit> tSetRW;				//(2)reserved word table
tUnit 	  	  tSetInt[TableSize];	//(3)integer table
tUnit 	  	  tSetRN[TableSize];	//(4)real number table
idT         tSetId[TableSize];		//(5)identifier table
vector<iU> immSet;					//(6)quadruple table 		//�O�������X  
int		  	  tSetInfor[TableSize];	//(7)information table
int readTables = 0;
string fName = "\0";
ofstream fileOut;
string currentProgram = "\0";

sUnit sData;
vector<sUnit>	sSet;			//�s�@��ԭz�� 
tU tokens;
iU immT;
vector<tableUnit> arrayList;	//�O��array���e 
tableUnit arrayU;
vector<string> labelList;		//�ΨӽT�{label���e���L�Q�ŧi ==> ��syntax error
vector<tUnit> errorList;		//�������~���a�� 
tUnit error;

void introMsg(void)			// initiate an opening message
{   
	cout << "**************************************************" << endl;
    cout << " FRANCIS COMPILER                                *" << endl;
    cout << " Step 1: Read and show tables                    *" << endl;
    cout << " Step 2: FRANCIS COMPILER                        *" << endl;
    cout << "**************************************************" << endl;
    cout << "##################################################" << endl;
    cout << " Step 1: Read and show tables  "                    << endl;
    cout << "##################################################" << endl;
} //introMsg()
  
void initeData(){		//��l�Ƥ@�ӱԭz���e 
	sData.statement = "\0";
	sData.tSet.clear();
}

bool ReadWriteFile()		//Ū��J�ɨö}�l�g�J��X�� 
{	
	string temp = "\0";
	sUnit tempEX;
	vector<sUnit> setLTORG;
	while(fName != "0"){
		cout << "Input file name...[0]Quit: " << endl;
		cin >> fName;
		string infName = fName + ".txt";
		ifstream inFile;
		inFile.open(infName.c_str());		//Ū�ѤJ�� 
		if (!inFile.is_open())
			cout << endl << "### " << infName << " does not exist! ###" << endl;
  	    else{
  	    	string outFileName =  fName + "_output.txt";		//�g��X�� 
			fileOut.open(outFileName.c_str(), ios::out | ios::trunc);
			
  			while ( !inFile.eof() ) {    
  				if (getline(inFile, temp) && temp != "\0"){	
					//fileOut << temp <<endl;
					initeData();
					sData.statement = temp;
					GetToken(temp);			//��Ū�i�����e�����@�ӭ�token
					sSet.push_back(sData);		
    			}//if		
  			}//while
			return true; 		
		}//else
	}//while

	return false;
} //ReadWriteFile()  
  
void WhichTable(string num, vector<tUnit> aSet)			//�ھ�Ū�J��table�ɮש�J�۹�����vector�� 
{
   	string tName;
	if(num == "1" && tSetDeli.empty()){			//Instruction Table
		tSetDeli = aSet;
		readTables = readTables + 1;
		tName = "Delimiter Table 1";
	}	
	else if(num == "2" && tSetRW.empty()){			//Pseudo and Extra Table
		tSetRW = aSet;
		readTables = readTables + 1;
		tName = "Reserved Word Table 2";
	}	
		
	cout << tName << endl;
	for(int i = 0; i < aSet.size(); i++){			//�ù���Xtable���e 
		cout << "\t[" << aSet[i].num << "] " << aSet[i].cont << setw(4);
		if(!((i+1)% 5))
			cout << endl;
	}//for		
} //WhichTable()

void InputTable()			//Ū�Jtable�ɮ� 
{
	ifstream inFile;
	string numTable = "-1", fileName = "Table";
	vector<tUnit> tSet;
	tUnit tNode ;
    tNode.num = 0;
	
	while(numTable != "0"){
		cout << endl << "Input the table number: 1 ~ 2...,[0]Quit: ";
		cin >> numTable;
	
		fileName = fileName + numTable + ".table";	
		inFile.open(fileName.c_str());
	
		if(numTable != "0"){
			if (!inFile.is_open())
				cout << endl << "### " << fileName << " does not exist! ###" << endl;
  	    	else{		
				while ( !inFile.eof () ) {   
//					if(numTable == "1" && tNode.num == 8){
//						inFile >> tNode.cont;
//						tNode.cont = "��";
//					}
//					else if(numTable == "1" && tNode.num == 9){
//						inFile >> tNode.cont;
//						tNode.cont = "��";
//					}
//					else if(numTable == "1" && tNode.num == 10){
//						inFile >> tNode.cont;
//						tNode.cont = "��";
//					}
//					else 
  	  				inFile >> tNode.cont;
					tNode.num = tNode.num + 1; 
					tSet.push_back(tNode);
   				}//while
			
				WhichTable(numTable, tSet);		//�ھ�Ū�J��table�ɮש�J�۹�����vector��
				inFile.close();	
			}
		}//if
		else{
			if(readTables != 2)
				cout << endl << "### Must read Table1 ~ Table2 done!! ###\n";
			else
				return;
		}
	
		numTable = "-1", fileName = "Table";
		tNode.num = 0;
		tSet.clear();
	}//while

} //InputTable()

void InsertIdentifier(string token)			//��Identifier Table 
{   
	int pos = setHashKey(token);			//���} 
	int orgPos = 0;
	bool ok = false;
	for(; ! ok; pos++){		// ���}��J�A�Y�J���}���O�Ū��A�h�[�@��U�@�Ӧ�} 
		if(pos >= TableSize)	pos = pos - TableSize;		// �W�Ltable�j�p�A�A�q[0]�}�l 
		
		if(tSetId[pos].num == -999){				//����}���šA��J��� 
			tSetId[pos].num = pos;
			tSetId[pos].id = token;
			ok = true;
			orgPos = pos;
			tokens.table = 5;
			tokens.entry = tSetId[pos].num;
			tokens.cont = tSetId[pos].id;
			sData.tSet.push_back(tokens);
			for(int k = 0; (strcmp(tSetId[pos].id.c_str(), currentProgram.c_str()) != 0) && (k < TableSize); k++){
				if(strcmp(tSetId[k].id.c_str(), currentProgram.c_str()) == 0){
					tSetId[pos].sub = tSetId[k].num;			//�O����token��scope 
					tSetId[pos].subP = currentProgram;
				}			
			}
		}//if_insert new index in hash				    
	}  	
}

void CheckTable(string token)			//���Rtoken 
{
	for(int i = 0; i < tSetRW.size(); i++){				//��token�ݩ�Register Table
		if(stricmp(tSetRW[i].cont.c_str(), token.c_str()) == 0){
			tokens.table = 2;
			tokens.entry = tSetRW[i].num;
			tokens.cont = token;
			sData.tSet.push_back(tokens);			
			return;
		}		
	}
	
	for(int i = 0; i < tSetDeli.size(); i++){				//��token�ݩ�Delimiter Table
		if(strcmp(tSetDeli[i].cont.c_str(), token.c_str()) == 0){
			tokens.table = 1;
			tokens.entry = tSetDeli[i].num;
			tokens.cont = token;
			sData.tSet.push_back(tokens);
			return;
		}		
	}

	for(int i = 0; i < TableSize; i++){	
		if((!sData.tSet.empty()) && (stricmp(sData.tSet[sData.tSet.size()-1].cont.c_str(), "PROGRAM") == 0 || stricmp(sData.tSet[sData.tSet.size()-1].cont.c_str(), "SUBROUTINE") == 0))
			currentProgram = token;					//�ثe�O�B�bmain�٬Osubroutine? 
						
		if((tSetInt[i].num != -999) && (strcmp(tSetInt[i].cont.c_str(), token.c_str()) == 0)){
			tokens.table = 3;						//integer(tabel 3)
			tokens.entry = tSetInt[i].num;
			tokens.cont = token;
			sData.tSet.push_back(tokens);
			return;
		}
		else if((tSetRN[i].num != -999) && (strcmp(tSetRN[i].cont.c_str(), token.c_str()) == 0)){
			tokens.table = 4;					//real number(table 4)
			tokens.entry = tSetRN[i].num;
			tokens.cont = token;
			sData.tSet.push_back(tokens);
			return;
		}
		else if((tSetId[i].num != -999) && (strcmp(tSetId[i].id.c_str(), token.c_str()) == 0) && (strcmp(sData.tSet[0].cont.c_str(), "CALL") == 0)){
			tokens.table = 5;			//�]��CALL�� identifier�O�bmain�ŧi���A�ҥH���ήھ�currentProgram��� 
			tokens.entry = tSetId[i].num;
			tokens.cont = token;
			sData.tSet.push_back(tokens);
			return;
		}
		else if((tSetId[i].num != -999) && (strcmp(tSetId[i].id.c_str(), token.c_str()) == 0) && (strcmp(tSetId[i].subP.c_str(), currentProgram.c_str()) == 0)){
			tokens.table = 5;					//identifier (table 5)
			tokens.entry = tSetId[i].num;
			tokens.cont = token;
			sData.tSet.push_back(tokens);
			return;
		}
	}//for	
	ExtraTable(token);				//��h�Ӭ۳s��token�A��J�Ӧa���ΡB���R�A�٦�String Table������ 
} //CheckTable()

void ExtraTable(string token)				//��h�Ӭ۳s��token�A��J�Ӧa���ΡB���R
{											//�Y�btable���䤣��A�h�n��hash function�s�W��table�� 
	string temp1, temp2;
	bool created = false;		//�ˬdtoken�O�_�H�إߦbtable�� 
	
	if(token != "\0"){
		
		if(isNum(token)){								//��token�ݩ�Integer/real
			insertHashLinear(token, tSetInt, 3);
			CheckTable(token);
		}
		else{							
		//label�����Ÿ��s�� 
			string temp = "\0";
			for(int i = 0; i < token.length(); i++){
				if(! isDeli(token[i])){
					temp = temp + token[i];
				}
				else{ //isDeli(token[i])
					CheckTable(temp);
					temp1 = token[i];
					CheckTable(temp1);
					temp = "\0";
				}
			}//for
			bool isReal = false;
			for(int f = 0; f < temp.length(); f++){
				if(temp[f] == '.')	isReal = true;
			}
				
			if(isReal){			//the token is a real number
				insertHashLinear(token, tSetRN, 4);	
				for(int i = 0; i < TableSize; i++){
					if((tSetRN[i].num != -999) && (strcmp(tSetRN[i].cont.c_str(), token.c_str()) == 0)){
						tokens.table = 4;
						tokens.entry = tSetRN[i].num;
						tokens.cont = token;
						sData.tSet.push_back(tokens);	
					}//if	
				}//for	
			}//if
			else if(temp != "\0"){			//create the idenrifier table
				InsertIdentifier(temp);
			}
		}//else
	} //token != "\0"
	
} //ExtraTable()

void GetToken(string word)				//��Ū�i�����e�����@�ӭ�token 
{
  char * cut;
  cut = strtok (const_cast<char  *>(word.c_str())," \t\n");
  while (cut != NULL)/*&& cut != "\n"*/
  {
    CheckTable(cut);
    cut = strtok (NULL, " \t\n");
  }
	
} //GetToken()

bool isDeli(char test)			//token�O�_���Ÿ� 
{	
	bool found = false;
	string token = "\0";
	token += test;
	for(int i = 0; i < tSetDeli.size(); i++){				//��token�ݩ�Delimiter Table
		if(strcmp(tSetDeli[i].cont.c_str(), token.c_str()) == 0){
			found = true;
			return true;
		}
	}//for
	if(! found)
		return false;

} //isDeli() 

bool isNum(string test)			//token�O�_���Ʀr 
{	
	int i = 0;		
	while((test != " ") && (i < test.length())){
		if(test[i] >= '0' && test[i] <= '9')
			i++;
		else
			return false;	
	}
	
	return true;	
} //isNum()

int setHashKey(string test)         //hash��� =�]�C�Ӧr��������ASCII�s�X�ۥ[�^ ���H 100(hash table size) ���l��                                
{   
  int sum = 0, pos = 0;
  for(int i = 0; i < test.length(); i++)			//�C�Ӧr��������ASCII�s�X�ۥ[   
  		sum = sum + test[i]; 	

  sum = sum % 100;
  pos = sum ;
  return pos;
} //setHashKey()

void insertHashLinear(string test, tUnit list[], int tableType)          // �u�ʱ����إ������                             
{   														// �̾�setHashKey()���Ʃ���������}  					
	int pos = setHashKey(test);			//���} 
	int orgPos = 0;
	bool ok = false;
	for(; ! ok; pos++){		// ���}��J�A�Y�J���}���O�Ū��A�h�[�@��U�@�Ӧ�} 
		if(pos >= TableSize)	pos = pos - TableSize;		// �W�Ltable�j�p�A�A�q[0]�}�l 
		
		if(list[pos].num == -999){				//����}���šA��J��� 
			list[pos].num = pos;
			list[pos].cont = test;
			ok = true;
			orgPos = pos;
		}				    
	}                                                                  
} //insertHashLinear()

void resetList(tUnit list[])     //��l�������                                    
{   
	for(int i = 0; i < TableSize; i++){	
		list[i].cont = "\0";
		list[i].num = -999;
	}                                                                                 
} //resetList()

void resetIDList(idT list[])     //��l�������(identifier table)                                    
{   
	for(int i = 0; i < TableSize; i++){	
		list[i].id = "\0";
		list[i].num = -999;
		list[i].sub = 0;
		list[i].subP = "\0";
//		list[i].type = 0;
	}                                                                                 
} //resetList()
 
int StrToHex(string sNum){			//�r��===>16�i��� 
	int decNum = atoi(sNum.c_str());
	
	std::stringstream ss;
	ss << uppercase << std::hex << decNum;

	int num;
	ss >> num;
	
	return num;	
}

string HexToStr(int iNum, int nibble){			//16�i���===>�r�� 
	
	std::stringstream ss;
	
	if(iNum < 100 && nibble == 2)
		ss << setfill('0') << setw(3) << uppercase << std::hex << iNum;
	else if(nibble == 2)
		ss << setfill('0') << setw(2) << uppercase << std::hex << iNum;
	else if(nibble == 1)
		ss << uppercase << std::hex << iNum;
	else if(nibble == 3)
		ss << setfill('0') << setw(2) << uppercase << std::hex << iNum;
	
	return ss.str();
}

int StrToInt(string sNum){			//�r��===>���(10�i��) 
	int decNum = atoi(sNum.c_str());
	
	std::stringstream ss;
	ss << std::dec << decNum;

	int num;
	ss >> num;
	
	return num;		
}

string IntToStr(int iNum){			//���(10�i��)===>�r�� 
	
	std::stringstream ss;
	ss << std::dec << iNum;
	
	return ss.str();
}

string HexCharToBinStr(char cNum){			//16�i��r��===>2�i��r�� 
	//string sNum = HexToStr(iNum);
	string opcode = "\0";
	if(cNum == '0')
		opcode = "0000"; 
	else if(cNum == '1')
		opcode = "0001";
	else if(cNum == '2')
		opcode = "0010";
	else if(cNum == '3')
		opcode = "0011";
	else if(cNum == '4')
		opcode = "0100";
	else if(cNum == '5')
		opcode = "0101";
	else if(cNum == '6')
		opcode = "0110";
	else if(cNum == '7')
		opcode = "0111";
	else if(cNum == '8')
		opcode = "1000";
	else if(cNum == '9')
		opcode = "1001";
	else if(cNum == 'A' || cNum == 'a')
		opcode = "1010";
	else if(cNum == 'B' || cNum == 'b')
		opcode = "1011";
	else if(cNum == 'C' || cNum == 'c')
		opcode = "1100";
	else if(cNum == 'D' || cNum == 'd')
		opcode = "1101";
	else if(cNum == 'E' || cNum == 'e')
		opcode = "1110";
	else if(cNum == 'F' || cNum == 'f')
		opcode = "1111";
		
	return opcode;
		
}

int BinStrToHex(string sNum){			//2�i��r��===>16�i��� 
	int iNum = 0x0;
	if(stricmp(sNum.c_str(), "0000") == 0)
		iNum = 0x0; 
	else if(stricmp(sNum.c_str(), "0001") == 0)
		iNum = 0x1;
	else if(stricmp(sNum.c_str(), "0010") == 0)
		iNum = 0x2;
	else if(stricmp(sNum.c_str(), "0011") == 0)
		iNum = 0x3;
	else if(stricmp(sNum.c_str(), "0100") == 0)
		iNum = 0x4;
	else if(stricmp(sNum.c_str(), "0101") == 0)
		iNum = 0x5;
	else if(stricmp(sNum.c_str(), "0110") == 0)
		iNum = 0x6;
	else if(stricmp(sNum.c_str(), "0111") == 0)
		iNum = 0x7;
	else if(stricmp(sNum.c_str(), "1000") == 0)
		iNum = 0x8;
	else if(stricmp(sNum.c_str(), "1001") == 0)
		iNum = 0x9;
	else if(stricmp(sNum.c_str(), "1010") == 0)
		iNum = 0xA;
	else if(stricmp(sNum.c_str(), "1011") == 0)
		iNum = 0xB;
	else if(stricmp(sNum.c_str(), "1100") == 0)
		iNum = 0xC;
	else if(stricmp(sNum.c_str(), "1101") == 0)
		iNum = 0xD;
	else if(stricmp(sNum.c_str(), "1110") == 0)
		iNum = 0xE;
	else if(stricmp(sNum.c_str(), "1111") == 0)
		iNum = 0xF;
		
	return iNum;
		
}

void resetListInfor(int list[])     //��l������� (information table)                                   
{   
	for(int i = 0; i < TableSize; i++){	
		list[i] = -999;
	}                                                                                 
} //resetList()

int insertHashLinearInfor(int test)          // �u�ʱ����إ������(information table)                              
{   														// �̾�setHashKey()���Ʃ���������}  					
//	int pos = setHashKey(test);			//���} 	//�̧ǩ�Y�i 
	int pos = 0;
	int orgPos = 0;
	bool ok = false;
	for(; ! ok; pos++){		// ���}��J�A�Y�J���}���O�Ū��A�h�[�@��U�@�Ӧ�} 
		if(pos >= TableSize)	pos = pos - TableSize;		// �W�Ltable�j�p�A�A�q[0]�}�l 
		
		if(tSetInfor[pos] == -999){				//����}���šA��J��� 
			tSetInfor[pos] = test;
			ok = true;
			orgPos = pos;
		}				    
	}    
	return orgPos+1;                                                              
} //insertHashLinear()

int NumForDeli(string deli){				//�ھڲŸ����u�����Ǩӵ��w�Ʀr�A��K��j�p 
	if(strcmp(deli.c_str(), "^") == 0)
		return 5;
	else if((strcmp(deli.c_str(), "*") == 0) || strcmp(deli.c_str(), "/") == 0)
		return 4;
	else if((strcmp(deli.c_str(), "+") == 0) || strcmp(deli.c_str(), "-") == 0)
		return 3;
	else if(strcmp(deli.c_str(), "(") == 0 || strcmp(deli.c_str(), ")") == 0)
		return 2;
	else if(strcmp(deli.c_str(), "=") == 0)
		return 1;
	else	return 0;
}

bool CheckDeli(string test, string inStack){	//�B�zassigment�A��Ÿ��j�p�Atrue:�n��Jstack 
	if(strcmp(test.c_str(), "(") == 0)
		return true;
	else if((strcmp(test.c_str(), ")") == 0) || (NumForDeli(test) < NumForDeli(inStack))) 	
		return false;
	else if(NumForDeli(test) >= NumForDeli(inStack))
		return true;		
}

void InsertTable0(){		//�B�zassigment�A�Ȧs����table (table 0) 
	tU unit;
	unit.table = 0;
	unit.entry = tSetLab.size() + 1;
	unit.cont = "T" + IntToStr(unit.entry);
	tSetLab.push_back(unit);	
}

tU ArrayToken(string token){		//�B�zarray�p��A�N�s�W�B�⤸����lexical analysis 
	tU temp;
	for(int i = 0; i < tSetDeli.size(); i++){				//��token�ݩ�Delimiter Table
		if(strcmp(tSetDeli[i].cont.c_str(), token.c_str()) == 0){
			temp.table = 1;
			temp.entry = tSetDeli[i].num;
			temp.cont = token;
			return temp;
		}		
	}

	for(int i = 0; i < TableSize; i++){						//��token�ݩ�Dinteger Table
		if((tSetInt[i].num != -999) && (strcmp(tSetInt[i].cont.c_str(), token.c_str()) == 0)){
			temp.table = 3;
			temp.entry = tSetInt[i].num;
			temp.cont = token;
			return temp;
		}
	}//for
	
	temp.table = 3;							//��integer��hash 
	int pos = setHashKey(token);			//���} 
	int orgPos = 0;
	bool ok = false;
	for(; ! ok; pos++){		// ���}��J�A�Y�J���}���O�Ū��A�h�[�@��U�@�Ӧ�} 
		if(pos >= TableSize)	pos = pos - TableSize;		// �W�Ltable�j�p�A�A�q[0]�}�l 
		
		if(tSetInt[pos].num == -999){				//����}���šA��J��� 
			tSetInt[pos].num = pos;
			temp.entry = pos;
			tSetInt[pos].cont = token;
			temp.cont = token;
			ok = true;
			orgPos = pos;
		}				    
	}
	return temp;  	
}

void ForArray(vector<string> &operand, vector<string> &sub, vector<int> &opLex, vector<int> &subLex, int &num){
	vector<string> temp, newSub;			//temp is for T1�BT2.... 	//�B�zarray��statement�������X(stack pop) 
	vector<int> tempLex, newSubLex;			
	for(int q = sub.size()-1; q >= 0 ; q--){	//��쥻��stack���e�˹L�ө�J�s��stack 
		newSub.push_back(sub[q]);
	}
	for(int q = subLex.size()-1; q >= 0; q--){
		newSubLex.push_back(subLex[q]);		//��entry�Atable 
	}

	while(!newSub.empty()){						//ex: B(I,J) = (J-I)*M+I
		if(temp.empty()){						//�٨S���ȦsT(T1)�AEX: (J-I)
			immT.oper1[1] = newSubLex.back();
			newSubLex.pop_back();
			immT.oper1[0] = newSubLex.back();
			newSubLex.pop_back();
			immT.oper[1] = newSubLex.back();
			newSubLex.pop_back();
			immT.oper[0] = newSubLex.back();
			newSubLex.pop_back();
			immT.oper2[1] = newSubLex.back();
			newSubLex.pop_back();
			immT.oper2[0] = newSubLex.back();
			newSubLex.pop_back();
			immT.title = " = " + newSub.back();
			newSub.pop_back();
			immT.title = immT.title + newSub.back();
			newSub.pop_back();
			immT.title = immT.title + newSub.back();
			newSub.pop_back();
		}//if
		else if(newSub.size() == 2){		//ex: +I���� 
			immT.oper2[1] = tempLex.back();
			tempLex.pop_back();
			immT.oper2[0] = tempLex.back();
			tempLex.pop_back();
			immT.oper[1] = newSubLex.back();
			newSubLex.pop_back();
			immT.oper[0] = newSubLex.back();
			newSubLex.pop_back();
			immT.oper1[1] = newSubLex.back();
			newSubLex.pop_back();
			immT.oper1[0] = newSubLex.back();
			newSubLex.pop_back();
			immT.title = newSub.back() + temp.back();
			newSub.pop_back();
			immT.title = " = " + newSub.back() + immT.title;
			newSub.pop_back();
			temp.pop_back();
		}
		else{
			immT.oper1[1] = tempLex.back();
			tempLex.pop_back();
			immT.oper1[0] = tempLex.back();
			tempLex.pop_back();
			immT.oper[1] = newSubLex.back();
			newSubLex.pop_back();
			immT.oper[0] = newSubLex.back();
			newSubLex.pop_back();
			immT.oper2[1] = newSubLex.back();
			newSubLex.pop_back();
			immT.oper2[0] = newSubLex.back();
			newSubLex.pop_back();
			immT.title = " = " + temp.back() + newSub.back();
			newSub.pop_back();
			immT.title = immT.title + newSub.back();
			newSub.pop_back();
			temp.pop_back();
		}//else

		InsertTable0();
		temp.push_back(tSetLab.back().cont);
		tempLex.push_back(tSetLab.back().table);
		tempLex.push_back(tSetLab.back().entry);
							
		immT.result[0] = tSetLab.back().table;
		immT.result[1] = tSetLab.back().entry;
		immT.title = temp.back() + immT.title;
		num = num + 1;
		immT.line = num;
		immSet.push_back(immT);			
	}//while
	operand.push_back(temp.back());				//����B(T4)��T4 
	opLex.push_back(tSetLab.back().table);
	opLex.push_back(tSetLab.back().entry);
}

bool CheckLabel(string label){			//�ˬd�ϥ��ܼƫe�O�_�w�Q�ŧi 
	for(int i = 0; i < labelList.size(); i++){
		if(strcmp(labelList[i].c_str(), label.c_str()) == 0)
			return true;
	}
	
	return false;
}

int IFstatement(int place, int start, int end, int &num, vector<int> &forGTO){	//�B�zIF��statement�A��k�Passigment 
	int returnStart = num + 1;													//�^��IF��statement���}�Y��m 
		if(strcmp(sSet[place].tSet[start].cont.c_str(), "GTO") == 0){
			immT.oper[0] = sSet[place].tSet[start].table;
			immT.oper[1] = sSet[place].tSet[start].entry;
			immT.oper1[0] = -99;
			immT.oper2[0] = -99;
			immT.title = "\0";
			for(int k = start; k < end; k++)
				immT.title = immT.title + " " + sSet[place].tSet[k].cont;
			num = num + 1;
			immT.line = num;
			immSet.push_back(immT);
			if(start != (end-1)){
				forGTO.push_back(immT.line-1); //vector�q0�}�l
				forGTO.push_back(sSet[place].tSet[start+1].entry);	 
			}
		}// GTO
		else if(sSet[place].tSet[start].table == 5){		//�}�l�B�zassignment 
			if(! CheckLabel(sSet[place].tSet[start].cont)){
				error.num = place;
				error.cont = "  : �ϥΥ��ŧi�L���ܼ�: " +  sSet[place].tSet[start].cont;
				errorList.push_back(error);
				return -9999;
			}
		
			vector<string> operand, operators;
			vector<int> operandLex, operatorsLex;
			bool equRight = false, hasArray = false, putArray = false, multiDim = false;	
			int count = 0;	
			bool isArray = false;	//�o�ӬA���Oarray��	
			for(int w = start; w < end; w++){	//��쩳���X�ӬA��(���Oarray��) 
				if((w > 0) && (sSet[place].tSet[w-1].table == 5) && (stricmp(sSet[place].tSet[w].cont.c_str(), "(") == 0)) isArray = true;
				else if((w > 0) && (sSet[place].tSet[w-1].table == 1) && (stricmp(sSet[place].tSet[w].cont.c_str(), "(") == 0)){
					count = count + 1;
					isArray = false;
				} 
				else if((! isArray) && (w > 0) && (sSet[place].tSet[w+1].table == 1) && (stricmp(sSet[place].tSet[w].cont.c_str(), ")") == 0)) count = count + 1;				
			}
			
			if(count % 2 != 0){		//syntax error
				error.num = place;
				error.cont = "  : �A�������(�ʤ֤@�䪺�A��): ";
				errorList.push_back(error);
				return -9999;
			}
			count = count / 2;
				
			for(int w = start; w <= end; w++){   
				if(stricmp(sSet[place].tSet[w].cont.c_str(), "=") == 0)	equRight = true; 
				
				if((sSet[place].tSet[w].table == 1) || (sSet[place].tSet[w].table == 2)){		//put into operators
					if((w > 0) && (sSet[place].tSet[w-1].table == 5) && (stricmp(sSet[place].tSet[w].cont.c_str(), "(") == 0))	hasArray = true;	//�t���}�C 
						
					if(!equRight && hasArray){		//�J��"(",���٨S��i�h  		//�������䦳�}�C  
						string array = sSet[place].tSet[w-1].cont;
						for(; strcmp(sSet[place].tSet[w].cont.c_str(), ")") != 0; w++){
							array = array + sSet[place].tSet[w].cont;
							if(sSet[place].tSet[w].table == 5){			//�tA(K): A�BK��lex Code 
								operandLex.push_back(sSet[place].tSet[w].table);
								operandLex.push_back(sSet[place].tSet[w].entry);
							}
						} 
						array = array + sSet[place].tSet[w].cont;
						w = w + 1;				//���L")" 
						operand.push_back(array);
						hasArray = false;
						putArray = true;			
					}//�B�z���䪺array 
					else if(equRight && hasArray){		//�����k�䦳�}�C 
						int dim = 0;	//��array����
						vector<string> arrayCont;
						string array1 = sSet[place].tSet[w-1].cont;
						for(; strcmp(sSet[place].tSet[w].cont.c_str(), ")") != 0; w++){
							array1 = array1 + sSet[place].tSet[w].cont;
							if(sSet[place].tSet[w].table == 5){
								dim = dim + 1;
								arrayCont.push_back(sSet[place].tSet[w].cont);
								operandLex.push_back(sSet[place].tSet[w].table);
								operandLex.push_back(sSet[place].tSet[w].entry);
							}	
						} 
						array1 = array1 + sSet[place].tSet[w].cont;
						w = w + 1;				//���L")" 
						string arrayHead = operand.back();		// B	
						operand.push_back(array1);				//B(I,J)
						//����operand stack�u�����array���Y(array�W��) 
						if(dim == 1){
							immT.title = operand.back(); // for dim > 1
							operand.pop_back();		//ex:A(K)
							arrayCont.pop_back();	//ex:K
								
							operand.pop_back();	//pop array head
							InsertTable0();
							operand.push_back(tSetLab.back().cont);
							operandLex.push_back(tSetLab.back().table);
							operandLex.push_back(tSetLab.back().entry);
								
							immT.oper[1] = operatorsLex.back();		//"="
							operatorsLex.pop_back();
							immT.oper[0] = operatorsLex.back();		
							operatorsLex.pop_back();	
								
							immT.result[1] = operandLex.back();		//T
							operandLex.pop_back();
							immT.result[0] = operandLex.back();		
							operandLex.pop_back();
								
							immT.oper2[1] = operandLex.back();		//arrayCont
							operandLex.pop_back();
							immT.oper2[0] = operandLex.back(); 
							operandLex.pop_back();
								
							immT.oper1[1] = operandLex.back();		//array head
							operandLex.pop_back();
							immT.oper1[0] = operandLex.back();	
							operandLex.pop_back();
									
							immT.title = operand.back() + "=" + immT.title;
							num = num + 1;
							immT.line = num;
							immSet.push_back(immT);
								
							//====================================== for J = T1					
							immT.result[1] = operandLex.back();		//J
							operandLex.pop_back();
							immT.result[0] = operandLex.back();		
							operandLex.pop_back();
								
							immT.title = "=" + operand.back();
							operand.pop_back();
							immT.oper2[0] = -99;
							
							operandLex.push_back(tSetLab.back().table); //push T1 lex
							operandLex.push_back(tSetLab.back().entry);
							immT.oper1[1] = operandLex.back();		//T1
							operandLex.pop_back();
							immT.oper1[0] = operandLex.back();	
							operandLex.pop_back();
								
							immT.title =  operand.back() + immT.title;
							operand.pop_back();
							num = num + 1;
							immT.line = num;
							immSet.push_back(immT);
								
							break;
						}
						else{	//dim > 1
							multiDim = true;
							vector<string> subSet;
							vector<int> subLex;
							while(!arrayCont.empty()){
								if(arrayCont.size() == 1){		//�ѳ̫�@�Ӻ��׮� 
									subSet.push_back(arrayCont.back());
									
									subLex.push_back(operandLex.back());
									operandLex.pop_back();
									subLex.push_back(operandLex.back());
									operandLex.pop_back();
										
									arrayCont.pop_back();
								}
								else{		//�B�zarray������statement�B�� 
									for(int a = 1; a < dim; a++){
										subSet.push_back(arrayCont.back());
										subLex.push_back(operandLex.back());
										operandLex.pop_back();
										subLex.push_back(operandLex.back());
										operandLex.pop_back();
										subSet.push_back("-");
										subLex.push_back(ArrayToken("-").entry);
										subLex.push_back(ArrayToken("-").table);
										subSet.push_back("1");					
										subLex.push_back(ArrayToken("1").entry);
										subLex.push_back(ArrayToken("1").table);		
										arrayCont.pop_back();
										for(int currentDim = a; currentDim > 0; currentDim--){
											int used = 0;
											for(int c = 0; c < arrayList.size(); c++){
												if((used < currentDim) && (strcmp(arrayHead.c_str(), arrayList[c].cont.c_str()) == 0)){
													subSet.push_back("*");
													subLex.push_back(ArrayToken("*").entry);
													subLex.push_back(ArrayToken("*").table);
													subSet.push_back(IntToStr(arrayList[c].num));
													subLex.push_back(ArrayToken(IntToStr(arrayList[c].num)).entry);
													subLex.push_back(ArrayToken(IntToStr(arrayList[c].num)).table);
													used = used + 1;
												}
											}
											subSet.push_back("+");
											subLex.push_back(ArrayToken("+").entry);
											subLex.push_back(ArrayToken("+").table);
										}//for currentDim
									}// for dim			
								}//else
							}//while
							operand.pop_back();		//B(I,J)
							ForArray(operand, subSet, operandLex, subLex, num);// T5 = B(T4)�e���q ===> �ҥH�٨S��T5
 
							immT.oper2[1] = operandLex.back();
							operandLex.pop_back();
							immT.oper2[0] = operandLex.back();
							operandLex.pop_back();
							immT.oper1[1] = operandLex.back();
							operandLex.pop_back();
							immT.oper1[0] = operandLex.back();
							operandLex.pop_back();
							immT.oper[1] = operatorsLex.back();
							operatorsLex.pop_back();
							immT.oper[0] = operatorsLex.back();
							operatorsLex.pop_back();
							immT.title = "(" + operand.back() + ")";
							operand.pop_back();
							immT.title = " = " + operand.back() + immT.title;
							operand.pop_back();
								
							InsertTable0();
							operand.push_back(tSetLab.back().cont);
							operandLex.push_back(tSetLab.back().table);
							operandLex.push_back(tSetLab.back().entry);
								
							immT.result[0] = tSetLab.back().table;
							immT.result[1] = tSetLab.back().entry;
							immT.title = operand.back() + immT.title;

							num = num + 1;
							immT.line = num;
							immSet.push_back(immT);

							equRight = false;
							hasArray = false;
						}//else dim > 1
					}// els if �B�z�k�䪺array 
						
					if(strcmp(sSet[place].tSet[w].cont.c_str(), "=") == 0)	equRight = true;
						
					if((strcmp(sSet[place].tSet[w].cont.c_str(), ";") == 0) || (sSet[place].tSet[w].table == 2)){    
						while(!operators.empty()){
							if(multiDim){
								immT.title = operand.back();
								operand.pop_back();
									
								immT.oper[1] = operatorsLex.back();	//entry
								operatorsLex.pop_back();
								immT.oper[0] = operatorsLex.back();	//table
								operatorsLex.pop_back();
								immT.oper2[1] = operandLex.back();	//entry
								operandLex.pop_back();
								immT.oper2[0] = operandLex.back();	//table
								operandLex.pop_back();
								immT.oper1[1] = operandLex.back();
								operandLex.pop_back();
								immT.oper1[0] = operandLex.back();
								operandLex.pop_back();
									
								//operators.push_back("=");	//�w�g���F�A���ΦA�s�W 
								operatorsLex.push_back(ArrayToken("=").table);
								operatorsLex.push_back(ArrayToken("=").entry);
								multiDim = false;
									
								immT.title = operand.back() + operators.back() + immT.title;
								operand.pop_back();
							}
							else if(strcmp(operators.back().c_str(), "=") == 0){	//not an array of multiDim
								immT.oper1[1] = operandLex.back();	//table
								operandLex.pop_back();
								immT.oper1[0] = operandLex.back();	//entry
								operandLex.pop_back();
								immT.title = operand.back();
								operand.pop_back();
								immT.oper[1] = operatorsLex.back();	//table
								operatorsLex.pop_back();
								immT.oper[0] = operatorsLex.back();	//entry
								operatorsLex.pop_back();
								immT.result[1] = operandLex.back();
								operandLex.pop_back();
								immT.result[0] = operandLex.back();
								operandLex.pop_back();
								immT.oper2[0] = -99;
									
								if(putArray){		//�������䦳�}�C 
									immT.oper2[1] = operandLex.back();
									operandLex.pop_back();
									immT.oper2[0] = operandLex.back();
									operandLex.pop_back();
									putArray = false;
								}
									
								immT.title = operand.back() + operators.back() + immT.title;
								operand.pop_back();
							}
							else{
								if(count == 0 && operand.size() != 0 && operators.size() > 2){
									vector<string> tOper1, tOper2;		//�⦡���S���A���B���T�ӹB��l�H�W(K =T1+T2+T3) 
									vector<int> tLex1, tLex2;			//�ѥ��ܥk 
									for(int t = 1; t < operand.size(); t++){
										tOper1.push_back(operand[t]);
									}
									tOper1.push_back(operand.front());
									operand.clear();
									while(!tOper1.empty()){
										operand.push_back(tOper1.back());
										tOper1.pop_back();
									}
										
									for(int t = 2; t < operandLex.size(); t = t + 2){
										tLex1.push_back(operandLex[t+1]);
										tLex1.push_back(operandLex[t]);
									}
									tLex1.push_back(operandLex[1]);
									tLex1.push_back(operandLex.front());
									operandLex.clear();
									while(!tLex1.empty()){
										operandLex.push_back(tLex1.back());
										tLex1.pop_back();
									}
										
									for(int t = 1; t < operators.size(); t++){
										tOper2.push_back(operators[t]);
									}
									tOper2.push_back(operators.front());
									operators.clear();
									while(!tOper2.empty()){
										operators.push_back(tOper2.back());
										tOper2.pop_back();
									}
										
									for(int t = 2; t < operatorsLex.size(); t = t + 2){
										tLex2.push_back(operatorsLex[t+1]);
										tLex2.push_back(operatorsLex[t]);
									}
									tLex2.push_back(operatorsLex[1]);
									tLex2.push_back(operatorsLex.front());
									operatorsLex.clear();
									while(!tLex2.empty()){
										operatorsLex.push_back(tLex2.back());
										tLex2.pop_back();
									}
										
									immT.title = operand.back();
									operand.pop_back();
									immT.title = immT.title + operators.back() + operand.back();
									operand.pop_back();
								}
								else{
									immT.title = operand.back();
									operand.pop_back();
									immT.title = operand.back() + operators.back() + immT.title;
									operand.pop_back();
								}
								immT.oper1[1] = operandLex.back();	//entry
								operandLex.pop_back();
								immT.oper1[0] = operandLex.back();	//table
								operandLex.pop_back();
									
								immT.oper[1] = operatorsLex.back();	//entry
								operatorsLex.pop_back();
								immT.oper[0] = operatorsLex.back();	//table
								operatorsLex.pop_back();
								immT.oper2[1] = operandLex.back();
								operandLex.pop_back();
								immT.oper2[0] = operandLex.back();
								operandLex.pop_back();
							}
								
							if(strcmp(operators.back().c_str(), "=") != 0){		//�٨S�J�쵥���A�����ͼȦsT 
								InsertTable0();
								operand.push_back(tSetLab.back().cont);
								operandLex.push_back(tSetLab.back().table);
								operandLex.push_back(tSetLab.back().entry);
									
								immT.result[0] = tSetLab.back().table;
								immT.result[1] = tSetLab.back().entry;
								immT.title = tSetLab.back().cont + " = " + immT.title;
								num = num + 1;
								immT.line = num;
								immSet.push_back(immT);
							}
							else{
								num = num + 1;
								immT.line = num;
								immSet.push_back(immT);
							}
									
							operators.pop_back() ;
						}	
					}	
					else if((operators.empty()) || (CheckDeli(sSet[place].tSet[w].cont, operators.back()))){
						operators.push_back(sSet[place].tSet[w].cont);
						operatorsLex.push_back(sSet[place].tSet[w].table);
						operatorsLex.push_back(sSet[place].tSet[w].entry);
					}
					else{
						if(strcmp(sSet[place].tSet[w].cont.c_str(), ")") == 0){			//�B�z�A�����B��						
							for(; strcmp(operators.back().c_str(), "(") != 0;){
								immT.oper2[1] = operandLex.back();	//entry
								operandLex.pop_back();
								immT.oper2[0] = operandLex.back();	//table
								operandLex.pop_back();
								immT.title = operand.back();
									
								operand.pop_back();
								immT.oper[1] = operatorsLex.back();	//entry
								operatorsLex.pop_back();
								immT.oper[0] = operatorsLex.back();	//table
								operatorsLex.pop_back();
								immT.oper1[1] = operandLex.back();
								operandLex.pop_back();
								immT.oper1[0] = operandLex.back();
								operandLex.pop_back();
								immT.title = operand.back() + operators.back() + immT.title;

								operand.pop_back();
								if(strcmp(operators.back().c_str(), "(") != 0){
									InsertTable0();
									operand.push_back(tSetLab.back().cont);
									operandLex.push_back(tSetLab.back().table);
									operandLex.push_back(tSetLab.back().entry);
									immT.result[0] = tSetLab.back().table;
									immT.result[1] = tSetLab.back().entry;
									immT.title = tSetLab.back().cont + " = " + immT.title;
									num = num + 1;
									immT.line = num;
									immSet.push_back(immT);
								}
								else{
									num = num + 1;
									immT.line = num;
									immSet.push_back(immT);
								}		
									
								operators.pop_back() ;
							}	
							operators.pop_back();	//pop out the "("
							operatorsLex.pop_back();	//pop out the "("'s entry 
							operatorsLex.pop_back();	//pop out the "("'s table
							count = count - 1;	//�Y�ѨM�A���N��@ 
						}// has "( )"
						else{
							while(!CheckDeli(sSet[place].tSet[w].cont, operators.back())){	//��stack�~���B��l�p��stack���� 
								immT.oper2[1] = operandLex.back();	//entry					//����pop stack���B�� 
								operandLex.pop_back();
								immT.oper2[0] = operandLex.back();	//table
								operandLex.pop_back();
								immT.title = operand.back();
									
								operand.pop_back();
								immT.oper[1] = operatorsLex.back();	//entry
								operatorsLex.pop_back();
								immT.oper[0] = operatorsLex.back();	//table
								operatorsLex.pop_back();
								immT.oper1[1] = operandLex.back();
								operandLex.pop_back();
								immT.oper1[0] = operandLex.back();
								operandLex.pop_back();
								immT.title = operand.back() + operators.back() + immT.title;

								operand.pop_back();
								if(strcmp(operators.back().c_str(), "=") != 0){
									InsertTable0();
									operand.push_back(tSetLab.back().cont);
									operandLex.push_back(tSetLab.back().table);
									operandLex.push_back(tSetLab.back().entry);
									immT.result[0] = tSetLab.back().table;
									immT.result[1] = tSetLab.back().entry;
									immT.title = tSetLab.back().cont + " = " + immT.title;
									num = num + 1;
									immT.line = num;
									immSet.push_back(immT);
								}
								else{
									num = num + 1;
									immT.line = num;
									immSet.push_back(immT);
								}
									
								operators.pop_back() ;
							}
							operators.push_back(sSet[place].tSet[w].cont);
							operatorsLex.push_back(sSet[place].tSet[w].table);
							operatorsLex.push_back(sSet[place].tSet[w].entry);
						} 
					}	
				}// operators
				else{	//put into operand
					operand.push_back(sSet[place].tSet[w].cont);
					operandLex.push_back(sSet[place].tSet[w].table);
					operandLex.push_back(sSet[place].tSet[w].entry);
				}// operand
			}//for Reverse Polish Notation 

		}
	return returnStart;
}

void ForGTO(vector<tU> labelSet, vector<int> list){		//��GTO��forward reference
	while(!list.empty()){
		for(int w = 0; w < labelSet.size(); w++){
			if(labelSet[w].table == list.back()){
				list.pop_back();
				immSet[list.back()].result[0] = 6;
				immSet[list.back()].result[1] = labelSet[w].entry;
				list.pop_back();
			}//if
		}//for
	}//while
}

void SetImmCode(){			//���ͤ����X 
	int num = 0;
	tU aLabel;
	vector<tU> labelSet;
	vector<int> forGTO;
	for(int i = 0; i < sSet.size(); i++){
		if(strcmp(sSet[i].tSet[sSet[i].tSet.size()-1].cont.c_str(), ";") != 0){		//statement�����@�w�O";"
			error.num = i;
			error.cont = "  : �̫�r���D�������O ';'";
			errorList.push_back(error);
		}
		else if((stricmp(sSet[i].tSet[0].cont.c_str(), "VARIABLE") == 0) || (stricmp(sSet[i].tSet[0].cont.c_str(), "DIMENSION") == 0) || (stricmp(sSet[i].tSet[0].cont.c_str(), "LABEL") == 0)){
			int type = 0, dim = 0;
			vector<int> arrSize;
			bool arrBeg = false;
			if(stricmp(sSet[i].tSet[1].cont.c_str(), "ARRAY") == 0)				type = 1;
			else if(stricmp(sSet[i].tSet[1].cont.c_str(), "BOOLEAN") == 0)		type = 2;
			else if(stricmp(sSet[i].tSet[1].cont.c_str(), "CHARACTER") == 0)	type = 3;
			else if(stricmp(sSet[i].tSet[1].cont.c_str(), "INTEGER") == 0)		type = 4;
			else if(stricmp(sSet[i].tSet[1].cont.c_str(), "LABEL") == 0)		type = 5;
			else if(stricmp(sSet[i].tSet[1].cont.c_str(), "REAL") == 0)			type = 6;
			//else if(stricmp(sSet[i].tSet[0].cont.c_str(), "LABEL") != 0)	              //syntax error	
			
			for(int r = 0; r < sSet[i].tSet.size(); r++){
				if(sSet[i].tSet[r].table == 5){
					labelList.push_back(sSet[i].tSet[r].cont);		//�s�J�w�ŧi���ܼ� 
					immT.oper[0] = sSet[i].tSet[r].table;
					immT.oper[1] = sSet[i].tSet[r].entry;
					immT.oper1[0] = -99;
					immT.oper2[0] = -99;
					immT.result[0] = -99;
					immT.title = sSet[i].tSet[r].cont;
					num = num + 1;
					immT.line = num;
					immSet.push_back(immT);
					if(stricmp(sSet[i].tSet[0].cont.c_str(), "LABEL") == 0){	//�Φb�B�zGTO 
						aLabel.cont = sSet[i].tSet[r].cont;
						labelSet.push_back(aLabel);
					}
				}
					// record table7 for DIMENSION 
					if(stricmp(sSet[i].tSet[0].cont.c_str(), "DIMENSION") == 0){	
						if(stricmp(sSet[i].tSet[r].cont.c_str(), "(") == 0){
							arrayU.cont = sSet[i].tSet[r-1].cont;		//�Oarray���W�� 
							arrBeg = true; 
						}		
						
						if(arrBeg && (r > 0) && ((sSet[i].tSet[r-1].table != 1) || (sSet[i].tSet[r+1].table != 1)) && (sSet[i].tSet[r].table == 3)){
							error.num = i;
							error.cont = "  : �ʤֲŸ����j";
							errorList.push_back(error);
						}
						else if(arrBeg && (sSet[i].tSet[r].table == 3)){			//��array��dimension 
							dim = dim + 1;									//�O���C�@dim��size 
							arrSize.push_back(StrToInt(sSet[i].tSet[r].cont));
							if(arrayU.cont == "\0")	arrayU.cont = arrayList.back().cont;
							arrayU.num = arrSize.back();	//�Oarray size 
							arrayList.push_back(arrayU);
							arrayU.cont = "\0";		//reset
							arrayU.num == 0;		//reset
						}
						else if(stricmp(sSet[i].tSet[r].cont.c_str(), ")") == 0){
							insertHashLinearInfor(type);		//���W+1���ˤl
							insertHashLinearInfor(dim);						//�btable7�O�Udim�B�Udim��size 
							for(int w = 0; w < arrSize.size(); w++){
								insertHashLinearInfor(arrSize[w]);	
							}	
							dim = 0;
							arrSize.clear();
						}
					}// DIMENSION
			}//for	
		}// VARIABLE & DIMENSION & LABEL
		else if(stricmp(sSet[i].tSet[0].cont.c_str(), "CALL") == 0){
			int para = 0;			//�ѼƭӼ� 
			bool callBeg = false, isError = false;
			vector<int> paraCont;	//�Ѽƪ����e:table�Bentry 
			immT.title = sSet[i].statement.substr(0, sSet[i].statement.length()-1);
			immT.oper[0] = sSet[i].tSet[0].table;
			immT.oper[1] = sSet[i].tSet[0].entry;
			immT.oper2[0] = -99;
			for(int y = 0; y < sSet.size(); y++){
				if(stricmp(sSet[y].tSet[0].cont.c_str(), "SUBROUTINE") == 0){
					bool subBeg = false;
					for(int r = 0; r < sSet[y].tSet.size(); r++){
						if(stricmp(sSet[y].tSet[r].cont.c_str(), "(") == 0)		subBeg = true;
						else if(stricmp(sSet[y].tSet[r].cont.c_str(), ")") == 0)		subBeg = false;
				
						if((!subBeg) && sSet[y].tSet[r].table == 5){
							labelList.push_back(sSet[y].tSet[r].cont);		//�s�J�w�ŧi���ܼ� 
							break;
						}
					}
				}//SUBROUTINE
			}
			for(int r = 0; r < sSet[i].tSet.size(); r++){
				if((!callBeg) && (! CheckLabel(sSet[i].tSet[r].cont)) && (sSet[i].tSet[r].table == 5)){
						error.num = i;
						error.cont = "  : �ϥΥ��ŧi�L���ܼ�: " +  sSet[i].tSet[r].cont;
						errorList.push_back(error);
						isError = true;
						break;
				}
				else if((!callBeg) && (sSet[i].tSet[r].table == 5)){
					immT.oper1[0] = sSet[i].tSet[r].table;
					immT.oper1[1] = sSet[i].tSet[r].entry;
				}
				
				if(stricmp(sSet[i].tSet[r].cont.c_str(), "(") == 0)		callBeg = true;
				
				if((callBeg) && (r > 0) && ((sSet[i].tSet[r-1].table != 1) || (sSet[i].tSet[r+1].table != 1)) && ((sSet[i].tSet[r].table != 1) && (sSet[i].tSet[r].table != 2))){
					error.num = i;
					error.cont = "  : �ʤֲŸ����j";
					errorList.push_back(error);
					isError = true;
					break;
				}
				if(callBeg && (sSet[i].tSet[r].table != 1) && (sSet[i].tSet[r].table != 2)){	//�����N�J���Ѽ� 
					para = para + 1;
					paraCont.push_back(sSet[i].tSet[r].table);
					paraCont.push_back(sSet[i].tSet[r].entry);
				} 
				else if(stricmp(sSet[i].tSet[r].cont.c_str(), ")") == 0){
					//�btable7�O�UCALL���ѼƭӼơB�U�ѼƤ��e 
					immT.result[0] = 7; 
					immT.result[1] = insertHashLinearInfor(para);	
					for(int w = 0; w < paraCont.size(); w++){
						insertHashLinearInfor(paraCont[w]);	
					}	
					para = 0;
					paraCont.clear();
				}
			}
			
			if(! isError){
				num = num + 1;
				immT.line = num;
				immSet.push_back(immT);
			}
						
		}// CALL
		else if((stricmp(sSet[i].tSet[0].cont.c_str(), "ENP") == 0) || (stricmp(sSet[i].tSet[0].cont.c_str(), "ENS") == 0)){
			immT.oper[0] = sSet[i].tSet[0].table;
			immT.oper[1] = sSet[i].tSet[0].entry;             
			immT.oper1[0] = -99;
			immT.oper2[0] = -99;
			immT.result[0] = -99;
			immT.title = sSet[i].statement.substr(0, sSet[i].statement.length()-1);
			num = num + 1;
			immT.line = num;
			immSet.push_back(immT);	
		}// ENP or ENS
		else if(stricmp(sSet[i].tSet[0].cont.c_str(), "SUBROUTINE") == 0){
			bool subBeg = false;
			for(int r = 0; r < sSet[i].tSet.size(); r++){
				if(stricmp(sSet[i].tSet[r].cont.c_str(), "(") == 0)		subBeg = true;
				else if(stricmp(sSet[i].tSet[r].cont.c_str(), ")") == 0)		subBeg = false;
				
				if((!subBeg) && sSet[i].tSet[r].table == 5){
					labelList.push_back(sSet[i].tSet[r].cont);		//�s�J�w�ŧi���ܼ� 
				}
				
				if((subBeg) && (r > 0) && ((sSet[i].tSet[r-1].table != 1) || (sSet[i].tSet[r+1].table != 1)) && (sSet[i].tSet[r].table == 5)){
					error.num = i;
					error.cont = "  : �ʤֲŸ����j";
					errorList.push_back(error);
					break;
				}
				else if((subBeg) && (sSet[i].tSet[r].table != 1) && (sSet[i].tSet[r].table != 2)){
					immT.oper[0] = sSet[i].tSet[r].table;
					immT.oper[1] = sSet[i].tSet[r].entry;
					immT.oper1[0] = -99;
					immT.oper2[0] = -99;
					immT.result[0] = -99;
					immT.title = sSet[i].tSet[r].cont;
					num = num + 1;
					immT.line = num;
					immSet.push_back(immT);	
				}//subBeg
			}
		}// SUBROUTINE
		else if(stricmp(sSet[i].tSet[0].cont.c_str(), "IF") == 0){			
				int s1 = 0, s2 = 0, e1 = 0, e2 = 0;
				for(int w = 1; w < sSet[i].tSet.size(); w++){ 
					if(strcmp(sSet[i].tSet[w].cont.c_str(), "THEN") == 0){
						s1 = w + 1;			//s1 - 1 �N�OIF~THEN���� 
					}
					else if(strcmp(sSet[i].tSet[w].cont.c_str(), "ELSE") == 0){
						s2 = w + 1;
						e1 = w;
					}
					else if(strcmp(sSet[i].tSet[w].cont.c_str(), ";") == 0){
						e2 = w;
					}
				}//for
				
				bool isError = false;		//���~�ˬd 
				if(((s1-0) <= 2) || ((s2-s1) <= 2) || ((e2-s2) <= 1)){		//����(IF)��S��������Ʊ�(ex: IF THEN ELSE;) 
					error.num = i;
					error.cont = "  : IF��k���~";
					errorList.push_back(error);
					isError = true;
				}
				else if((strcmp(sSet[i].tSet[0].cont.c_str(), "IF") == 0) && (strcmp(sSet[i].tSet[s1-1].cont.c_str(), "THEN") == 0)){
					immT.oper[0] = sSet[i].tSet[2].table;
					immT.oper[1] = sSet[i].tSet[2].entry;
					
					if(! CheckLabel(sSet[i].tSet[1].cont)){
						error.num = i;
						error.cont = "  : �ϥΥ��ŧi�L���ܼ�: " +  sSet[i].tSet[1].cont;
						errorList.push_back(error);
						isError = true;
					}				
					immT.oper1[0] = sSet[i].tSet[1].table;
					immT.oper1[1] = sSet[i].tSet[1].entry;
					
					if(! CheckLabel(sSet[i].tSet[3].cont)){
						error.num = i;
						error.cont = "  : �ϥΥ��ŧi�L���ܼ�: " +  sSet[i].tSet[3].cont;
						errorList.push_back(error);
						isError = true;
					}
					immT.oper2[0] = sSet[i].tSet[3].table;
					immT.oper2[1] = sSet[i].tSet[3].entry;
					InsertTable0();
					immT.result[0] = tSetLab.back().table;
					immT.result[1] = tSetLab.back().entry;
					immT.title = tSetLab.back().cont + " = " + sSet[i].tSet[1].cont + " " + sSet[i].tSet[2].cont + " " + sSet[i].tSet[3].cont;
					num = num + 1;
					immT.line = num;
					immSet.push_back(immT);		
				}
				else{
					error.num = i;
					error.cont = "  : IF��k���~";
					errorList.push_back(error);
					isError = true;
				} 
				
				if(! isError){
					num = num + 1;
					int IFline = num - 1; //�]���bvector�|-1 (�q0�}�l�O)
					immT.line = num;
					immSet.push_back(immT);		//�̭��S���e�A�n��forward reference
				
					int thenS = IFstatement(i, s1, e1, num, forGTO);	//��K��btable 6����m 
					int elseS = IFstatement(i, s2, e2, num, forGTO);
					immSet[IFline].oper[0] = sSet[i].tSet[0].table;
					immSet[IFline].oper[1] = sSet[i].tSet[0].entry; 
					immSet[IFline].oper1[0] = tSetLab.back().table;
					immSet[IFline].oper1[1] = tSetLab.back().entry;
					immSet[IFline].oper2[0] = 6;
					immSet[IFline].oper2[1] = thenS;
					immSet[IFline].result[0] = 6;
					immSet[IFline].result[1] = elseS;
					immSet[IFline].title = "IF " + tSetLab.back().cont + " GO TO " + IntToStr(thenS) + ", ELSE GO TO " + IntToStr(elseS);
				}
				
		}// IF
		else if(stricmp(sSet[i].tSet[0].cont.c_str(), "GTO") == 0){
			immT.oper[0] = sSet[i].tSet[0].table;
			immT.oper[1] = sSet[i].tSet[0].entry;
			immT.oper1[0] = -99;
			immT.oper2[0] = -99;
			immT.title = "\0";
			immT.title = sSet[i].statement.substr(0, sSet[i].statement.length()-1);
			num = num + 1;
			immT.line = num;
			immSet.push_back(immT);
			if(sSet[i].tSet.size() > 2){		//����Ω�forward reference 
				forGTO.push_back(immT.line-1); //vector�q0�}�l 
				forGTO.push_back(sSet[i].tSet[1].entry);
			}
			else{		//ERROR
				error.num = i;
				error.cont = "  : ��k���~�AGTO��S���I�s�ܼ�";
				errorList.push_back(error);
				immSet.pop_back();
			}	
		}// GTO
		else if(sSet[i].tSet[0].table == 5){
			if(! CheckLabel(sSet[i].tSet[0].cont)){
				error.num = i;
				error.cont = "  : �ϥΥ��ŧi�L���ܼ�: " +  sSet[i].tSet[0].cont;
				errorList.push_back(error);
			}			
			else if(strcmp(sSet[i].tSet[1].cont.c_str(), "IF") == 0){
				for(int e = 0; e < labelSet.size(); e++){		//�]�wLABEL�btable 6����m 
					if(strcmp(labelSet[e].cont.c_str(), sSet[i].tSet[0].cont.c_str()) == 0){
						num = num + 1;
						labelSet[e].table = sSet[i].tSet[0].entry;	//�o�̪�table�令��label�쥻entry���N�A�����K�� 
						labelSet[e].entry = num;
					}
				}
				
				int s1 = 2, s2 = 0, e1 = 0, e2 = 0;
				for(int w = 2; w < sSet[i].tSet.size(); w++){ 
					if(strcmp(sSet[i].tSet[w].cont.c_str(), "THEN") == 0){
						s1 = w + 1;			//s1 - 1 �N�OIF~THEN���� 
					}
					else if(strcmp(sSet[i].tSet[w].cont.c_str(), "ELSE") == 0){
						s2 = w + 1;
						e1 = w;
					}
					else if(strcmp(sSet[i].tSet[w].cont.c_str(), ";") == 0){
						e2 = w;
					}
				}//for
				
				bool isError = false;		//���~�ˬd 
				if(((s1-0) <= 2) || ((s2-s1) <= 2) || ((e2-s2) <= 1)){		//����(IF)��S��������Ʊ�(ex: IF THEN ELSE;) 
					error.num = i;
					error.cont = "  : IF��k���~";
					errorList.push_back(error);
					isError = true;
				}
				else if((strcmp(sSet[i].tSet[1].cont.c_str(), "IF") == 0) && (strcmp(sSet[i].tSet[s1-1].cont.c_str(), "THEN") == 0)){
					immT.oper[0] = sSet[i].tSet[3].table;
					immT.oper[1] = sSet[i].tSet[3].entry;
					
					if(! CheckLabel(sSet[i].tSet[2].cont)){
						error.num = i;
						error.cont = "  : �ϥΥ��ŧi�L���ܼ�: " +  sSet[i].tSet[2].cont;
						errorList.push_back(error);
						isError = true;
					}
					immT.oper1[0] = sSet[i].tSet[2].table;
					immT.oper1[1] = sSet[i].tSet[2].entry;
					
					if(! CheckLabel(sSet[i].tSet[4].cont)){
						error.num = i;
						error.cont = "  : �ϥΥ��ŧi�L���ܼ�: " +  sSet[i].tSet[4].cont;
						errorList.push_back(error);
						isError = true;
					}
					immT.oper2[0] = sSet[i].tSet[4].table;
					immT.oper2[1] = sSet[i].tSet[4].entry;
					InsertTable0();
					immT.result[0] = tSetLab.back().table;
					immT.result[1] = tSetLab.back().entry;
					immT.title = tSetLab.back().cont + " = " + sSet[i].tSet[2].cont + " " + sSet[i].tSet[3].cont + " " + sSet[i].tSet[4].cont;
					immT.line = num;
					immSet.push_back(immT);		
				}
				else{
					error.num = i;
					error.cont = "  : IF��k���~";
					errorList.push_back(error);
					isError = true;
				} 
				
				if(! isError){ 		
					num = num + 1;
					int IFline = num - 1; //�]���bvector�|-1 (�q0�}�l�O)
					immT.line = num;
					immSet.push_back(immT);		//�̭��S���e�A�n��forward reference
				
					int thenS = IFstatement(i, s1, e1, num, forGTO);	//��K��btable 6����m 
					int elseS = IFstatement(i, s2, e2, num, forGTO);
					immSet[IFline].oper[0] = sSet[i].tSet[1].table;
					immSet[IFline].oper[1] = sSet[i].tSet[1].entry; 
					immSet[IFline].oper1[0] = tSetLab.back().table;
					immSet[IFline].oper1[1] = tSetLab.back().entry;
					immSet[IFline].oper2[0] = 6;
					immSet[IFline].oper2[1] = thenS;
					immSet[IFline].result[0] = 6;
					immSet[IFline].result[1] = elseS;
					immSet[IFline].title = "IF " + tSetLab.back().cont + " GO TO " + IntToStr(thenS) + ", ELSE GO TO " + IntToStr(elseS);
				}
			}// IF
			else if((stricmp(sSet[i].tSet[1].cont.c_str(), "ENP") == 0) || (stricmp(sSet[i].tSet[1].cont.c_str(), "ENS") == 0)){
				bool isError = true;
				for(int e = 0; e < labelSet.size(); e++){		//�]�wLABEL�btable 6����m 
					if(strcmp(labelSet[e].cont.c_str(), sSet[i].tSet[0].cont.c_str()) == 0){
						num = num + 1;
						labelSet[e].table = sSet[i].tSet[0].entry;
						labelSet[e].entry = num;
						isError = false;
					}
				}
				if(! isError){
					immT.oper[0] = sSet[i].tSet[1].table;
					immT.oper[1] = sSet[i].tSet[1].entry;            //FOR ex: L91 ENP 
					immT.oper1[0] = -99;
					immT.oper2[0] = -99;
					immT.result[0] = -99;
					immT.title = sSet[i].statement.substr(0, sSet[i].statement.length()-1);
					immT.line = num;
					immSet.push_back(immT);	
				}
				else{
					error.num = i;
					error.cont = "  : ��k���~";
					errorList.push_back(error);
				}
						
			}// ENP & ENS
			else{		//�}�l�B�zassignment 
				vector<string> operand, operators;
				vector<int> operandLex, operatorsLex;
				bool equRight = false, hasArray = false, putArray = false, multiDim = false;		//putArray:�b�����k��� , ��array�� 
				int count = 0;	
				bool isArray = false;	//�o�ӬA���Oarray��	
				for(int w = 0; w < sSet[i].tSet.size(); w++){	//��쩳���X�ӬA��(���Oarray��) 
					if((w > 0) && (sSet[i].tSet[w-1].table == 5) && (strcmp(sSet[i].tSet[w].cont.c_str(), "(") == 0)) isArray = true;					
					else if((w > 0) && (sSet[i].tSet[w-1].table == 1) && (strcmp(sSet[i].tSet[w].cont.c_str(), "(") == 0)){
						count = count + 1;
						isArray = false;
					} 
					else if((! isArray) && (w > 0) && (sSet[i].tSet[w+1].table == 1) && (strcmp(sSet[i].tSet[w].cont.c_str(), ")") == 0)) count = count + 1;	
				}
				
				bool isError = false;
				if(count % 2 != 0){		//ERROR
					error.num = i;
					error.cont = "  : �A�������(�ʤ֤@�䪺�A��): ";
					errorList.push_back(error);
					isError = true;
				}	
				count = count / 2;
				
				for(int w = 0; w < sSet[i].tSet.size(); w++){   
					if(isError)	break;
					
					if(stricmp(sSet[i].tSet[w].cont.c_str(), "=") == 0)	equRight = true; 
				
					if(sSet[i].tSet[w].table == 1){		//put into operators
						if((w > 0) && (sSet[i].tSet[w-1].table == 5) && (stricmp(sSet[i].tSet[w].cont.c_str(), "(") == 0))	hasArray = true;
						
						if(!equRight && hasArray){		//�J��"(",���٨S��i�h 
							string array = sSet[i].tSet[w-1].cont;
							for(; strcmp(sSet[i].tSet[w].cont.c_str(), ")") != 0; w++){
								array = array + sSet[i].tSet[w].cont;
								if(sSet[i].tSet[w].table == 5){			//�tA(K): A�BK��lex Code 
									operandLex.push_back(sSet[i].tSet[w].table);
									operandLex.push_back(sSet[i].tSet[w].entry);
								}
							} 
							array = array + sSet[i].tSet[w].cont;
							w = w + 1;				//���L")" 
							operand.push_back(array);
							hasArray = false;
							putArray = true;			
						}//�B�z���䪺array 
						else if(equRight && hasArray){
							int dim = 0;	//��array����
							vector<string> arrayCont;
							string array1 = sSet[i].tSet[w-1].cont;
							for(; strcmp(sSet[i].tSet[w].cont.c_str(), ")") != 0; w++){
								array1 = array1 + sSet[i].tSet[w].cont;
								if(sSet[i].tSet[w].table == 5){
									dim = dim + 1;
									arrayCont.push_back(sSet[i].tSet[w].cont);
									operandLex.push_back(sSet[i].tSet[w].table);
									operandLex.push_back(sSet[i].tSet[w].entry);
								}	
							} 
							array1 = array1 + sSet[i].tSet[w].cont;
							w = w + 1;				//���L")" 
							string arrayHead = operand.back();		// B	
							operand.push_back(array1);				//B(I,J)
							//����operand stack�u�����array���Y(array�W��) 
							if(dim == 1){
								immT.title = operand.back(); // for dim > 1
								operand.pop_back();		//ex:A(K)
								arrayCont.pop_back();	//ex:K
								
								operand.pop_back();	//pop array head
								InsertTable0();
								operand.push_back(tSetLab.back().cont);
								operandLex.push_back(tSetLab.back().table);
								operandLex.push_back(tSetLab.back().entry);
								
								immT.oper[1] = operatorsLex.back();		//"="
								operatorsLex.pop_back();
								immT.oper[0] = operatorsLex.back();		
								operatorsLex.pop_back();	
								
								immT.result[1] = operandLex.back();		//T
								operandLex.pop_back();
								immT.result[0] = operandLex.back();		
								operandLex.pop_back();
								
								immT.oper2[1] = operandLex.back();		//arrayCont
								operandLex.pop_back();
								immT.oper2[0] = operandLex.back(); 
								operandLex.pop_back();
								
								immT.oper1[1] = operandLex.back();		//array head
								operandLex.pop_back();
								immT.oper1[0] = operandLex.back();	
								operandLex.pop_back();
								
								
								immT.title = operand.back() + "=" + immT.title;
								num = num + 1;
								immT.line = num;
								immSet.push_back(immT);
								
								//====================================== for J = T1					
								immT.result[1] = operandLex.back();		//J
								operandLex.pop_back();
								immT.result[0] = operandLex.back();		
								operandLex.pop_back();
								
								immT.title = "=" + operand.back();
								operand.pop_back();
								immT.oper2[0] = -99;
								
								operandLex.push_back(tSetLab.back().table); //push T1 lex
								operandLex.push_back(tSetLab.back().entry);
								immT.oper1[1] = operandLex.back();		//T1
								operandLex.pop_back();
								immT.oper1[0] = operandLex.back();	
								operandLex.pop_back();
								
								
								immT.title =  operand.back() + immT.title;
								operand.pop_back();
								num = num + 1;
								immT.line = num;
								immSet.push_back(immT);
								
								break;
							}
							else{	//dim > 1
								multiDim = true;
								vector<string> subSet;
								vector<int> subLex;
								while(!arrayCont.empty()){
									if(arrayCont.size() == 1){		//�ѳ̫�@�Ӻ��׮� 
										subSet.push_back(arrayCont.back());
										
										subLex.push_back(operandLex.back());
										operandLex.pop_back();
										subLex.push_back(operandLex.back());
										operandLex.pop_back();
										
										arrayCont.pop_back();
									}
									else{			//�B�z�}�C������statement 
										for(int a = 1; a < dim; a++){
											subSet.push_back(arrayCont.back());
											subLex.push_back(operandLex.back());
											operandLex.pop_back();
											subLex.push_back(operandLex.back());
											operandLex.pop_back();
											subSet.push_back("-");
											subLex.push_back(ArrayToken("-").entry);
											subLex.push_back(ArrayToken("-").table);
											subSet.push_back("1");					
											subLex.push_back(ArrayToken("1").entry);
											subLex.push_back(ArrayToken("1").table);		
											arrayCont.pop_back();
											for(int currentDim = a; currentDim > 0; currentDim--){
												int used = 0;
												for(int c = 0; c < arrayList.size(); c++){
													if((used < currentDim) && (strcmp(arrayHead.c_str(), arrayList[c].cont.c_str()) == 0)){
														subSet.push_back("*");
														subLex.push_back(ArrayToken("*").entry);
														subLex.push_back(ArrayToken("*").table);
														subSet.push_back(IntToStr(arrayList[c].num));
														subLex.push_back(ArrayToken(IntToStr(arrayList[c].num)).entry);
														subLex.push_back(ArrayToken(IntToStr(arrayList[c].num)).table);
														used = used + 1;
													}
												}
												subSet.push_back("+");
												subLex.push_back(ArrayToken("+").entry);
												subLex.push_back(ArrayToken("+").table);
											}//for currentDim
										}// for dim			
									}//else
								}//while
								operand.pop_back();		//B(I,J)
								ForArray(operand, subSet, operandLex, subLex, num);// T5 = B(T4)�e���q ===> �ҥH�٨S��T5
 
								immT.oper2[1] = operandLex.back();
								operandLex.pop_back();
								immT.oper2[0] = operandLex.back();
								operandLex.pop_back();
								immT.oper1[1] = operandLex.back();
								operandLex.pop_back();
								immT.oper1[0] = operandLex.back();
								operandLex.pop_back();
								immT.oper[1] = operatorsLex.back();
								operatorsLex.pop_back();
								immT.oper[0] = operatorsLex.back();
								operatorsLex.pop_back();
								immT.title = "(" + operand.back() + ")";
								operand.pop_back();
								immT.title = " = " + operand.back() + immT.title;
								operand.pop_back();
								
								InsertTable0();
								operand.push_back(tSetLab.back().cont);
								operandLex.push_back(tSetLab.back().table);
								operandLex.push_back(tSetLab.back().entry);
								
								immT.result[0] = tSetLab.back().table;
								immT.result[1] = tSetLab.back().entry;
								immT.title = operand.back() + immT.title;

								num = num + 1;
								immT.line = num;
								immSet.push_back(immT);

								equRight = false;
								hasArray = false;
							}//else dim > 1
						}// els if �B�z�k�䪺array 
						
						if(strcmp(sSet[i].tSet[w].cont.c_str(), "=") == 0)	equRight = true;
						
						if(strcmp(sSet[i].tSet[w].cont.c_str(), ";") == 0){
							while(!operators.empty()){
								if(multiDim){
									immT.title = operand.back();
									operand.pop_back();
									
									immT.oper[1] = operatorsLex.back();	//entry
									operatorsLex.pop_back();
									immT.oper[0] = operatorsLex.back();	//table
									operatorsLex.pop_back();
									immT.oper2[1] = operandLex.back();	//entry
									operandLex.pop_back();
									immT.oper2[0] = operandLex.back();	//table
									operandLex.pop_back();
									immT.oper1[1] = operandLex.back();
									operandLex.pop_back();
									immT.oper1[0] = operandLex.back();
									operandLex.pop_back();
									
									//operators.push_back("=");	//�w�g���F�A���ΦA�s�W 
									operatorsLex.push_back(ArrayToken("=").table);
									operatorsLex.push_back(ArrayToken("=").entry);
									multiDim = false;
									
									immT.title = operand.back() + operators.back() + immT.title;
									operand.pop_back();
								}
								else if(strcmp(operators.back().c_str(), "=") == 0){	//not an array of multiDim
									immT.oper1[1] = operandLex.back();	//table
									operandLex.pop_back();
									immT.oper1[0] = operandLex.back();	//entry
									operandLex.pop_back();
									immT.title = operand.back();
									operand.pop_back();
									immT.oper[1] = operatorsLex.back();	//table
									operatorsLex.pop_back();
									immT.oper[0] = operatorsLex.back();	//entry
									operatorsLex.pop_back();
									immT.result[1] = operandLex.back();
									operandLex.pop_back();
									immT.result[0] = operandLex.back();
									operandLex.pop_back();
									immT.oper2[0] = -99;
									
									if(putArray){
										immT.oper2[1] = operandLex.back();
										operandLex.pop_back();
										immT.oper2[0] = operandLex.back();
										operandLex.pop_back();
										putArray = false;
									}
									
									immT.title = operand.back() + operators.back() + immT.title;
									operand.pop_back();
								}
								else{
									if(count == 0 && operand.size() != 0 && operators.size() > 2){
										vector<string> tOper1, tOper2;		//�⦡���S���A���B���T�ӹB��l�H�W(K =T1+T2+T3) 
										vector<int> tLex1, tLex2;			//�ѥ��ܥk 
										for(int t = 1; t < operand.size(); t++){
											tOper1.push_back(operand[t]);
										}
										tOper1.push_back(operand.front());
										operand.clear();
										while(!tOper1.empty()){
											operand.push_back(tOper1.back());
											tOper1.pop_back();
										}
										
										for(int t = 2; t < operandLex.size(); t = t + 2){
											tLex1.push_back(operandLex[t+1]);
											tLex1.push_back(operandLex[t]);
										}
										tLex1.push_back(operandLex[1]);
										tLex1.push_back(operandLex.front());
										operandLex.clear();
										while(!tLex1.empty()){
											operandLex.push_back(tLex1.back());
											tLex1.pop_back();
										}
										
										for(int t = 1; t < operators.size(); t++){
											tOper2.push_back(operators[t]);
										}
										tOper2.push_back(operators.front());
										operators.clear();
										while(!tOper2.empty()){
											operators.push_back(tOper2.back());
											tOper2.pop_back();
										}
										
										for(int t = 2; t < operatorsLex.size(); t = t + 2){
											tLex2.push_back(operatorsLex[t+1]);
											tLex2.push_back(operatorsLex[t]);
										}
										tLex2.push_back(operatorsLex[1]);
										tLex2.push_back(operatorsLex.front());
										operatorsLex.clear();
										while(!tLex2.empty()){
											operatorsLex.push_back(tLex2.back());
											tLex2.pop_back();
										}
										
										immT.title = operand.back();
										operand.pop_back();
										immT.title = immT.title + operators.back() + operand.back();
										operand.pop_back();
									}
									else{
										immT.title = operand.back();
										operand.pop_back();
										immT.title = immT.title + operators.back() + operand.back();
										operand.pop_back();
									}
									immT.oper1[1] = operandLex.back();	//entry
									operandLex.pop_back();
									immT.oper1[0] = operandLex.back();	//table
									operandLex.pop_back();
									
									immT.oper[1] = operatorsLex.back();	//entry
									operatorsLex.pop_back();
									immT.oper[0] = operatorsLex.back();	//table
									operatorsLex.pop_back();
									immT.oper2[1] = operandLex.back();
									operandLex.pop_back();
									immT.oper2[0] = operandLex.back();
									operandLex.pop_back();
								}
								
								if(strcmp(operators.back().c_str(), "=") != 0){  //��z=z+z�ܨ�檺�����X 
									InsertTable0();
									operand.push_back(tSetLab.back().cont);
									operandLex.push_back(tSetLab.back().table);
									operandLex.push_back(tSetLab.back().entry);
									
									immT.result[0] = tSetLab.back().table;
									immT.result[1] = tSetLab.back().entry;
									immT.title = tSetLab.back().cont + " = " + immT.title;
									num = num + 1;
									immT.line = num;
									immSet.push_back(immT);
								}
								else{
									num = num + 1;
									immT.line = num;
									immSet.push_back(immT);
								}
									
								operators.pop_back() ;
							}	
						}	
						else if((operators.empty()) || (CheckDeli(sSet[i].tSet[w].cont, operators.back()))){
							operators.push_back(sSet[i].tSet[w].cont);
							operatorsLex.push_back(sSet[i].tSet[w].table);
							operatorsLex.push_back(sSet[i].tSet[w].entry);
						}
						else{
							if(strcmp(sSet[i].tSet[w].cont.c_str(), ")") == 0){		//�B�z�A�������B��						
								for(; strcmp(operators.back().c_str(), "(") != 0;){
									immT.oper2[1] = operandLex.back();	//entry
									operandLex.pop_back();
									immT.oper2[0] = operandLex.back();	//table
									operandLex.pop_back();
									immT.title = operand.back();
									
									operand.pop_back();
									immT.oper[1] = operatorsLex.back();	//entry
									operatorsLex.pop_back();
									immT.oper[0] = operatorsLex.back();	//table
									operatorsLex.pop_back();
									immT.oper1[1] = operandLex.back();
									operandLex.pop_back();
									immT.oper1[0] = operandLex.back();
									operandLex.pop_back();
									immT.title = operand.back() + operators.back() + immT.title;

									operand.pop_back();
									if(strcmp(operators.back().c_str(), "(") != 0){
										InsertTable0();
										operand.push_back(tSetLab.back().cont);
										operandLex.push_back(tSetLab.back().table);
										operandLex.push_back(tSetLab.back().entry);
										immT.result[0] = tSetLab.back().table;
										immT.result[1] = tSetLab.back().entry;
										immT.title = tSetLab.back().cont + " = " + immT.title;
										num = num + 1;
										immT.line = num;
										immSet.push_back(immT);
									}
									else{
										num = num + 1;
										immT.line = num;
										immSet.push_back(immT);
									}		
									
									operators.pop_back() ;
								}	
								operators.pop_back();	//pop out the "("
								operatorsLex.pop_back();	//pop out the "("'s entry 
								operatorsLex.pop_back();	//pop out the "("'s table
								count = count - 1;	//�Y�ѨM�A���N��@ 
							}// has "( )"
							else{
								while(!CheckDeli(sSet[i].tSet[w].cont, operators.back())){	//��stack�~��operator�p��stack���� 
									immT.oper2[1] = operandLex.back();	//entry				//����pop stack�����B�� 
									operandLex.pop_back();
									immT.oper2[0] = operandLex.back();	//table
									operandLex.pop_back();
									immT.title = operand.back();
									
									operand.pop_back();
									immT.oper[1] = operatorsLex.back();	//entry
									operatorsLex.pop_back();
									immT.oper[0] = operatorsLex.back();	//table
									operatorsLex.pop_back();
									immT.oper1[1] = operandLex.back();
									operandLex.pop_back();
									immT.oper1[0] = operandLex.back();
									operandLex.pop_back();
									immT.title = operand.back() + operators.back() + immT.title;

									operand.pop_back();
									if(strcmp(operators.back().c_str(), "=") != 0){
										InsertTable0();
										operand.push_back(tSetLab.back().cont);
										operandLex.push_back(tSetLab.back().table);
										operandLex.push_back(tSetLab.back().entry);
										immT.result[0] = tSetLab.back().table;
										immT.result[1] = tSetLab.back().entry;
										immT.title = tSetLab.back().cont + " = " + immT.title;
										num = num + 1;
										immT.line = num;
										immSet.push_back(immT);
									}
									else{
										num = num + 1;
										immT.line = num;
										immSet.push_back(immT);
									}
									
									operators.pop_back() ;
								}
								operators.push_back(sSet[i].tSet[w].cont);
								operatorsLex.push_back(sSet[i].tSet[w].table);
								operatorsLex.push_back(sSet[i].tSet[w].entry);
							} 
						}	
					}// operators
					else{	//put into operand
						operand.push_back(sSet[i].tSet[w].cont);
						operandLex.push_back(sSet[i].tSet[w].table);
						operandLex.push_back(sSet[i].tSet[w].entry);
					}// operand
				}//for Reverse Polish Notation 

			}//is an assignment 
		}// �̫e���O LABEL 
	}//for
	
	if(!forGTO.empty())	ForGTO(labelSet, forGTO);	//�B�zGTO��forward reference 
}

int main(void)
{   introMsg();                         //initiate an opening message
	InputTable();						//Ū�Jtable�ɮ�
	do
	{	resetList(tSetInt); //��l������� 
  		resetList(tSetRN);  
  		resetIDList(tSetId); 
  		resetListInfor(tSetInfor);
		cout << "##################################################" << endl;
    	cout << " Step 2: FRANCIS COMPILER        "                  << endl;
    	cout << "##################################################" << endl << endl;
//********************************************************************************************/
        try
        {	if(ReadWriteFile())			//�O�_���\Ūinput�ɡB�goutput�� 
				cout << endl << "=== The output file has been created!! ===" << endl;

			SetImmCode();
			for(int i = 0; i < errorList.size(); i++){
				fileOut << "Line: " << errorList[i].num+1 << setw(20) << errorList[i].cont << "\t" << " ===> Syntax Error\n";
			}
			
			for(int i = 0; i < immSet.size(); i++){
				fileOut << immSet[i].line << "\t((" << immSet[i].oper[0] << "," << immSet[i].oper[1] << ")\t,";	
				if(immSet[i].oper1[0] != -99)
					fileOut << "(" << immSet[i].oper1[0] << "," << immSet[i].oper1[1] << ")";
					
				fileOut << "\t,";
				
				if(immSet[i].oper2[0] != -99)
					fileOut << "(" << immSet[i].oper2[0] << "," << immSet[i].oper2[1] << ")";
					
				fileOut << "\t,";
				
				if(immSet[i].result[0] != -99)
					fileOut << "(" << immSet[i].result[0] << "," << immSet[i].result[1] << ")";

				fileOut << "\t)\t" << left << setw(20) << immSet[i].title << endl;
					
			}

			fileOut.close();		
			sSet.clear();
			immSet.clear();	
			arrayList.clear();
			tSetLab.clear();
			errorList.clear();
			labelList.clear();
            cout << endl << endl << "[0]Quit or [Any other]continue?" << endl;
            string quitOrNot;
			cin >> quitOrNot;
			if (!quitOrNot.compare("0")) // press 0 to stop execution
				return 0;
        }	// end try
//********************************************************************************************/
        catch (std::bad_alloc& ba)								            // unable to allocate dynamic space
        {   std::cerr << endl << "bad_alloc caught: " << ba.what() << endl;
        }   // end catch
	}while(1);
}	// end main
//********************************************************************************************/
