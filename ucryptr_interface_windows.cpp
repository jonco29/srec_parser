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

uCryptrInterfaceWindows::uCryptrInterfaceWindows()
    :linkUp(false),
     readData(0),
     readLen(0)
{
    configParametersPtr = NULL;
    configParameterCount = 0;

    // create message rx semaphore
    rxSemaphore = CreateSemaphore(NULL, 0, 10, NULL);

    // setup callbacks
    callbackReceiver.messageReceivedCallback = messageReceivedCallbackPrint;
    callbackReceiver.linkEventOccurredCallback = linkEventOccurredCallbackPrint;
    callbackReceiver.loggingCallback = loggingCallbackPrint;

    // setup the transport
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
    memcpy(sendData, data, len);
    if (len < 64)
    {
        len = 64;
    }
    CTransSend(sendData, len);
    Sleep(500);
    return true;
}
void uCryptrInterfaceWindows::resetUC()
{
    char tmp;
    cout << "eject cryptr, have you done that y/n?";
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

        int i;
        printf ("Read response: ");
        for (i = 0; i < readLen; i++)
        {
            printf("%02X", readData[i]);
            if (i %16 == 15)
            {
                printf("\n");
            }
        }
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
}

VOID uCryptrInterfaceWindows::linkEventOccurredCallbackPrint(UINT32 eventType, UINT8* eventData, UINT32 eventDataLength)
{
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

    return;
}
VOID uCryptrInterfaceWindows::loggingCallbackPrint(UINT32 errorCode, TCHAR *location, UINT8 loggingLevel, VOID * m1, VOID * m2, VOID * m3)
{
}
