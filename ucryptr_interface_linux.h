#include "ucryptr_interface.h"

#ifndef UCRYPTR_INTERFACE_LINUX_H
#define UCRYPTR_INTERFACE_LINUX_H


#define DEV_UCRYPTR "/dev/cryptr_micro0"
#define POWERUP_TIMEOUT 30
#define FILE_ACCESS_RETRY_DELAY 1
#define SLEEP_FOR 1
#define UC_READ_LEN 1024
#define MMCHWRESET 0x5801

class uCryptrInterfaceLinux : public uCryptrInterface 
{
    public:
        uCryptrInterfaceLinux(bool isBlank=false);
        virtual ~uCryptrInterfaceLinux() {};

        virtual bool isReady();
        virtual bool sendRaw(unsigned char* data, unsigned int len, int sleepVal=0);
        virtual bool sendRawNoRx(unsigned char* data, unsigned int len, int sleepVal=0);
        virtual unsigned char* getResponse(unsigned int *len);
        virtual void resetUC();
    protected:
        virtual bool rxData();
        void cleanupReadData();
        bool blank;

    private:
        unsigned char* readData;
        unsigned char sendData[4000];
        ssize_t readLen;

};


#endif // UCRYPTR_INTERFACE_H

