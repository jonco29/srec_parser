#include <iostream>
#include <string>
#include <stdio.h>
#include <string.h>
#include "ucryptr_interface.h"
#include "srec2mem.h"
#include "unistd.h"


using namespace std;

bool doFlashLoader(char* arg, uCryptrInterface &uc);
bool doBootBlock(char* arg, uCryptrInterface &uc);
int main (int argc, char** argv)
{
    FILE *fstream;
    uCryptrInterface uc;
    //if (argc != 3)
    if (argc != 2)
    {
        cout << "usage: " << argv[0] << " <flashloader.srec> <bootblock.srec>" << endl;
        return 1;
    }

    // if (( fstream = fopen (argv[1], "rt")) == NULL)
    // {
    //     cout << "usage: " << argv[0] << " <flashloader.srec> <bootblock.srec>, but file: " << argv[1] << " cannot be opened" << endl;
    //     return 1;
    // }
    // char line[400] = {0xaa};

    // int i = 0;

    if (uc.isReady())
    {
          cout << "uCryptR is ready........." << endl;
        if (uc.sendMACEboot() == true)
        {
            cout << "MACEboot sent........." << endl;
        }
    }
    else
    {
        cout << "uCryptR is NOT ready........." << endl;
        return 1;
    }


    doFlashLoader(argv[1], uc);
    //while (fgets( line, sizeof(line), fstream) != NULL)
    //{
    //    cout << "line: " << i++ << " (" << strlen(line) << ")\t" << line ;
    //    uc.sendRaw((unsigned char*)line, strlen(line));
    //}
    //fclose (fstream);

    uc.sendMACEDownloadComplete();


    // now do the boot block
    doBootBlock(argv[2], uc);
}

bool doFlashLoader(char* arg, uCryptrInterface &uc)
{
    SRecord2Mem srecMem(arg);
    unsigned char *data = 0;
    unsigned char *outData = 0;
    int len = 0;
    const int reqLen = 16;


    len = srecMem.getNextData(&data, reqLen);
    while (len != 0)
    {
        int i;
        char* asdf = uc.formatData(data, &len);

        for (i = 0; i < len; i++)
        {
            printf("%c", asdf[i]);
        }
        std::cout << std::endl;

        // outData = new unsigned char[len + 4];
        // outData[0] = 0x00;
        // outData[1] = len+1;
        // memcpy(&outData[2], data, len);
        // outData[len+2] = 0;
        // outData[len+3] = '\r';
        uc.sendRaw((unsigned char*)asdf, len);

        delete outData;
        outData = 0;


        delete data;
        data = 0;
        len = srecMem.getNextData(&data, reqLen);
    }
}

bool doBootBlock(char* arg, uCryptrInterface &uc)
{
    SRecord2Mem srecMem(arg);
    unsigned char *data = 0;
    unsigned char *outData = 0;
    int len = 0;
    const int reqLen = 96;

    // first we need to send an empty boot block message
    // cout << "sleeping 3 seconds" << endl;
    // sleep(3);
    // cout << "done sleeping " << endl;
    unsigned char startBB[] = {0x11,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'\r'};
    uc.sendRaw((unsigned char*)startBB, 16);



    len = srecMem.getNextData(&data, reqLen);
    while (len != 0)
    {
        int i;
        for (i = 0; i < len; i++)
        {
            //std::cout << std::hex << data[i] ;
            printf("%02X", data[i]);
        }
        std::cout << std::endl;

        outData = new unsigned char[len + 4];
        outData[0] = 0x00;
        outData[1] = len+1;
        memcpy(&outData[2], data, len);
        outData[len+2] = 0;
        outData[len+3] = '\r';
        uc.sendRaw((unsigned char*)outData, len+4);

        delete outData;
        outData = 0;


        delete data;
        data = 0;
        len = srecMem.getNextData(&data, reqLen);
    }
}
