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
#include "srec2mem.h"
#include "ucryptr_interface.h"

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
    unsigned char *data = 0;
    int len = 0;
    const int reqLen = 48;
    SRecord2Mem srecMem(arg);
    //srecMem.ParseFile( srecMem.getFile() );
    std::cout << "the length of the srec is: " << srecMem.getSrecLength() << std::endl;
    uCryptrInterface uc;

    if (uc.isReady())
    {
        std::cout << "uCryptR is ready........." << std::endl;
        if (uc.sendMACEboot() == true)
        {
            std::cout << "MACEboot sent........." << std::endl;
        }
    }
    else
    {
        std::cout << "uCryptR is NOT ready........." << std::endl;
    }
 

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

        UCRYPTR_PAYLOAD_t* uc_payload = uc.formatData(data, len, 0x53);
        if (uc.send(uc_payload))
        {
            std::cout << "send success" << std::endl;
        }
        else
        {
            std::cout << "send failed" << std::endl;
        }
                
        delete data;
        data = 0;
        len = srecMem.getNextData(&data, reqLen);
    }
}
int main( int argc, char **argv )
{
   if ( argc != 2 )
   {
      fprintf( stderr, "Usage: srec2c\n" );
      exit( 1 );
   }

   test(argv[1]);

   exit( 0 );
   return 0;

} /* main */

