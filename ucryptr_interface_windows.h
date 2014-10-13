#include <Windows.h>
#include "ucryptr_interface.h"
#include "spgc_types.h"
#include "crypto_transport_service.h"

#ifndef UCRYPTR_INTERFACE_WINDOWS_H
#define UCRYPTR_INTERFACE_WINDOWS_H


class uCryptrInterfaceWindows : public uCryptrInterface 
{
    public:
        static uCryptrInterfaceWindows * getCryptrInterface();
        virtual ~uCryptrInterfaceWindows() {};

        virtual bool isReady();
        virtual bool sendRaw(unsigned char* data, unsigned int len, int sleepVal=0);
        virtual bool sendRawNoRx(unsigned char* data, unsigned int len, int sleepVal=0);
        virtual void resetUC();
        virtual unsigned char* getResponse(unsigned int *len);
    protected:
        virtual bool rxData();

    private:
        bool linkUp;
        uCryptrInterfaceWindows();
        static uCryptrInterfaceWindows *theInterface;
        unsigned char* readData;
        unsigned char sendData[4000];
        int readLen;
        void cleanupReadData();
        void notifyRx();
        HANDLE rxSemaphore;

        SPGC_LIB_CONFIG_PARAM_T * configParametersPtr ;
        UINT32 configParameterCount;



        // callbacks for windows driver
        static VOID messageReceivedCallbackPrint(const  UINT8 * buffer, UINT32 bufferLength);
        static VOID linkEventOccurredCallbackPrint(UINT32 eventType, UINT8* eventData, UINT32 eventDataLength);
        static VOID loggingCallbackPrint(UINT32 errorCode, TCHAR *location, UINT8 loggingLevel, VOID * m1, VOID * m2, VOID * m3);
};

static CTRANS_RECEIVER_INFO_T callbackReceiver;

#endif // UCRYPTR_INTERFACE__WINDOWS_H

