/*
	Christian Schwartz
	CSCE 4550 : 002
	12/5/2016
	Implementation of AES algorithm.
	Takes user input of plaintext and key then preforms operations on the plaintext.
	Will print to console and file given after each operation.
	1. Remove whitespace and punctuation.
	2. Substitution for each character using Vinengere cipher
	3. Adds padding to end of message to make 4x4 blocks (if necessary)
	4. Shifts characters in row depending on row within 4x4 block
	5. Parity bit of each character using Ascii value. Adds parity bit if odd number of 1's in binary representation
	6. Mix columns using Rijndael's Galois Field method.
*/
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>
#include <bitset>

using namespace std;

//Function Prototypes
void readFile(string, string, string*, string*);
string Substitution(string, string);
string Padding(string);
vector<string> Shift(string);
vector<int> Parity(vector<string>);
void MixColumns(vector<int>&);
int rgfMul(int number, int multi);
void printPre(string, ofstream&);
void printSub(string, ofstream&);
void printPad(string, ofstream&);
void printShift(vector<string>, ofstream&);
void printVector(vector<int>, ofstream&);

int main()
{
	//Variables
	string plain_name, key_name, write_name; 		//User input for file names
	string key, plaintext, cyphertext;			//Information throughout process
	vector<string> shifting;				//Vector each row made while shifting
	vector<int> parity;					//Vector for each int after parity bit
	ofstream outFile;					//Out file
	
	//Files used for cryptography from user input
	cout << "File name containing plaintext: ";
	cin >> plain_name;
	cout << "File name containing key: ";
	cin >> key_name;
	cout << "File name to write: ";
	cin >> write_name;
	
	//Open file
	outFile.open(write_name.c_str());
	
	/********** Read Files **********/
	readFile(plain_name, key_name, &key, &plaintext);
	
	/********** Preprocess **********/
	printPre(plaintext, outFile);
	
	/********* Substitution *********/
	cyphertext = Substitution(plaintext, key);
	printSub(cyphertext, outFile);
	
	/*********** Padding ************/
	cyphertext = Padding(cyphertext);
	printPad(cyphertext, outFile);
	
	/*********** Shifting ***********/
	shifting = Shift(cyphertext);
	printShift(shifting, outFile);
	
	/********** Parity Bit **********/
	parity = Parity(shifting);
	cout << endl << "Parity: " << endl;
	outFile << endl << "Parity: " << endl;
	printVector(parity, outFile);
	
	/********* Mix Columns **********/
	MixColumns(parity);
	cout << endl << "Mix Columns: " << endl;
	outFile << endl << "Mix Columns: " << endl;
	printVector(parity, outFile);
	
	//Close file
	outFile.close();

	return 0;
}

void readFile(string plain_name, string key_name, string* key, string* plaintext)
{
	ifstream plainFile, keyFile;				//In files
	string buffer;						//Temp string for each line read
	
	//Open files
	plainFile.open(plain_name.c_str());
	keyFile.open(key_name.c_str());
	
	keyFile >> *key;
	
	//Grab each line from file
	//Remove whitespace and punctuation
	//Add to plaintext
	while(getline(plainFile, buffer))
	{
		buffer.erase(remove_if(buffer.begin(), buffer.end(), ::isspace), buffer.end());
		buffer.erase(remove_if(buffer.begin(), buffer.end(), ::ispunct), buffer.end());
			
		*plaintext += buffer;
	}	
	
	//Close files
	plainFile.close();
	keyFile.close();
};

void printPre(string plaintext, ofstream& outFile)
{
	cout << endl << "Preprocessing: " << endl << plaintext << endl;
	outFile << endl << "Preprocessing: " << endl << plaintext << endl;
}

string Substitution(string buffer, string key)
{
	//Compares each character in buffer to key
	for(int i = 0, j = 0; i < buffer.length(); i++, j++)
	{
		//If at end of key, start over
		if(j == key.length())
			j = 0;
		
		//Subtract key character from buffer character
		//Use %26 to stay within the alphabet
		//Add 'A' to get ascii value back into range of A-Z
		buffer[i] = (((buffer[i] - 'A') + (key[j] - 'A')) % 26) + 'A';
	}
	
	return buffer;
}

void printSub(string cyphertext, ofstream& outFile)
{
	cout << endl << "Substitution: " << endl << cyphertext << endl;
	outFile << endl << "Substitution: " << endl << cyphertext << endl;
}

string Padding(string buffer)
{
	//Finds how many 'A' to add so %16 = 0
	int amount = 16 - (buffer.length() % 16);
	
	for(int i = 0; i < amount; i++)
		buffer += 'A';

	return buffer;
}

void printPad(string cyphertext, ofstream& outFile)
{
	cout << endl << "Padding: " << endl;
	outFile << endl << "Padding: " << endl;
	
	for(int i = 0; i < cyphertext.size(); i++)
	{
		cout << cyphertext[i];
		outFile << cyphertext[i];
		
		//Formatting
		if( (i + 1) % 4 == 0)
		{
			cout << endl;
			outFile << endl;
		}
		
		if( (i + 1) % 16 == 0)
		{
			cout << endl;
			outFile << endl;
		}	

	}
}

