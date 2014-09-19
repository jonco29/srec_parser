#include "MaceBlob.h"
#include <iostream>
#include <vector>
#include <string.h>

using namespace std;

MaceBlob::MaceBlob(unsigned char* inData)
{
    id = inData[0];
    length = 0;
    length |= inData[9] << 16;
    length |= inData[10] << 8;
    length |= inData[11] << 0;
    algoId = 0;
    algoId |= inData[16] << 8;
    algoId |= inData[17] << 0;
}

bool MaceBlob::addData(unsigned char* inData, unsigned int len)
{
    unsigned char *d = new unsigned char[len];
    memcpy(d, inData, len);
    data.push_back(new DataBlob(d, len));
    
    return false;
}

MaceBlob::~MaceBlob()
{
    for (int i = 0; i < data.size(); i++)
    {
        delete data[i];
    }
}

MaceBlob::MaceBlob(MaceBlob &other)
{
    cout << "MaceBlob::MaceBlob(MaceBlob &other) -- called " << endl;
    length = other.length;
    id = other.id;
    algoId = other.algoId;
    data = other.data;
}

/*
private:
    unsigned int length;
    unsigned char id;
    unsigned short algoId;
    vector<DataBlob*> data;
    vector<DataBlob*>::iterator it;
    */
