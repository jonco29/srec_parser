

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
    unsigned char   length[2];
    unsigned char   index[8];
    unsigned char   data[1];
} UCRYPTR_PAYLOAD_t;


#define DEV_UCRYPTR "/dev/cryptr_micro0"
#define POWERUP_TIMEOUT 30
#define FILE_ACCESS_RETRY_DELAY 1
#define SLEEP_FOR 1
#define UC_READ_LEN 1024



class uCryptrInterface 
{
    public:
        uCryptrInterface();
        ~uCryptrInterface() {};

        bool isReady();
        UCRYPTR_PAYLOAD_t *formatData(unsigned char* data, unsigned short len, unsigned char opcode);
        bool sendMACEboot();
        bool send(UCRYPTR_PAYLOAD_t* data);
        unsigned char* getResponse();
    private:
        bool sendRaw(unsigned char* data, unsigned int len);
        unsigned char* readData;
        bool rxData();
        void cleanupReadData();
        void hex2ascii(unsigned char val, char* data);
        char asciiVal(unsigned char val);
        unsigned char ascii2hex(unsigned char* data);
        unsigned char hexVal(char val);

};



#endif // UCRYPTR_INTERFACE_H

