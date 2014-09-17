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

SRecord2Mem::SRecord2Mem(const char* fileName)
: mLength (0),
    file(0),
    dataArray(0),
    currentDataOffset(0),
    dataArraySize(32)
{
    if (openFile(fileName))
    {
        dataArraySize = mLength;
        dataArray = new unsigned char[dataArraySize];
        ParseFile(file);
        fclose(file);
        file = NULL;
    }
    else
    {
        dataArraySize = 32;
        dataArray = new unsigned char[dataArraySize];
    }
}

SRecord2Mem::~SRecord2Mem()
{
    if (file != 0)
    {
        fclose(file);
        file = NULL;
    }
    if (dataArray != 0)
    {
        std::cout  << "deleting data" << std::endl;
        delete dataArray;
        dataArray = NULL;
    }
}


bool  SRecord2Mem::Data( const SRecordData *sRecData )
{
    if (sRecData != NULL)
    {
        if ((currentDataOffset + sRecData->m_dataLen) < dataArraySize)
        {
            memcpy(&dataArray[currentDataOffset], sRecData->m_data, sRecData->m_dataLen);
            currentDataOffset += sRecData->m_dataLen;
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

bool  SRecord2Mem::FinishSegment( unsigned addr, unsigned len )
{
    mLength = len;
    currentDataOffset = 0;
    return true;
}

bool SRecord2Mem::openFile(const char* fileName)
{
    // this will open a file.  the file will be closed by srec.cpp, upon parseFile();
   FILE  *fs = NULL;
   int fd = 0;

   if (( fs = fopen( fileName, "rt" )) == NULL )
   {
      return false;
   }
   fd = fileno(fs);

   struct stat stat_buf;
   int rc = fstat(fd, &stat_buf);
   if (rc == 0)
   {
       mLength = stat_buf.st_size/2;
       file = fs;
       return true;
   }
   return false;
}
// private
FILE* SRecord2Mem::getFile()
{
    return file;
}

int SRecord2Mem::getNextData(unsigned char** ptr, int reqLen)
{
    int len = 0;

    if (reqLen <= (mLength - currentDataOffset))
    {
        len = reqLen;
    }
    else if (reqLen > (mLength - currentDataOffset))
    {
        len = (mLength - currentDataOffset);
    }
    if (len > 0)
    {
        *ptr = new unsigned char[len];
        memcpy(*ptr, &dataArray[currentDataOffset], len);
        currentDataOffset += len;
    }
    return len;
}

int SRecord2Mem::getSrecLength()
{
    return mLength;
}


