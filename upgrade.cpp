#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef WIN32
#include <unistd.h>
#include "ucryptr_interface_linux.h"
#else
#include <Windows.h>
#include "ucryptr_interface_windows.h"
#endif
#include "combinedSrec2mem.h"
#include "MaceBlob.h"
#include "crypto_transport_service.h"
#include "spgc_types.h"
#include <Events.h>


using namespace std;

#define BB_3X                                           0x00
#define A7                                              0x01
#define A9                                              0x02
#define ALG                                             0x03

#define   ENTER_PROGRAMMING_MODE_OPCODE                 0xD1
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

#define   PAL_CTRL_BYTE                                 0x39

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

void mySleep(int seconds);

unsigned char* createStartImage(unsigned int  id, unsigned short algoId, int *len);
unsigned char* createDataDownload(unsigned char* inData, unsigned int* len);
unsigned char* createWaitForNextImage(unsigned int* len);
unsigned char* createDownLoadComplete(unsigned int* len);
void enterProgMode(uCryptrInterface *uc);
void eraseSlots(uCryptrInterface *uc);
void createEraseAppImage(uCryptrInterface *uc, unsigned char id);
void doUpgrade(uCryptrInterface *uc, CombinedSRecord2Mem* srec);

unsigned int getNextUpgradeData(MaceBlob *m, unsigned char** data, unsigned int len);
void programBootBlock(uCryptrInterface *uc, MaceBlob *m);
void programARM9(uCryptrInterface *uc, MaceBlob *m);
void programAlgos(uCryptrInterface *uc, MaceBlob *m, bool algoErased);


UCHAR *current_msg_ptr = NULL;

UINT8 module_info_request[] =     {0x80, 0x01, 0x82, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };


int i = 0;
VOID messageReceivedCallbackPrint(const  UINT8 * buffer, UINT32 bufferLength)
{
    printf_s("JONATHAN ------------  messageReceivedCallbackPrint ::::::::::::  Callback: Received %d bytes\n", bufferLength);
    if ( *(UINT32 *) &buffer[4] == *(UINT32 *) &current_msg_ptr[4]) {
       printf_s ("**********iteration 0x%X matches! Value 0x%X\n", i, *(UINT32 *) &buffer[4]);    
    } else {
       printf_s ("!!!!!!!!!iteration 0x%X does not match! Value  0x%X \n",  i, *(UINT32 *) &buffer[4]);
    }
    return;
}

VOID linkEventOccurredCallbackPrint(UINT32 eventType, UINT8* eventData, UINT32 eventDataLength)
{
	if (eventType == EXTERNAL_EVENT_ID_LINK_UP) {
		printf_s("JONATHAN ------------  linkEventOccurredCallbackPrint ::::::::::::  LINK IS UP!!!!!!!!!          Callback\n");
	} else if (eventType == EXTERNAL_EVENT_ID_LINK_DOWN) {
		printf_s("JONATHAN ------------  linkEventOccurredCallbackPrint ::::::::::::  LINK IS DOWN :'(   wahhhh!!! Callback\n");
	}
    return;
}


VOID loggingCallbackPrint(UINT32 errorCode, TCHAR *location, UINT8 loggingLevel, VOID * m1, VOID * m2, VOID * m3)

{
    int i = 0;

    switch(errorCode)
    {
        case SPGC_LOG_CODE_TRANSPORT_EVENT:
        {
            break;
        }
        case SPGC_LOG_CODE_TRANSPORT_DATA_OUT:
        {
			printf_s("JONATHAN ------------  loggingCallbackPrint ::::::::::::  Callback\n" );
            printf_s(("%s traced buffer of length %x [out] "), location, (int) m2);

            for(i=0; (i < (int) m2); i++)
            {
                printf_s((" %02X"), ((UINT8 *)m1)[i]);
            }
            printf_s("\n");
            break;
        }
        case SPGC_LOG_CODE_TRANSPORT_DATA_IN:
        {
			printf_s("JONATHAN ------------  loggingCallbackPrint ::::::::::::  Callback\n" );
            printf_s(("%s traced buffer of length %x [in] "), location, (int) m2);

            for(i=0; (i < (int) m2); i++)
            {
                printf_s((" %02X"), ((UINT8 *)m1)[i]);
            }
            printf_s("\n");
            break;
        }
        default:
        break;
    }
    return ;
}
int main (int argc, char** argv)
{
    uCryptrInterfaceWindows *uc = uCryptrInterfaceWindows::getCryptrInterface();
    CombinedSRecord2Mem *srec = 0;
    CTRANS_RECEIVER_INFO_T callbackReceiver;
    // callbackReceiver.messageReceivedCallback = messageReceivedCallbackPrint;
    // callbackReceiver.linkEventOccurredCallback = linkEventOccurredCallbackPrint;
    // callbackReceiver.loggingCallback = loggingCallbackPrint;
    callbackReceiver.messageReceivedCallback = 0;
    callbackReceiver.linkEventOccurredCallback = 0;
    callbackReceiver.loggingCallback = 0;
    uc->setCallBackReceiver(callbackReceiver);

    if (argc != 2)
    {
        cout << "usage: " << argv[0] << " <combined_srec>" << endl;
        return 1;
    }

    // read in the srec
    srec = new CombinedSRecord2Mem(argv[1]);


    if (uc->isReady())
    {
          cout << "uCryptR is ready........." << endl;
    }
    else
    {
        cout << "uCryptR is NOT ready........." << endl;
        return 1;
    }
    // cout << "test with my own, canned message\n";
    // current_msg_ptr = module_info_request;
    // uc->sendRawNoRx(current_msg_ptr, sizeof(module_info_request));
    // cout << "done testing with my own, canned message\n";


    doUpgrade(uc, srec);


    delete srec;
    cout <<"\nall done....\n";
}

