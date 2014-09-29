#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include "ucryptr_interface.h"

using namespace std;

uCryptrInterface::uCryptrInterface(bool isBlank)
    :readData(0),
    blank(isBlank),
    readLen(0)
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

char *uCryptrInterface::formatData (unsigned char* data, int *len, unsigned char opcode )
{
    int payloadSize = *len + sizeof(UCRYPTR_PAYLOAD_t);
    UCRYPTR_PAYLOAD_t* payload = (UCRYPTR_PAYLOAD_t*)new unsigned char[payloadSize];
    char *asciiPayload = new char[payloadSize * 2];
    unsigned char* payloadPtr = (unsigned char*)payload;
    unsigned char* ptr = (unsigned char*)payload;
    int i = 0;
    int j = 0;
    
    // first setup a ucryptr_payload in hex values, then we'll fluff it up to ascii
    payload->opcode = 'S';
    payload->type = 3;
    payload->length = *len + 4 + 1;                      // data len + addr len + crc len
    memset(payload->addr, 0, 4);
    memcpy(payload->data, data, *len);
    payload->data[*len] = 0;


    // put in the S3 first
    asciiPayload[i] = ptr[i];
    i++;
    asciiPayload[i] = '3';

    // now do the rest of the payload
    for (i = 2, j = 2; i < payloadSize; i++)
    {
        int hi = ptr[i] / 16;
        int low = ptr[i] % 16;
        asciiPayload[j++] = asciiVal(hi);
        asciiPayload[j++] = asciiVal(low);
    }
    asciiPayload[j++] = '\r';
    asciiPayload[j++] = '\n';
    *len = j;

    return asciiPayload;
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
    return status;
}

bool uCryptrInterface::sendRaw (unsigned char* data, unsigned int len, int sleepVal)
{
    bool status = false;
    status = sendRawNoRx(data, len, sleepVal);
    if (status)
    {
        status = rxData();
    }
    return status;
}
bool uCryptrInterface::sendRawNoRx(unsigned char* data, unsigned int len, int sleepVal)
{
    int fd  = 0;
    int ret = 0;
    int i = 0;
    bool status = false;
    unsigned char* outData = data;
    memcpy(sendData, data, len);
    outData = sendData;

    // if we are blank, we don't want to pad
    // but if we are blank, we need to pad
    if (blank == false)
    {
        if (len < 64)
        {
            len = 64;
        }
    }

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
                //printf("request_handler() - Data written to %s\n", DEV_UCRYPTR);
                i++;
                status = true;

                if(-1 == (ret = close(fd)))
                {
                    perror("request_handler() - Failure to close /dev/sd_cryptr0");
                }

                if (sleepVal)
                {
                    sleep(sleepVal);
                }
            }
        }
    }

    return status;
}
void uCryptrInterface::cleanupReadData()
{
    if (readData != 0)
    {
        delete readData;
        readData = 0;
        readLen = 0;
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


    if( 0 > (fd = open(DEV_UCRYPTR, O_RDWR))) 
    {
        perror("response_handler - Failed to open /dev/ucryptr0");
        rc = fd;
    } 
    else 
    {
        if(0 < (rc = read(fd, readData, UC_READ_LEN))) 
        {
            status = true;
            readLen = rc;
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

    return status;
}
unsigned char* uCryptrInterface::getResponse(unsigned int *len)
{
    unsigned char* data = 0;

    if (readLen <= 0)
    {
        if (rxData() != true)
        {
            exit(1);
        }
    }
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

void uCryptrInterface::resetUC()
{
    int fd = -1;
    int status = 0;


    if( 0 > (fd = open(DEV_UCRYPTR, O_RDWR))) 
    {
        perror("response_handler - Failed to open /dev/ucryptr0");
    } 
    else 
    {
        printf("resetting cryptr....\n");
        status = ioctl(fd, MMCHWRESET, 0);
        sleep(2);
        printf("reset of cryptr is complete.  the status is: 0x%02X\n", status);
    }
}
