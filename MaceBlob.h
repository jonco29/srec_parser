#include <vector> 
#include <deque> 
#include <string.h> 
#include <iostream> 

#ifndef MACEBLOB_H
#define MACEBLOB_H

using namespace std;

class DataBlob
{
    public:
        DataBlob (unsigned char* inData, unsigned int inLen) :data(inData), len(inLen){}
        ~DataBlob () 
        {
            cout <<"calling: ~DataBlob\n";
            delete data;
        }

        DataBlob(const DataBlob &other) {
            cout << "DataBlob(DataBlob const &other) --- called" << endl;
            len = other.len;
            data = new unsigned char[len];
            memcpy (data, other.data, len);
        }
        DataBlob& operator=(const DataBlob &other) 
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
    MaceBlob(const MaceBlob &other);
    MaceBlob& operator= (const MaceBlob &other) ;

    bool addData(unsigned char* data, unsigned int len);
    unsigned int getNextDataLen();
    unsigned char* getNextData();
    unsigned int getLength() {return length;}
    unsigned char getId() {return id;}
    unsigned short getAlgoId() {return algoId;}


private:
    unsigned int length;
    unsigned char id;
    unsigned short algoId;
    deque<DataBlob*> data;
    deque<DataBlob*>::iterator it;
    unsigned int numRecords;

};

#endif // MACEBLOB_H
