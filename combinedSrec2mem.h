#include <vector>
#include <iostream>
#include "srec.h"
#include "SRecMem.h"
#include "MaceBlob.h"

#ifndef COMBINEDSREC2MEM_H
#define COMBINEDSREC2MEM_H

using namespace std;

// forward declare this, b/c defined below
class CombinedSrecImageData;

class CombinedSRecord2Mem : public SRecordParser
{
public:
   CombinedSRecord2Mem(const char* file);
   virtual ~CombinedSRecord2Mem();
   virtual  bool  Data( const SRecordData *sRecData );
   MaceBlob* getNextImage();

protected:
   virtual  bool  FinishSegment( unsigned addr, unsigned len );
private:
   FILE *file;
   FILE* getFile();
   bool openFile(const char* name);
   bool combinedSrecValidated;
   bool combinedSrecParsed;
   vector <CombinedSrecImageData> images;
   vector <CombinedSrecImageData>::iterator imageIt;

};

class CombinedSrecImageHeader : public SRecordParser
{
public:
    CombinedSrecImageHeader(FILE *f, const char* initialData);
    ~CombinedSrecImageHeader();
    virtual  bool  Data( const SRecordData *sRecData );
    unsigned char* getData();

private:
    FILE *file;
    unsigned char data[32];
};

class CombinedSrecImageData : public SRecordParser
{
public:
    CombinedSrecImageData(unsigned int addr);
    ~CombinedSrecImageData();
    CombinedSrecImageData(const CombinedSrecImageData &other)
    {
        cout << "CombinedSrecImageData(CombinedSrecImageData &other) -- called" << endl;
    }
    // CombinedSrecImageData& operator= (CombinedSrecImageData const &other) 
    // {
    //     cout << "CombinedSrecImageData& operator= (CombinedSrecImageData const &other) -- called" << endl;
    // }
    void setNextAddr(unsigned int addr);
    void setHeader(unsigned char* headerData);
    virtual  bool  Data( const SRecordData *sRecData );
    unsigned int getAddress();
    unsigned int getNextAddress();
    MaceBlob *getImage();

private:
    unsigned int address;
    unsigned int nextAddress;
    unsigned char header[32];
    unsigned char* data;
    unsigned int dataLength;
    unsigned int index;
    MaceBlob *blob;
};

#endif