void doUpgrade(uCryptrInterface *uc, CombinedSRecord2Mem* srec)
{
    bool retVal = false;
    bool progModeEntered = false;
    MaceBlob *m;
    bool algoStarted = false;
    unsigned char* sendData = 0;
    unsigned char* response = 0;
    unsigned int readLen = 0;

    while ( (m = srec->getNextImage()) != 0 )
    {
        // we need to enter prog mode
        if (progModeEntered == false)
        {
            enterProgMode(uc);
            progModeEntered = true;
        }

        switch (m->getId())
        {
            case BB_3X:
            {
                cout <<"\nProgramming Boot Block\n";
                programBootBlock(uc, m);
                progModeEntered = false;
                break;
            }
            case A7:
            {
                cout <<"\nProgramming ARM7 application\n";
                // same flow as arm7
                programARM9(uc, m);
                break;
            }
            case A9:
            {
                cout <<"\nProgramming ARM9 application\n";
                programARM9(uc, m);
                break;
            }
            case ALG:
            {
                cout <<"\nProgramming Algorithms\n";
                programAlgos(uc, m, algoStarted);
                algoStarted = true;
                break;
            }
        }
    }
    if (progModeEntered == true)
    {
        unsigned int len = 0;
        cout << "\nsending d/l complte\n";
        sendData = createDownLoadComplete(&len);
        //uc->sendRaw( (unsigned char*)sendData, len);
        if (uc->sendRawNoRx(sendData, len))
        {
            response = uc->getResponse(&readLen);
            delete response;

            mySleep (1);
            uc->resetUC();
        }
        else
        {
            exit(1);
        }
        delete sendData;
    }
}

unsigned char* createStartImage(unsigned int  id, unsigned short algoId, int* len)
{
    *len = 6;
    //unsigned char* msg = new unsigned char[*len];
    unsigned char* startImageMsg = new unsigned char[20];
    memset(startImageMsg, 0, 6);
    startImageMsg[0] = PAL_CTRL_BYTE;
    startImageMsg[1] = START_IMAGE_OPCODE;
    startImageMsg[2] = 0;
    startImageMsg[3] = id;
    startImageMsg[4] = (algoId >> 8 ) & 0xff;
    startImageMsg[5] = (algoId >> 0 ) & 0xff;
    return startImageMsg;
}

