#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ucryptr_interface.h"

using namespace std;


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

