
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
            delete data;
        }
        delete m;
    }

    delete srecMem;
    cout << "all done" << endl;
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