unsigned char* createDataDownload(unsigned char* inData, unsigned int* len)
{
    unsigned char* outData = new unsigned char[*len + 6];
    UINT16 crc = 0xffff;

    outData[0] = PAL_CTRL_BYTE;
    outData[1] = DATA_OPCODE;
    outData[2] = 0;
    outData[3] = *len;
    outData[4] = 0;
    outData[5] = 0;
    memcpy(&outData[6], inData, *len);
    *len +=6;
    return outData;
}
unsigned char* createWaitForNextImage(unsigned int* len)
{
    *len = 3;
    unsigned char* msg = new unsigned char[*len];
    msg[0] = PAL_CTRL_BYTE;
    msg[1] = WAIT_FOR_NEXT_OPCODE;
    msg[2] = 0;
    return msg;
}
unsigned char* createDownLoadComplete(unsigned int* len)
{
    *len = 3;
    unsigned char* msg = new unsigned char[*len];
    msg[0] = PAL_CTRL_BYTE;
    msg[1] = DOWNLOAD_COMPLETE_OPCODE;
    msg[2] = 0;
    return msg;
}
void enterProgMode(uCryptrInterface *uc)
{
    
    const unsigned int len = 3;
    unsigned char* msg = new unsigned char[len];
    unsigned char* response = 0;
    unsigned int readLen = 0;
    bool appMode = false;

    printf("\nentering program mode\n");

    // first send a link setup
    msg[0] = 0x80;
    msg[1] = 0x01;
    msg[2] = 0x82;
    //uc->sendRaw( msg, len);
    if (uc->sendRawNoRx(msg, len))
    {
        response = uc->getResponse(&readLen);
    }
    else
    {
        exit(1);
    }
    if(response[12] == 0x80)
    {
        appMode = true;
    }
    delete response;

    // now send enter prog mode

    msg[0] = 0x30;
    msg[1] = ENTER_PROGRAMMING_MODE_OPCODE;
    msg[2] = 0;
    if (appMode == true)
    {
        printf ("\nentering programming mode -- eject card, and the program will exit. otherwise you will have to wait 30 or so seconds...\n");
    }

    //uc->sendRaw( msg, len);
    if (uc->sendRawNoRx(msg, len))
    {
        if (appMode == false)
        {
            response = uc->getResponse(&readLen);
            delete response;
        }
        else
        {
            mySleep(1);
            uc->resetUC();
        }
    }
    else
    {
        exit(1);
    }

    mySleep(2);
    printf ("program mode should be entered\n");
    delete msg;
}
void eraseSlots(uCryptrInterface *uc)
{
    const unsigned int len = 4;
    unsigned char* msg = new unsigned char[len];
    unsigned int i;
    unsigned char* response = 0;
    unsigned int readLen = 0;
    msg[0] = PAL_CTRL_BYTE;
    msg[1] = ERASE_ALGO_BY_SLOT_OPCODE;
    msg[2] = 0;
    msg[3] = 0;

    for (i = 0; i < 8; i++)
    {
        msg[3] = i & 0xff;
        printf ("erasing slot id: 0x%02X\n", i);
        //uc->sendRaw( msg, len, 3);
        if (uc->sendRawNoRx(msg, len, 3))
        {
            response = uc->getResponse(&readLen);
            delete response;
        }
        else
        {
            exit(1);
        }
        printf ("\ndone erasing slot id: 0x%02X\n", i);
    }
    delete msg;
}
void createEraseAppImage(uCryptrInterface *uc, unsigned char id)
{
    const unsigned int len = 4;
    unsigned char* msg = new unsigned char[len];
    unsigned char* response = 0;
    unsigned int readLen = 0;
    msg[0] = PAL_CTRL_BYTE;
    msg[1] = ERASE_APP_AREA_OPCODE;
    msg[2] = 0;
    msg[3] = id;
    printf ("erasing image id: 0x%02X, this will take several seconds for the write\n", id);
    //uc->sendRaw( msg, len, 7);
    if (uc->sendRawNoRx(msg, len, 7))
    {
        response = uc->getResponse(&readLen);
        delete response;
    }
    else
    {
        exit(1);
    }
    printf ("\ndone erasing image\n");
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

void programBootBlock(uCryptrInterface *uc, MaceBlob *m)
{
    // we need to do the following
    //  - start image
    //  - iterate over data
    //  wait for next image
    //  d/l complete
    int xferLen = 0;
    unsigned char* sendData = 0;
    unsigned int readLen = 0;
    unsigned char* response = 0;

    printf("starting to program boot block\n");

    createEraseAppImage(uc, A9);

    sendData = createStartImage(m->getId(), m->getAlgoId(),  &xferLen);
    //uc->sendRaw( sendData, xferLen);
    if ( uc->sendRawNoRx( sendData, xferLen))
    {
        response = uc->getResponse(&readLen);
        delete response;
    }
    delete sendData;

    //return ;
    const unsigned int reqLen = 160;
    unsigned int len = 64;
    unsigned char* data = 0;
    int bogus = 0;
    while( (len = getNextUpgradeData(m, &data, reqLen)) > 0)
    {
        // unsigned int i = 0;
        // for (i = 0; i < len; i++)
        // {
        //     printf("%02X", data[i]);
        //     if ((i+1) %16 == 0)
        //     {
        //         cout << endl;
        //     }
        // }
        sendData = createDataDownload(data, &len);
        //uc->sendRaw ((unsigned char*)sendData, len);
        if ( uc->sendRawNoRx( sendData, len))
        {
            response = uc->getResponse(&readLen);
            delete response;
        }
        delete data;
        delete sendData;
    }

    len = 0;
    // send wait for next image
    sendData = createWaitForNextImage(&len);
    cout << "sending wait for next image and sleeping 5 seconds to validate the image before d/l complete\n";
    // uc->sendRaw( (unsigned char*)sendData, len, 5);
    if (uc->sendRawNoRx(sendData, len, 5))
    {
        response = uc->getResponse(&readLen);
        delete response;
    }
    else
    {
        exit(1);
    }
    delete sendData;

    cout << "\nsending d/l complte\n";
    sendData = createDownLoadComplete(&len);
    //uc->sendRaw( (unsigned char*)sendData, len);
    if (uc->sendRawNoRx(sendData, len))
    {
        response = uc->getResponse(&readLen);
        delete response;
    }
    else
    {
        exit(1);
    }
    delete sendData;

    cout << "\nresetting, should take several seconds to get up and running:\n";
    uc->resetUC();
    mySleep (1);
    //uc->resetUC();
    //while (true)
    //{
    //    cout << "now you need to eject the card and re-insert twice:\n";
    //    cout << "have you done that?:  [y/n] ";
    //    char a;
    //    cin >> a;
    //    if (a == 'y' || a == 'Y')
    //    {
    //        break;
    //    }
    //}
    printf("done programming boot block\n");

}

void programARM9(uCryptrInterface *uc, MaceBlob *m)
{
    // we need to do the following
    //  - start image
    //  - iterate over data
    //  wait for next image
    //  d/l complete
    int xferLen = 0;
    unsigned char* sendData = 0;
    unsigned char* response = 0;
    unsigned int readLen = 0;


    printf("starting to program image id: 0x%02X\n", m->getId());
    createEraseAppImage(uc, m->getId());

    sendData = createStartImage(m->getId(), m->getAlgoId(),  &xferLen);
    // uc->sendRaw( sendData, xferLen);
    if (uc->sendRawNoRx(sendData, xferLen))
    {
        response = uc->getResponse(&readLen);
        delete response;
    }
    else
    {
        exit(1);
    }
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
        //uc->sendRaw ((unsigned char*)sendData, len);
        if (uc->sendRawNoRx(sendData, len))
        {
            response = uc->getResponse(&readLen);
            delete response;
        }
        else
        {
            exit(1);
        }
        delete data;
        delete sendData;
    }

    len = 0;
    // send wait for next image
    sendData = createWaitForNextImage(&len);
    //uc->sendRaw( (unsigned char*)sendData, len);
    if (uc->sendRawNoRx(sendData, len))
    {
        response = uc->getResponse(&readLen);
        delete response;
    }
    else
    {
        exit(1);
    }
    delete sendData;
    printf("\ndone programming app id: 0x%02X\n", m->getId());
}

