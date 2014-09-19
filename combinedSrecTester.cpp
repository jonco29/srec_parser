
/***************************************************************************
*
*  srec2c.c
*
*  This program reads an S-Record file and generates a C source file
*  which contains the data.
*
****************************************************************************/

/* ---- Include Files ---------------------------------------------------- */

#include <iostream>
#include <iomanip>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <fstream>
#include <string>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/unistd.h>


#include "srec.h"
#include "SRecMem.h"
#include "combinedSrec2mem.h"
#include "ucryptr_interface.h"
#include "MaceBlob.h"

/* ---- Public Variables ------------------------------------------------- */
/* ---- Private Constants and Types -------------------------------------- */

/* ---- Private Variables ------------------------------------------------ */
/* ---- Private Function Prototypes -------------------------------------- */

//**************************************************************************


//**************************************************************************


/* ---- Functions -------------------------------------------------------- */

/***************************************************************************
*
*  main
*
****************************************************************************/

int test (const char* arg)
{
    //CombinedSRecord2Mem srecMem(arg);
    CombinedSRecord2Mem *srecMem = new CombinedSRecord2Mem(arg);
    MaceBlob *m;

    while ((m = srecMem->getNextImage()) != 0)
    {
        cout <<"================================================================================\n";
        printf("this is id: %02X\n", m->getId());
        printf("this is algoId: %02X\n", m->getAlgoId());
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
        }

    }
        


    delete srecMem;


    cout << "all done" << endl;
    // unsigned char *data = 0;
    // int len = 0;
    // const int reqLen = 48;
    // SRecord2Mem srecMem(arg);
    // //srecMem.ParseFile( srecMem.getFile() );
    // std::cout << "the length of the srec is: " << srecMem.getSrecLength() << std::endl;
    // uCryptrInterface uc;

    // if (uc.isReady())
    // {
    //     std::cout << "uCryptR is ready........." << std::endl;
    //     if (uc.sendMACEboot() == true)
    //     {
    //         std::cout << "MACEboot sent........." << std::endl;
    //     }
    // }
    // else
    // {
    //     std::cout << "uCryptR is NOT ready........." << std::endl;
    // }
 

    // len = srecMem.getNextData(&data, reqLen);
    // while (len != 0)
    // {
    //     int i;
    //     for (i = 0; i < len; i++)
    //     {
    //         //std::cout << std::hex << data[i] ;
    //         printf("%02X", data[i]);
    //     }
    //     std::cout << std::endl;

    //     UCRYPTR_PAYLOAD_t* uc_payload = uc.formatData(data, len, 0x53);
    //     if (uc.send(uc_payload))
    //     {
    //         std::cout << "send success" << std::endl;
    //     }
    //     else
    //     {
    //         std::cout << "send failed" << std::endl;
    //     }
    //             
    //     delete data;
    //     data = 0;
    //     len = srecMem.getNextData(&data, reqLen);
    // }
}
int main( int argc, char **argv )
{
   if ( argc != 2 )
   {
      fprintf( stderr, "Usage: combinedSrec2mem\n" );
      exit( 1 );
   }

   test(argv[1]);

   exit( 0 );
   return 0;

} /* main */

