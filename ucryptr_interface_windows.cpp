#include <Windows.h>
#include "spgc_types.h"
#include "crypto_transport_service.h"
#include "ucryptr_interface_windows.h"
#include <time.h>
#include <stdio.h>
#include <iostream>
#include <Events.h>


using namespace std;

uCryptrInterfaceWindows* uCryptrInterfaceWindows::theInterface = NULL;
HANDLE uCryptrInterfaceWindows::gMutex;

uCryptrInterfaceWindows::uCryptrInterfaceWindows()
    :linkUp(false),
     readData(0),
     readLen(0),
     last_tag_id(1)
{
    configParametersPtr = NULL;
    configParameterCount = 0;

    // create message rx semaphore
    rxSemaphore = CreateSemaphore(NULL, 0, 10, NULL);

    // setup callbacks
    // callbackReceiver.messageReceivedCallback = messageReceivedCallbackPrint;
    // callbackReceiver.linkEventOccurredCallback = linkEventOccurredCallbackPrint;
    // callbackReceiver.loggingCallback = loggingCallbackPrint;

    // // setup the transport
    // CTransInitialize(configParameterCount, configParametersPtr, callbackReceiver);
}

void uCryptrInterfaceWindows::setCallBackReceiver(CTRANS_RECEIVER_INFO_T  cb)
{
    gMutex = CreateMutex(NULL, FALSE, NULL);
    if (gMutex == NULL)
    {
        cout << "cannot create mutex, exiting\n";
        exit(1);
    }

    callbackReceiver = cb;
    // setup the transport
    callbackReceiver.messageReceivedCallback = messageReceivedCallbackPrint;
    callbackReceiver.linkEventOccurredCallback = linkEventOccurredCallbackPrint;
    callbackReceiver.loggingCallback = loggingCallbackPrint;
    CTransInitialize(configParameterCount, configParametersPtr, callbackReceiver);

}

uCryptrInterfaceWindows* uCryptrInterfaceWindows::getCryptrInterface()
{
    if (theInterface == NULL)
    {
        theInterface = new uCryptrInterfaceWindows();
    }
    return theInterface;
}

bool uCryptrInterfaceWindows::isReady()
{
    return linkUp;
}
bool uCryptrInterfaceWindows::sendRaw(unsigned char* data, unsigned int len, int sleepVal)
{
    bool status = false;
    status = sendRawNoRx(data, len, sleepVal);

    // if (status)
    // {
    //     status = rxData();
    // }
    return status;
}
bool uCryptrInterfaceWindows::sendRawNoRx(unsigned char* data, unsigned int len, int sleepVal)
{
    bool retVal = false;
    SDIO_COMMON_MSG *fid_hdr = (SDIO_COMMON_MSG*)sendData;
    memset(sendData, 0, sizeof(SDIO_COMMON_MSG));
    fid_hdr->tag_id = last_tag_id;
    last_tag_id++;
    unsigned char* txData2 = (unsigned char*)(fid_hdr + 1);
    memcpy(txData2, data, len);
    fid_hdr->payload_size = len;
    len += sizeof(SDIO_COMMON_MSG);

    if (len < 32)
    {
        len = 32;
    }
    //current_msg_ptr = sendData;
    if (CTransSend(sendData, len) == SPGC_ERR_SUCCESS)
    {
        retVal = true;
    }
    return retVal;
}
// {
//     memcpy(sendData, data, len);
//     if (len < 64)
//     {
//         len = 64;
//     }
//     CTransSend(sendData, len);
//     Sleep(500);
//     return true;
// }
void uCryptrInterfaceWindows::resetUC()
{
    char tmp;
    cout << "\neject cryptr, have you done that y/n?";
    cin >> tmp;
}
bool uCryptrInterfaceWindows::rxData()
{
    return false;
}
unsigned char* uCryptrInterfaceWindows::getResponse(unsigned int *len)
{
    DWORD semWaitResult;
    unsigned char* data = 0;

    // give us 20 seconds to respond
    semWaitResult = WaitForSingleObject( rxSemaphore, 20000);

    if (readLen > 0)
    {
        *len = readLen;
        data = new unsigned char [readLen];
        memcpy (data, readData, readLen);
    }
    return data;
}

