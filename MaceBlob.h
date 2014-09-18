
#include <vector> 

#ifndef MACEBLOB_H
#define MACEBLOB_H

using namespace std;

class MaceBlob 
{
public:
    MaceBlob(unsigned char* data);
    bool addData(unsigned char* data, unsigned int len);

private:
    unsigned int length;
    unsigned char id;
    unsigned short algoId;
    vector<unsigned char*> data;

};

#endif // MACEBLOB_H
