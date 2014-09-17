

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


#define DEV_UCRYPTR "/dev/cryptr_micro0"
#define POWERUP_TIMEOUT 30
#define FILE_ACCESS_RETRY_DELAY 1
#define SLEEP_FOR 1
#define UC_READ_LEN 1024

#define MACE_DOWNLOAD_COMPLETE  0x59



class uCryptrInterface 
{
    public:
        uCryptrInterface();
        ~uCryptrInterface() {};

        bool isReady();
        char *formatData(unsigned char* data, int *len, unsigned char opcode = 'S');
        bool sendMACEboot();
        bool sendMACEDownloadComplete();
        bool send(UCRYPTR_PAYLOAD_t* data);
        unsigned char* getResponse();
        bool sendRaw(unsigned char* data, unsigned int len);
    private:
        unsigned char* readData;
        bool rxData();
        void cleanupReadData();
        char asciiVal(unsigned char val);

};



#endif // UCRYPTR_INTERFACE_H

