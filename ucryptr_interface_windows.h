#include <Windows.h>
#include "ucryptr_interface.h"
#include "spgc_types.h"
#include "crypto_transport_service.h"

#ifndef UCRYPTR_INTERFACE_WINDOWS_H
#define UCRYPTR_INTERFACE_WINDOWS_H

#ifndef _MSC_VER
#define PACKED __attribute__ ((__packed__))
#else
#define PACKED
#pragma pack(push, 1)
#endif


typedef struct PACKED
{
    UINT16 fid;
    UINT16 opcode;
    UINT32 tag_id;
    UINT8  message_flags;
    UINT8 reserved;
    UINT16 payload_size;
} SDIO_COMMON_MSG;

#ifdef _MSC_VER
#pragma pack(pop)
#endif

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
        void setCallBackReceiver(CTRANS_RECEIVER_INFO_T  cb);
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
        UINT32 last_tag_id;

        SPGC_LIB_CONFIG_PARAM_T * configParametersPtr ;
        UINT32 configParameterCount;
        CTRANS_RECEIVER_INFO_T callbackReceiver;



        // callbacks for windows driver
        static VOID messageReceivedCallbackPrint(const  UINT8 * buffer, UINT32 bufferLength);
        static VOID linkEventOccurredCallbackPrint(UINT32 eventType, UINT8* eventData, UINT32 eventDataLength);
        static VOID loggingCallbackPrint(UINT32 errorCode, TCHAR *location, UINT8 loggingLevel, VOID * m1, VOID * m2, VOID * m3);
        static HANDLE gMutex;
};

static CTRANS_RECEIVER_INFO_T callbackReceiver;

#endif // UCRYPTR_INTERFACE__WINDOWS_H

