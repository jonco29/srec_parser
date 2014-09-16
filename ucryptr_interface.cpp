#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include "ucryptr_interface.h"

using namespace std;

uCryptrInterface::uCryptrInterface()
    :readData(0)
{
}

bool uCryptrInterface::isReady()
{
    bool ready = false;         // DEV_UCRYPTR exists in the sys filesystem
    bool return_status = false;
    unsigned char failCounter   = 0;   // allow POWERUP_TIMEOUT attemtps before returning with a failure
    int ucryptr_ready   = -1;


    while (ready == false) {

        if(-1 == (ucryptr_ready = access(DEV_UCRYPTR, W_OK) ) ) 
        {

            if(++failCounter < POWERUP_TIMEOUT) 
            {
                printf("exists_ucryptr_dev() -- Device is not available yet: %s \n", strerror(errno));
                sleep(FILE_ACCESS_RETRY_DELAY);
            } 
            else 
            {
                printf("exists_ucryptr_dev() -- Device unavailable after %d seconds - returning %s\n",
                        failCounter, strerror(errno));
                ready = true;
            }
            return_status = false;

        } 
        else 
        {
            printf("exists_ucryptr_dev - %s initialized\n", DEV_UCRYPTR);
            return_status = true;
            ready = true;
        }
    }

    return return_status;
}

// typedef struct PACKED
// {
//     unsigned char   opcode;
//     unsigned char   type;
//     unsigned char   length[2];
//     unsigned char   index[8];
//     unsigned char   data[1];
// } UCRYPTR_PAYLOAD_t;

UCRYPTR_PAYLOAD_t *uCryptrInterface::formatData(unsigned char* data, unsigned short len, unsigned char opcode)
{
    UCRYPTR_PAYLOAD_t* payload = (UCRYPTR_PAYLOAD_t*)new unsigned char[len + sizeof(UCRYPTR_PAYLOAD_t)];
    char asciiLen[2] = {0};


    hex2ascii((unsigned char)len/2 + 5, asciiLen);
    
    payload->opcode = opcode;
    payload->type = '3';
    //payload->length = (len << 8 | len >> 8);
    //payload->length = len;
    payload->length[0] = asciiLen[0];
    payload->length[1] = asciiLen[1];
    memcpy(payload->data, data, len);

    return payload;
}

bool uCryptrInterface::sendMACEboot()
{
    unsigned char data[] = {'M', 'A', 'C', 'E', 'b', 'o', 'o', 't', '\r'};
    unsigned int len = 9;
    return sendRaw(data,len);
}
bool uCryptrInterface::sendMACEDownloadComplete()
{
    unsigned char data[] = {MACE_DOWNLOAD_COMPLETE, '\r'};
    unsigned int len = 2;
    return sendRaw(data,len);
}

bool uCryptrInterface::send(UCRYPTR_PAYLOAD_t* data)
{
    bool status = false;
    unsigned char len = 0;

    if (data != 0)
    {
        len = ascii2hex(data->length);
        len -= 5;
        len *= 2;
        //len = data->length;
        //len = (len << 8|len >> 8);
        //len &= 0xffff;
        len += sizeof(UCRYPTR_PAYLOAD_t) -1;
        //len = data->length + sizeof(UCRYPTR_PAYLOAD_t) -1;
        status = sendRaw((unsigned char* )data, len);
    }
    return status;
}

bool uCryptrInterface::sendRaw(unsigned char* data, unsigned int len)
{
    int fd  = 0;
    int ret = 0;
    int i = 0;
    bool status = false;
    unsigned char* outData = data;
    // unsigned char *outData = new unsigned char[len + 1];
    // memcpy(outData, data, len);
    // outData[len] = '0';
    // outData[len+1] = '\r';
    // len++;
    // len++;

    cleanupReadData();

    if (data != 0)
    {

        if(0 > (fd = open(DEV_UCRYPTR, O_RDWR))) 
        {
            perror("request_handler() - Failed to open /dev/ucryptr0");
            ret = false;
        } 
        else 
        {
            //display_response((unsigned char*)message); // display the message to send

            if(0 > (ret = write(fd, outData, len)))
            {
                perror("request_handler() - Failure to write to /dev/ucryptr0");

                if(-1 == (ret = close(fd)))
                {
                    perror("request_handler() - Failure to close /dev/sd_cryptr0");
                }

            } 
            else 
            {
                printf("request_handler() - Module Info Request written to %s\n", DEV_UCRYPTR);
                i++;
                status = true;

                if(-1 == (ret = close(fd)))
                {
                    perror("request_handler() - Failure to close /dev/sd_cryptr0");
                }

                status = rxData();

            }
        }
    }
    //delete outData;

    return status;
}
void uCryptrInterface::cleanupReadData()
{
    if (readData != 0)
    {
        delete readData;
        readData = 0;
    }
}

bool uCryptrInterface::rxData()
{
    int rc;
    bool status = false;
    cleanupReadData();
    readData = new unsigned char[UC_READ_LEN];
    unsigned int len = 0;
    int fd = -1;



    //while(0 != rsp_loop--) 
    {

        if(0 > (fd = open(DEV_UCRYPTR, O_RDWR))) {

            perror("response_handler - Failed to open /dev/ucryptr0");
            rc = fd;
        } 
        else 
        {
            if(0 < (rc = read(fd, readData, UC_READ_LEN))) 
            {
                //display_response((unsigned  char*)output_buffer);
                status = true;
            } 
            else 
            {
                perror("Failure to read from /dev/ucryptr0");
            }

            if(-1 == (rc = close(fd)))
            {
                perror("response_handler() - Failure to close /dev/sd_cryptr0");
            }
        }

    }

    return status;
}

void uCryptrInterface::hex2ascii(unsigned char val, char* data)
{
    int hi = val/16;
    int low = val%16;
    data[0] = asciiVal(hi);
    data[1] = asciiVal(low);
}
char uCryptrInterface::asciiVal(unsigned char a)
{
    if (a >= 0 && a <= 9)
    {
        a += '0';
    }
    else if (a >= 0x0a && a <= 0x0f)
    {
        a = a - 0x0a;
        a+= 'A';
    }
    else
    {
        a = 0;
    }
    return (char)a;
}
unsigned char uCryptrInterface::ascii2hex(unsigned char* data)
{
    int shiftVal = 4;
    const int shift = 4;
    int i;
    int val = 0;
    int tmp;
    for (i = 0; i < 2; i++)
    {
        tmp = 0;
        tmp = hexVal(data[i]);
        tmp <<= shiftVal;
        val |= tmp;
        shiftVal -= shift;
    }
    return (unsigned char)val;
}
unsigned char uCryptrInterface::hexVal(char a)
{
    int val = 0;
    if (a >= '0' && a <= '9')
    {
        val = a - '0';
    }
    else if (a >= 'A' && a <= 'F')
    {
        val = a - 'A';
        val += 0xa;
    }
    else
    {
        val = 0;
    }
    return val;
}
