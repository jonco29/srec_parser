#include <vector> 
#include <string.h> 
#include <iostream> 

#ifndef MACEBLOB_H
#define MACEBLOB_H

using namespace std;

class DataBlob
{
    public:
        DataBlob (unsigned char* inData, unsigned int inLen) :data(inData), len(inLen){}
        ~DataBlob () {delete data;}

        DataBlob(DataBlob const &other) {
            cout << "DataBlob(DataBlob const &other) --- called" << endl;
            len = other.len;
            data = new unsigned char[len];
            memcpy (data, other.data, len);
        }
        DataBlob& operator=(DataBlob const &other) 
        {
            cout << "DataBlob& operator=(DataBlob const &other) -- called" << endl;
            len = other.len;
            data = new unsigned char[len];
            memcpy (data, other.data, len);
        }

        unsigned char* data;
        unsigned int len;
};
    

class MaceBlob 
{
public:
    MaceBlob(unsigned char* data);
    ~MaceBlob();
    MaceBlob(MaceBlob &other);
    MaceBlob& operator= (MaceBlob const &other) 
    {
        cout << "MaceBlob::MaceBlob& operator =(MaceBlob const &other) -- called" << endl;
        length = other.length;
        id = other.id;
        algoId = other.algoId;
        data = other.data;
    }

    bool addData(unsigned char* data, unsigned int len);

private:
    unsigned int length;
    unsigned char id;
    unsigned short algoId;
    vector<DataBlob*> data;
    vector<DataBlob*>::iterator it;

};

#endif // MACEBLOB_H
