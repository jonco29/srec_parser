#include "MaceBlob.h"
#include <iostream>
#include <vector>
#include <deque>
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
    numRecords++;
    
    return false;
}

MaceBlob::~MaceBlob()
{
    for (int i = 0; i < data.size(); i++)
    {
        delete data[i];
    }
}

MaceBlob::MaceBlob(const MaceBlob &other)
{
    // cout << "MaceBlob::MaceBlob(MaceBlob &other) -- called " << endl;
    length = other.length;
    id = other.id;
    algoId = other.algoId;
    data = other.data;
    numRecords = other.numRecords;
}

MaceBlob& MaceBlob::operator= (const MaceBlob &other) 
{
    // cout << "MaceBlob::MaceBlob& operator =(MaceBlob const &other) -- called" << endl;
    length = other.length;
    id = other.id;
    algoId = other.algoId;
    data = other.data;
    numRecords = other.numRecords;
}
unsigned int MaceBlob::getNextDataLen()
{
    unsigned int len = 0;
    if (data.empty() == false)
    {
        len = data.front()->len;
    }
    return len;
}
unsigned char* MaceBlob::getNextData()
{
    unsigned char* d = 0;
    if (data.empty() == false)
    {
        d = data.front()->data;
        data.front()->data = 0;
        data.pop_front();
    }
    return d;
}
// MaceBlob* CombinedSRecord2Mem::getNextImag()
// {
//     //MaceBlob *m = images[0].getImage();
//     MaceBlob *m = 0;
//     if (images.empty() == false)
//     {
//         m = images.front().getImage();
//         images.pop_front();
//     }
//     return m;
// }
