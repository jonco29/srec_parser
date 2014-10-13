

#ifndef UCRYPTR_INTERFACE_H
#define UCRYPTR_INTERFACE_H

#ifndef _MSC_VER
#define PACKED __attribute__ ((__packed__))
#else
#define PACKED
#pragma pack(push, 1)
#endif


typedef struct PACKED
{
    unsigned char   opcode;
    unsigned char   type;
    unsigned char   length;
    unsigned char   addr[4];
    unsigned char   data[1];
} UCRYPTR_PAYLOAD_t;

#ifdef _MSC_VER
#pragma pack(pop)
#endif

// #define DEV_UCRYPTR "/dev/cryptr_micro0"
// #define POWERUP_TIMEOUT 30
// #define FILE_ACCESS_RETRY_DELAY 1
// #define SLEEP_FOR 1
// #define UC_READ_LEN 1024
// 
#define MACE_DOWNLOAD_COMPLETE  0x59

// #define MMCHWRESET 0x5801



class uCryptrInterface 
{
    public:
        uCryptrInterface() {};
        virtual ~uCryptrInterface() {};

        virtual bool isReady() = 0;
        virtual bool sendRaw(unsigned char* data, unsigned int len, int sleepVal=0) = 0;
        virtual bool sendRawNoRx(unsigned char* data, unsigned int len, int sleepVal=0) = 0;
        virtual void resetUC() = 0;
        virtual unsigned char* getResponse(unsigned int *len) = 0;

        char *formatData(unsigned char* data, int *len, unsigned char opcode = 'S');
        bool sendMACEboot();
        bool sendMACEDownloadComplete();

        // not used or implemented
        bool send(UCRYPTR_PAYLOAD_t* data);
    private:
        char asciiVal(unsigned char val);
};



#endif // UCRYPTR_INTERFACE_H

