#include <iostream>
#include <string>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "ucryptr_interface.h"
#include "combinedSrec2mem.h"
#include "MaceBlob.h"


using namespace std;

#define BB_3X           0x00
#define A7              0x01
#define A9              0x02
#define ALG             0x03


unsigned char* createStartImage(unsigned int  id, unsigned short algoId, int *len);
unsigned char* createDataDownload(unsigned char* inData, unsigned int* len);
unsigned char* createWaitForNextImage(void);
unsigned char* createDownLoadComplete(void);
unsigned char* eraseSlots(uCryptrInterface &uc);
void doUpgrade(uCryptrInterface &uc, CombinedSRecord2Mem* srec);

int main (int argc, char** argv)
{
    FILE *fstream;
    uCryptrInterface uc;
    CombinedSRecord2Mem *srec = 0;

    if (argc != 2)
    {
        cout << "usage: " << argv[0] << " <combined_srec>" << endl;
        return 1;
    }

    // read in the srec
    srec = new CombinedSRecord2Mem(argv[1]);


    if (uc.isReady())
    {
          cout << "uCryptR is ready........." << endl;
    }
    else
    {
        cout << "uCryptR is NOT ready........." << endl;
        return 1;
    }


    doUpgrade(uc, srec);


    delete srec;
    cout <<"all done....\n";
}
void doUpgrade(uCryptrInterface &uc, CombinedSRecord2Mem* srec)
{
    bool retVal = false;
    MaceBlob *m;
    bool algoStarted = false;
    while ( (m = srec->getNextImage()) != 0 )
    {
        switch (m->getId())
        {
            case BB_3X:
            {
                // we need to do the following
                //  - start image
                //  - iterate over data
                //  wait for next image
                //  d/l complete
                int xferLen = 0;
                unsigned char *sendData = createStartImage(m->getId(), m->getAlgoId(),  &xferLen);
                uc.sendRaw( sendData, xferLen);
                
                return ;


                // while (m->getNextDataLen() != 0)
                // {
                //     unsigned int len = 0;
                //     unsigned char* data = 0;
                //     len = m->getNextDataLen();
                //     data = m->getNextData();
                //     int i;
                //     for (i = 0; i < len; i++)
                //     {
                //         printf("%02X", data[i]);
                //     }
                //     cout << endl;
                //     char* sendData = uc.formatData(data, (int *)&len);
                //     uc.sendRaw( (unsigned char*)sendData, len);
                //     delete data;
                //     delete sendData;
                // }
            }
        }
    }
}

unsigned char* createStartImage(unsigned int  id, unsigned short algoId, int* len)
{
    unsigned char* msg = new unsigned char[5];
    *len = 5;
    msg[0] = 0xd3;
    msg[1] = 0;
    msg[2] = id;
    msg[3] = (algoId >> 8 ) & 0xff;
    msg[4] = (algoId >> 0 ) & 0xff;
    return msg;
}

unsigned char* createDataDownload(unsigned char* inData, unsigned int* len)
{
    return 0;
}
unsigned char* createWaitForNextImage(void)
{
    return 0;
}
unsigned char* createDownLoadComplete(void)
{
    return 0;
}
unsigned char* eraseSlots(uCryptrInterface &uc)
{
    return 0;
}

