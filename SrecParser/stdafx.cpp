// stdafx.cpp : source file that includes just the standard includes
// SrecParser.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"
#include <iostream>
#include "combinedSrec2mem.h"
#include "MaceBlob.h"
using namespace std;


// TODO: reference any additional headers you need in STDAFX.H
// and not in this file

int main(int argc, char** argv)
{
	CombinedSRecord2Mem *srec = 0;

	cout << "hello world, this is argc: " << argc << endl;
	for (int i = 0; i < argc; i++)
	{
		cout << "argc[" << i << "]: " << argv[i] << endl;
	}
	if (argc == 2)
	{
		srec = new CombinedSRecord2Mem(argv[1]);
	}
	cout << "this is srec: " << srec << endl;

	return 0;
}