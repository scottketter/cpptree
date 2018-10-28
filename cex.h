#ifndef VER_CEX
#define VER_CEX	1.0;

#include <iostream>
#include <string>
#include <sstream>

using namespace std;

// custom base exception class used for additional exception information
class cEx{

	protected:

	int Line = 0;
	double Val = 0;
	string File = "";
	string Func = "";
	string Ref = "";
	string Info = "";

	public:

	cEx(string file, string func, int line, string ref, double val){
		File = file; Func = func; Line = line; Ref = ref; Val = val;
		stringstream msg;
		msg << "Exception on line " << line << " in " << file << " for function " << func << " : " << ref << " (" << val << ")";
		Info = msg.str();
	}

	// get a full exception description
	virtual string getEx() const{return Info;}

	// get individual exception information
	int getLine() const{return Line;}
	string getFile() const{return File;}
	string getFunc() const{return Func;}
	string getRef() const{return Ref;}
	double getVal() const{return Val;}

};

// custom derived exception for values out of range
class cExBadRange : public cEx{

	double Min = 0;
	double Max = 0;

	public:

	cExBadRange(string file, string func, int line, string ref, double val, double min, double max) : cEx(file, func, line, ref, val){
		Min = min; Max = max;
		stringstream msg;
		msg << " : Value out of range (min=" << min << ",max=" << max << ")";
		Info += msg.str();
	}

	// get individual exception information
	double getMin() const{return Min;}
	double getMax() const{return Max;}

};

// custom derived exception for an invalid value
class cExBadValue : public cEx{

	public:

	cExBadValue(string file, string func, int line, string ref, double val) : cEx(file, func, line, ref, val){
		stringstream msg;
		msg << " : Bad value";
		Info += msg.str();
	}

};

// custom derived exception for an object that is already initialized
class cExAlreadyInit : public cEx{

	public:

	cExAlreadyInit(string file, string func, int line, string ref, double val) : cEx(file, func, line, ref, val){
		stringstream msg;
		msg << " : Object already initialized";
		Info += msg.str();
	}

};

// custom derived exception for memory allocation
class cExBadAlloc : public cEx{

	public:

	cExBadAlloc(string file, string func, int line, string ref, double val) : cEx(file, func, line, ref, val){
		stringstream msg;
		msg << " : Memory allocation error";
		Info += msg.str();
	}

};

// custom derived exception for an invalid object state
class cExBadState : public cEx{

	string State = "";

	public:

	cExBadState(string file, string func, int line, string ref, double val, string state) : cEx(file, func, line, ref, val){
		State = state;
		stringstream msg;
		msg << " : Invalid object state (" << state << ")";
		Info += msg.str();
	}

	// get individual exception information
	string getState() const{return State;}

};

// custom derived exception for an error in reading a file
class cExFileRead : public cEx{

	public:

	cExFileRead(string file, string func, int line, string ref, double val) : cEx(file, func, line, ref, val){
		stringstream msg;
		msg << " : Can't read";
		Info += msg.str();
	}

};

// custom derived exception for an error in writing a file
class cExFileWrite : public cEx{

	public:

	cExFileWrite(string file, string func, int line, string ref, double val) : cEx(file, func, line, ref, val){
		stringstream msg;
		msg << " : Can't write";
		Info += msg.str();
	}

};

// custom derived exception for an error in opening a file
class cExFileOpen : public cEx{

	public:

	cExFileOpen(string file, string func, int line, string ref, double val) : cEx(file, func, line, ref, val){
		stringstream msg;
		msg << " : Can't open";
		Info += msg.str();
	}

};

// custom derived exception for an error in closing a file
class cExFileClose : public cEx{

	public:

	cExFileClose(string file, string func, int line, string ref, double val) : cEx(file, func, line, ref, val){
		stringstream msg;
		msg << " : Can't close";
		Info += msg.str();
	}

};

// custom derived exception for an error in deleting a file
class cExFileDelete : public cEx{

	public:

	cExFileDelete(string file, string func, int line, string ref, double val) : cEx(file, func, line, ref, val){
		stringstream msg;
		msg << " : Can't delete";
		Info += msg.str();
	}

};

// custom derived exception for an error in renaming a file
class cExFileRename : public cEx{

	string Name = "";

	public:

	cExFileRename(string file, string func, int line, string ref, double val, string name) : cEx(file, func, line, ref, val){
		Name = name;
		stringstream msg;
		msg << " : Can't rename (" << name << ")";
		Info += msg.str();
	}

	// get individual exception information
	string getName() const{return Name;}

};

#endif