void programAlgos(uCryptrInterface *uc, MaceBlob *m, bool algoErased)
{
    // we need to do the following
    //  - start image
    //  - iterate over data
    //  wait for next image
    //  d/l complete
    int xferLen = 0;
    unsigned char* sendData = 0;
    unsigned char* response = 0;
    unsigned int readLen = 0;


    printf("starting to program algo id: 0x%02X\n", m->getAlgoId());
    if (algoErased == false)
    {
        eraseSlots(uc);
    }

    sendData = createStartImage(m->getId(), m->getAlgoId(),  &xferLen);
    //uc->sendRaw( sendData, xferLen);
    if (uc->sendRawNoRx(sendData, xferLen))
    {
        response = uc->getResponse(&readLen);
        delete response;
    }
    else
    {
        exit(1);
    }
    delete sendData;

    //return ;
    const unsigned int reqLen = 64;
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
        // uc->sendRaw ((unsigned char*)sendData, len);
        if (uc->sendRawNoRx(sendData, len))
        {
            response = uc->getResponse(&readLen);
            delete response;
        }
        else
        {
            exit(1);
        }
        delete data;
        delete sendData;
    }

    len = 0;
    // send wait for next image
    sendData = createWaitForNextImage(&len);
    // uc->sendRaw( (unsigned char*)sendData, len);
    if (uc->sendRawNoRx(sendData, len))
    {
        response = uc->getResponse(&readLen);
        delete response;
    }
    else
    {
        exit(1);
    }
    delete sendData;

    printf("\ndone programming algo id: 0x%02X\n", m->getAlgoId());

}

void mySleep(int seconds)
{
#ifdef WIN32
	Sleep(seconds * 1000);
#else
	sleep(seconds);
#endif
}
