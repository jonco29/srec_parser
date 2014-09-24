#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "ucryptr_interface.h"
#include "combinedSrec2mem.h"
#include "MaceBlob.h"


using namespace std;

#define BB_3X                                           0x00
#define A7                                              0x01
#define A9                                              0x02
#define ALG                                             0x03

#define   START_IMAGE_OPCODE                            0xD3
#define   DATA_OPCODE                                   0xD5
#define   DOWNLOAD_COMPLETE_OPCODE                      0xD7
#define   DOWNLOAD_STATUS_OPCODE                        0xD8
#define   WAIT_FOR_NEXT_OPCODE                          0xD9
#define   CHANGE_KVL_RATE_OPCODE                        0xDC
#define   ERASE_APP_AREA_OPCODE                         0xE6
#define   ERASE_ALGO_BY_ID_OPCODE                       0xE7
#define   ERASE_ALGO_BY_SLOT_OPCODE                     0xE8
#define   SOFTWARE_AREA_ERASE_RESP_OPCODE               0xE9
#define   ARM7_ERASE_ALGO_BY_ID_OPCODE                  0xE3
#define   ARM7_ERASE_ALGO_BY_SLOT_OPCODE                0xE4

#ifndef INT8
typedef signed char     INT8;
#endif

#ifndef INT16
typedef signed short    INT16;
#endif

#ifndef INT32
typedef signed int      INT32;
#endif

#ifndef UINT8
typedef unsigned char   UINT8;
#endif

#ifndef UINT16
typedef unsigned short  UINT16;
#endif

#ifndef UINT32
typedef unsigned int    UINT32;
#endif

#ifndef  BOOLEAN
typedef unsigned char   BOOLEAN;
#endif


unsigned char* createStartImage(unsigned int  id, unsigned short algoId, int *len);
unsigned char* createDataDownload(unsigned char* inData, unsigned int* len);
unsigned char* createWaitForNextImage(unsigned int* len);
unsigned char* createDownLoadComplete(unsigned int* len);
void eraseSlots(uCryptrInterface &uc);
void createEraseAppImage(uCryptrInterface &uc, unsigned char id);
void doUpgrade(uCryptrInterface &uc, CombinedSRecord2Mem* srec);

unsigned int getNextUpgradeData(MaceBlob *m, unsigned char** data, unsigned int len);
void programBootBlock(uCryptrInterface &uc, MaceBlob *m);
void programARM9(uCryptrInterface &uc, MaceBlob *m);
void programAlgos(uCryptrInterface &uc, MaceBlob *m, bool algoErased);

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
    unsigned char* sendData = 0;
    while ( (m = srec->getNextImage()) != 0 )
    {
        switch (m->getId())
        {
            case BB_3X:
            {
                programBootBlock(uc, m);
                break;
            }
            case A7:
            {
                // same flow as arm7
                programARM9(uc, m);
                break;
            }
            case A9:
            {
                programARM9(uc, m);
                break;
            }
            case ALG:
            {
                programAlgos(uc, m, algoStarted);
                algoStarted = true;
                break;
            }
        }
    }
}

unsigned char* createStartImage(unsigned int  id, unsigned short algoId, int* len)
{
    unsigned char* msg = new unsigned char[5];
    *len = 5;
    msg[0] = START_IMAGE_OPCODE;
    msg[1] = 0;
    msg[2] = id;
    msg[3] = (algoId >> 8 ) & 0xff;
    msg[4] = (algoId >> 0 ) & 0xff;
    return msg;
}

unsigned char* createDataDownload(unsigned char* inData, unsigned int* len)
{
    unsigned char* outData = new unsigned char[*len + 5];
    UINT16 crc = 0xffff;

    outData[0] = DATA_OPCODE;
    outData[1] = 0;
    outData[2] = *len;
    outData[3] = 0;
    outData[4] = 0;
    memcpy(&outData[5], inData, *len);
    *len +=5;
    return outData;
}
unsigned char* createWaitForNextImage(unsigned int* len)
{
    unsigned char* msg = new unsigned char[2];
    *len = 2;
    msg[0] = WAIT_FOR_NEXT_OPCODE;
    msg[1] = 0;
    return msg;
}
unsigned char* createDownLoadComplete(unsigned int* len)
{
    unsigned char* msg = new unsigned char[2];
    *len = 2;
    msg[0] = DOWNLOAD_COMPLETE_OPCODE;
    msg[1] = 0;
    return msg;
}
void eraseSlots(uCryptrInterface &uc)
{
    unsigned char* msg = new unsigned char[3];
    unsigned int i;
    msg[0] = ERASE_ALGO_BY_SLOT_OPCODE;
    msg[1] = 0;
    msg[2] = 0;

    for (i = 0; i < 8; i++)
    {
        msg[2] = i & 0xff;
        printf ("erasing slot id: 0x%02X, and waiting 1 second for the write\n", i);
        uc.sendRaw( msg, 3, 3);
        printf ("done erasing slot id: 0x%02X\n", i);
    }
    delete msg;
}
void createEraseAppImage(uCryptrInterface &uc, unsigned char id)
{
    unsigned char* msg = new unsigned char[3];
    msg[0] = ERASE_APP_AREA_OPCODE;
    msg[1] = 0;
    msg[2] = id;
    printf ("erasing image id: 0x%02X, and waiting 7 seconds for the write\n", id);
    uc.sendRaw( msg, 3, 7);
    printf ("done erasing image\n");
    delete msg;
}

