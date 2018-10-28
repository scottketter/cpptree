#ifndef VER_SUPPORT
#define VER_SUPPORT	1.0;

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>

// name space
using namespace std;

// prototypes
vector<string> split(const string &str, char delimiter);
bool fileexists(const string filename);

// split a string into a vector of strings that are separated by a delimiter
vector<string> split(const string &str, char delimiter){

	string Token;
	vector<string> Tokens;
	istringstream TokenStream(str);

	// get each token and place it in the vector
	while(getline(TokenStream, Token, delimiter)) Tokens.push_back(Token);

	// return the vector
	return Tokens;

}

// check if a file exists
bool fileexists(const string filename){

	// open the file
	ifstream File(filename.c_str());

	// return the status
	return File.good();

}

#endif