vector<string> Shift(string cyphertext)
{
	vector<string> message;					//Vector for each row added
	string sub;							//4 character strings
	
	//Shifts characters in a 4 column row
	for(int i = 0, j = 0; i < cyphertext.size(); i += 4, j++)
	{
		//Shifts depend on what row in the 4x4 block
		switch(j % 4)
		{
			//No shift
			case 0:
				sub += cyphertext[i];
				sub += cyphertext[i + 1];
				sub += cyphertext[i + 2];
				sub += cyphertext[i + 3];
				break;
				
			//Shift 1 left
			case 1:
				sub += cyphertext[i + 1];
				sub += cyphertext[i + 2];
				sub += cyphertext[i + 3];
				sub += cyphertext[i];
				break;
			
			//Shift 2 left
			case 2:
				sub += cyphertext[i + 2];
				sub += cyphertext[i + 3];
				sub += cyphertext[i];
				sub += cyphertext[i + 1];
				break;
			
			//Shift 3 left
			case 3:
				sub += cyphertext[i + 3];
				sub += cyphertext[i];
				sub += cyphertext[i + 1];
				sub += cyphertext[i + 2];
				break;
		}
		
		//Add to vector and reset sub string
		message.push_back(sub);
		sub = "";
	}
	
	return message;
}

void printShift(vector<string> shifting, ofstream& outFile)
{
	cout << endl << "Shift: " << endl;
	outFile << endl << "Shift: " << endl;
	
	for(int i = 0; i < shifting.size(); i++)
	{
		cout << shifting[i] << endl;
		outFile << shifting[i] << endl;
		
		//Formatting
		if( (i + 1) % 4 == 0)
		{
			cout << endl;
			outFile << endl;
		}
	}
}

vector<int> Parity(vector<string> shift)
{
	vector<int> parity;						//Each int after parity calculation
	int ascii, count = 0;					//Ascii rep, count of '1's in binary rep
	string binary;						//Binary rep of character
	
	//Each row in shifted cyphertext
	for(int i = 0; i < shift.size(); i++)
	{
		//Each character in the row
		for(int j = 0; j < 4; j++)
		{
			//Get ascii value and binary value of character
			ascii = (int)shift[i][j];
			binary = bitset<8>(ascii).to_string();
			
			//Count 1's in binary value
			for(int k = 0; k < binary.size(); k++)
				if(binary[k] == '1')
					count++;
			
			/*		Testing
			cout << shift[i][j] << " " << ascii << " " << binary << " " << count << "\t";
			*/
			
			//If count is odd add '1' to the beginning
			if(count % 2 != 0)
				binary[0] = '1';
			
			//Add decimal representation to vector
			parity.push_back(bitset<8>(binary).to_ulong());
		
			/*		Testing
			cout << dec << set[j] << " --> ";
			cout << hex << set[j] << endl;
			cout << dec;
			*/
			
			count = 0;
		}	
	}
	
	return parity;
}

void MixColumns(vector<int> & parity)
{	
	int a0, a1, a2, a3;						//Row positions in column
	
	//i goes through every 4x4 block
	for(int i = 0; i < parity.size(); i += 16)	
		for(int j = 0; j < 4; j++)				//j goes through each column
		{
			a0 = parity[i + j];
			a1 = parity[i + j + 4];
			a2 = parity[i + j + 8];
			a3 = parity[i + j + 12];
			
			//Uses Rijndael's Galois Field to caluclate new column values
			parity[i + j] = rgfMul(a0, 2) ^ rgfMul(a1, 3) ^ a2 ^ a3;
			parity[i + j + 4] = a0 ^ rgfMul(a1, 2) ^ rgfMul(a2, 3) ^ a3;
			parity[i + j + 8] = a0 ^ a1 ^ rgfMul(a2, 2) ^ rgfMul(a3, 3);
			parity[i + j + 12] = rgfMul(a0, 3) ^ a1 ^ a2 ^ rgfMul(a3, 2);
		}
}

int rgfMul(int number, int multi)
{
	int shifted;						//number * 2
	string num = bitset<8>(number).to_string();	//Binary of original number
	
	//Multiplying by 2 by shifting 1 left
	shifted = number << 1;
	shifted = bitset<8>(shifted).to_ulong();
	
	//If multiplying by 3
	//XOR our shifted result by the original
	if(multi == 3)
		shifted = shifted ^ number;
		
	//Test is original MSB is '1'
	//if so, XOR with 27
	if(num[0] == '1')
		shifted = shifted ^ 27;
	
	return shifted;
}

void printVector(vector<int> parity, ofstream& outFile)
{
	cout << hex;
	outFile << hex;
	
	for(int i = 0; i < parity.size(); i++)
	{
		cout << parity[i] << " ";
		outFile << parity[i] << " ";

		//Formatting
		if( (i + 1) % 4 == 0)
		{
			cout << endl;
			outFile << endl;
		}
	}
	
	cout << dec;
}