// len must be a multiple of 16
unsigned int getNextUpgradeData(MaceBlob *m, unsigned char** data, unsigned int len)
{
    unsigned int offset = 0;
    unsigned char tmpData[2048] = {0};

    if (len % 16)
    {
        return 0;
    }
    while (offset < len && m->getNextDataLen() != 0)
    {
        unsigned int len = 0;
        unsigned char* tmp = 0;
        len = m->getNextDataLen();
        tmp = m->getNextData();

        memcpy(&tmpData[offset], tmp, len);
        delete tmp;
        offset+= len;
    }
    if (offset > 0)
    {
        *data = new unsigned char[offset];
        memcpy(*data, tmpData, offset);
    }
    return offset;
}

void programBootBlock(uCryptrInterface &uc, MaceBlob *m)
{
    // we need to do the following
    //  - start image
    //  - iterate over data
    //  wait for next image
    //  d/l complete
    int xferLen = 0;
    unsigned char* sendData = 0;

    printf("starting to program boot block\n");

    sendData = createStartImage(m->getId(), m->getAlgoId(),  &xferLen);
    uc.sendRaw( sendData, xferLen);
    delete sendData;

    //return ;
    const unsigned int reqLen = 160;
    unsigned int len = 64;
    unsigned char* data = 0;
    int bogus = 0;
    while( (len = getNextUpgradeData(m, &data, reqLen)) > 0)
    {
        int i = 0;
        for (i = 0; i < len; i++)
        {
            printf("%02X", data[i]);
            if ((i+1) %16 == 0)
            {
                cout << endl;
            }
        }
        sendData = createDataDownload(data, &len);
        uc.sendRaw ((unsigned char*)sendData, len);
        delete data;
        delete sendData;
    }

    len = 0;
    // send wait for next image
    sendData = createWaitForNextImage(&len);
    cout << "sending wait for next image and sleeping 5 seconds to validate the image before d/l complete\n";
    uc.sendRaw( (unsigned char*)sendData, len, 5);
    delete sendData;

    cout << "sending d/l complte\n";
    sendData = createDownLoadComplete(&len);
    uc.sendRaw( (unsigned char*)sendData, len);
    delete sendData;

    sleep (1);
    while (true)
    {
        cout << "now you need to eject the card and re-insert twice:\n";
        cout << "have you done that?:  [y/n] ";
        char a;
        cin >> a;
        if (a == 'y' || a == 'Y')
        {
            break;
        }
    }
    printf("done programming boot block\n");

}

void programARM9(uCryptrInterface &uc, MaceBlob *m)
{
    // we need to do the following
    //  - start image
    //  - iterate over data
    //  wait for next image
    //  d/l complete
    int xferLen = 0;
    unsigned char* sendData = 0;


    printf("starting to program image id: 0x%02X\n", m->getId());
    createEraseAppImage(uc, m->getId());

    sendData = createStartImage(m->getId(), m->getAlgoId(),  &xferLen);
    uc.sendRaw( sendData, xferLen);
    delete sendData;

    //return ;
    const unsigned int reqLen = 160;
    unsigned int len = 64;
    unsigned char* data = 0;
    int bogus = 0;
    while( (len = getNextUpgradeData(m, &data, reqLen)) > 0)
    {
        // int i = 0;
        // for (i = 0; i < len; i++)
        // {
        //     printf("%02X", data[i]);
        //     if ((i+1) %16 == 0)
        //     {
        //         cout << endl;
        //     }
        // }
        sendData = createDataDownload(data, &len);
        uc.sendRaw ((unsigned char*)sendData, len);
        delete data;
        delete sendData;
    }

    len = 0;
    // send wait for next image
    sendData = createWaitForNextImage(&len);
    uc.sendRaw( (unsigned char*)sendData, len);
    delete sendData;
    printf("done programming app id: 0x%02X\n", m->getId());
}

void programAlgos(uCryptrInterface &uc, MaceBlob *m, bool algoErased)
{
    // we need to do the following
    //  - start image
    //  - iterate over data
    //  wait for next image
    //  d/l complete
    int xferLen = 0;
    unsigned char* sendData = 0;


    printf("starting to program algo id: 0x%02X\n", m->getAlgoId());
    if (algoErased == false)
    {
        eraseSlots(uc);
    }

    sendData = createStartImage(m->getId(), m->getAlgoId(),  &xferLen);
    uc.sendRaw( sendData, xferLen);
    delete sendData;

    //return ;
    const unsigned int reqLen = 160;
    unsigned int len = 64;
    unsigned char* data = 0;
    int bogus = 0;
    while( (len = getNextUpgradeData(m, &data, reqLen)) > 0)
    {
        // int i = 0;
        // for (i = 0; i < len; i++)
        // {
        //     printf("%02X", data[i]);
        //     if ((i+1) %16 == 0)
        //     {
        //         cout << endl;
        //     }
        // }
        sendData = createDataDownload(data, &len);
        uc.sendRaw ((unsigned char*)sendData, len);
        delete data;
        delete sendData;
    }

    len = 0;
    // send wait for next image
    sendData = createWaitForNextImage(&len);
    uc.sendRaw( (unsigned char*)sendData, len);
    delete sendData;

    printf("done programming algo id: 0x%02X\n", m->getAlgoId());

}