void uCryptrInterfaceWindows::cleanupReadData()
{
    if (readData != 0)
    {
        delete readData;
        readData = 0;
        readLen = 0;
    }
}
void uCryptrInterfaceWindows::notifyRx()
{
}


VOID uCryptrInterfaceWindows::messageReceivedCallbackPrint(const  UINT8 * buffer, UINT32 bufferLength)
{
    DWORD dwWaitResult;
    dwWaitResult = WaitForSingleObject(gMutex, INFINITE);

    if (theInterface == NULL)
    {
        getCryptrInterface();
    }
    theInterface->cleanupReadData();
    theInterface->readData = new unsigned char[bufferLength];
    memcpy(theInterface->readData, buffer, bufferLength);
    theInterface->readLen = bufferLength;
    theInterface->notifyRx();
    if (!ReleaseSemaphore( theInterface->rxSemaphore, 1, NULL) )
    {
        printf("ReleaseSemaphore Error: %d\n", GetLastError());
    }
    ReleaseMutex(gMutex);
}

VOID uCryptrInterfaceWindows::linkEventOccurredCallbackPrint(UINT32 eventType, UINT8* eventData, UINT32 eventDataLength)
{
    DWORD dwWaitResult;
    dwWaitResult = WaitForSingleObject(gMutex, INFINITE);

    if (theInterface == NULL)
    {
        getCryptrInterface();
    }
	if (eventType == EXTERNAL_EVENT_ID_LINK_UP)
	{
		//printf_s("JONATHAN ------------  linkEventOccurredCallbackPrint ::::::::::::  LINK IS UP!!!!!!!!!          Callback\n");
        theInterface->linkUp = true;
	}
	else if (eventType == EXTERNAL_EVENT_ID_LINK_DOWN)
	{
		//printf_s("JONATHAN ------------  linkEventOccurredCallbackPrint ::::::::::::  LINK IS DOWN :'(   wahhhh!!! Callback\n");
        theInterface->linkUp = false;
	}
    ReleaseMutex(gMutex);

    return;
}
VOID uCryptrInterfaceWindows::loggingCallbackPrint(UINT32 errorCode, TCHAR *location, UINT8 loggingLevel, VOID * m1, VOID * m2, VOID * m3)
{
    DWORD dwWaitResult;
    dwWaitResult = WaitForSingleObject(gMutex, INFINITE);
    int i = 0;

    switch(errorCode)
    {
        case SPGC_LOG_CODE_TRANSPORT_EVENT:
        {
            break;
        }
        case SPGC_LOG_CODE_TRANSPORT_DATA_OUT:
        {
            printf(".");
			// printf_s("JONATHAN ------------  loggingCallbackPrint ::::::::::::  Callback\n" );
            // printf_s(("%s traced buffer of length %x [out] "), location, (int) m2);

            // for(i=0; (i < 20 &&(i < (int) m2)); i++)
            // {
            //     printf_s((" %02X"), ((UINT8 *)m1)[i]);
            // }
            // printf_s("\n");
            break;
        }
        case SPGC_LOG_CODE_TRANSPORT_DATA_IN:
        {
			// printf_s("JONATHAN ------------  loggingCallbackPrint ::::::::::::  Callback\n" );
            // printf_s(("%s traced buffer of length %x [in] "), location, (int) m2);

            // for(i=0; (i < (int) m2); i++)
            // {
            //     printf_s((" %02X"), ((UINT8 *)m1)[i]);
            // }
            // printf_s("\n");
            break;
        }
        default:
        break;
    }
    ReleaseMutex(gMutex);
    return ;
}
