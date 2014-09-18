#include "MaceBlob.h"
#include <iostream>
#include <vector>

using namespace std;

MaceBlob::MaceBlob(unsigned char* data)
{
    id = data[0];
    length = 0;
    length |= data[9] << 16;
    length |= data[10] << 8;
    length |= data[11] << 0;
    algoId = 0;
    algoId |= data[16] << 8;
    algoId |= data[17] << 0;
}

bool MaceBlob::addData(unsigned char* data, unsigned int len)
{
    return false;
}

// private:
//     unsigned int length;
//     unsigned char id;
//     unsigned short algoId;
//     vector<unsigned char*> data;
// 
// };
