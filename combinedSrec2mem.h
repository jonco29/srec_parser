#include "srec.h"
#include "SRecMem.h"

#ifndef COMBINEDSREC2MEM_H
#define COMBINEDSREC2MEM_H

class CombinedSRecord2Mem : public SRecordParser
{
public:
   CombinedSRecord2Mem(const char* file);
   virtual ~CombinedSRecord2Mem();
   virtual  bool  Data( const SRecordData *sRecData );
   int getNextData(unsigned char** ptr, int len);
   int getSrecLength();

protected:
   virtual  bool  FinishSegment( unsigned addr, unsigned len );
private:
   FILE *file;
   FILE* getFile();
   bool openFile(const char* name);
   unsigned char *dataArray;
   unsigned int dataArraySize;
   unsigned int currentDataOffset;
   unsigned int mLength;
};

#endif
