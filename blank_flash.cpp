#include <iostream>
#include <string>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "ucryptr_interface_linux.h"
#include "combinedSrec2mem.h"
#include "MaceBlob.h"


using namespace std;

bool doFlashLoader(uCryptrInterface &uc, CombinedSRecord2Mem* srec);
bool doBootBlock(uCryptrInterface &uc, CombinedSRecord2Mem* srec);

int main (int argc, char** argv)
{
    FILE *fstream;
    uCryptrInterfaceLinux uc(true);
    CombinedSRecord2Mem *srec = 0;

    if (argc != 2)
    {
        cout << "usage: " << argv[0] << " <combined_srec>" << endl;
        return 1;
    }

    // read in the srec
    srec = new CombinedSRecord2Mem(argv[1]);


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


    doFlashLoader(uc, srec);


    // now do the boot block
    doBootBlock(uc, srec);

    delete srec;
    cout <<"all done....\n";
}

bool doFlashLoader(uCryptrInterface &uc, CombinedSRecord2Mem* srec)
{
    bool stop = false;
    bool retVal = false;
    MaceBlob *m;

    while ( (stop == false) && ((m = srec->getNextImage()) != 0))
    {
        cout <<"================================================================================\n";
        if (m->getId() == 5)
        {
            stop = true;

            // loop till we get to the end of the data
            while (m->getNextDataLen() != 0)
            {
                unsigned int len = 0;
                unsigned char* data = 0;
                len = m->getNextDataLen();
                data = m->getNextData();
                int i;
                for (i = 0; i < len; i++)
                {
                    printf("%02X", data[i]);
                }
                cout << endl;
                char* sendData = uc.formatData(data, (int *)&len);
                uc.sendRaw( (unsigned char*)sendData, len);
                delete data;
                delete sendData;
            }
            retVal = true;
        }
        delete m;
    }
    uc.sendMACEDownloadComplete();
    return retVal;

}

bool doBootBlock(uCryptrInterface &uc, CombinedSRecord2Mem* srec)
{
    bool stop = false;
    bool retVal = false;
    unsigned char *outData = 0;
    MaceBlob *m;

    while ( (stop == false) && ((m = srec->getNextImage()) != 0))
    {
        cout <<"================================================================================\n";
        if (m->getId() == 6)
        {
            stop = true;

            // send start message
            unsigned char startBB[] = {0x11,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'\r'};
            uc.sendRaw((unsigned char*)startBB, 16);

            // loop till we get to the end of the data
            while (m->getNextDataLen() != 0)
            {
                unsigned int len = 0;
                unsigned char* data = 0;
                len = m->getNextDataLen();
                data = m->getNextData();
                int i;
                for (i = 0; i < len; i++)
                {
                    printf("%02X", data[i]);
                }
                cout << endl;


                outData = new unsigned char[len + 4];
                outData[0] = 0x00;
                outData[1] = len+1;
                memcpy(&outData[2], data, len);
                outData[len+2] = 0;
                outData[len+3] = '\r';
                uc.sendRaw((unsigned char*)outData, len+4);

                //char* sendData = uc.formatData(data, (int *)&len);
                delete data;
                delete outData;
            }
            retVal = true;
        }
        delete m;
    }
}
