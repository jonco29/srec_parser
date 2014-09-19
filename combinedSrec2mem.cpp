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
#include <vector>

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

using namespace std;
/* ---- Public Variables ------------------------------------------------- */
/* ---- Private Constants and Types -------------------------------------- */

/* ---- Private Variables ------------------------------------------------ */
/* ---- Private Function Prototypes -------------------------------------- */

//**************************************************************************

// bool SRecordParser::ParseFile( FILE *fs )
// {
//    unsigned lineNum = 0;
//    char     line[ 200 ];
// 
//    while ( fgets( line, sizeof( line ), fs ) != NULL )
//    {
//       lineNum++;
// 
//       if ( !ParseLine( lineNum, line ))
//       {
//          return false;
//       }
//    }
//    Flush();
// 
//    return true;
// }

//**************************************************************************

CombinedSRecord2Mem::CombinedSRecord2Mem(const char* fileName)
: file(0),
    combinedSrecValidated(false),
    combinedSrecParsed(false)
{
    if (openFile(fileName))
    {
        char line[1000];
        unsigned int lineNum = 1;
        while ((combinedSrecValidated == false) && (fgets( line, sizeof(line), file) != NULL))
        {
            ParseLine(lineNum, line);
        }

        ParseFile(file);
        fclose(file);
        file = NULL;
    }
    else
    {
        exit ;
    }
}

CombinedSRecord2Mem::~CombinedSRecord2Mem()
{
    if (file != 0)
    {
        fclose(file);
        file = NULL;
    }
}


bool  CombinedSRecord2Mem::Data( const SRecordData *sRecData )
{
    const unsigned char combinedSrecKey[] = { 0x41, 0x42, 0x43, 0x44, 0x43, 0x46, 0x57, 0x49};
    if (sRecData != NULL)
    {
        if (combinedSrecValidated == false)
        {
            if (memcmp(combinedSrecKey, sRecData->m_data, sizeof(combinedSrecKey)) == 0)
            {
                combinedSrecValidated = true;
                return true;
            }
            else
            {
                return false;
            }
        }
        else if (combinedSrecParsed == false)
        {
            unsigned char numImages = sRecData->m_data[0];
            int i;
            int j;
            int dataIndex = 1;
            unsigned int addr;
            for (i = 0; i < numImages; i++)
            {
                addr = sRecData->m_addr & 0xfff00000;
                addr |= (sRecData->m_data[dataIndex++] << 24);
                addr |= (sRecData->m_data[dataIndex++] << 16);
                addr |= (sRecData->m_data[dataIndex++] << 8 );
                addr |= (sRecData->m_data[dataIndex++] << 0 );
                printf ("the address of image #%d is %08X\n", i+1, addr);
                images.push_back(CombinedSrecImageData(addr));
            }
            combinedSrecParsed = true;

            // now put the next addresses in the vector
            for ( imageIt = images.begin(); imageIt != images.end(); imageIt++)
            {
                if ((imageIt+1) != images.end())
                {
                    imageIt->setNextAddr( (imageIt+1)->getAddress());
                }
            }
            return true;
        }
        else
        {
            // iterate over our images till we find ours, then we will read data
            for ( imageIt = images.begin(); imageIt != images.end(); imageIt++)
            {
                if (imageIt->getAddress() == sRecData->m_addr)
                {
                    CombinedSrecImageHeader c(file, (const char*)sRecData->m_data);
                    unsigned char* d = c.getData();
                    imageIt->setHeader(d);
                    delete d;

                    // now iterate over till we get to the next address, as indicated by a false
                    bool  stillWorking = true;
                    char line[1000];
                    unsigned int lineNum = 1;
                    while (fgets (line, sizeof(line), file) != NULL)
                    {
                        if (imageIt->ParseLine(lineNum++, line) == false)
                        {
                            break;
                        }
                    }
                    break;
                }
            }
            return true;
        }
    }
    else
    {
        return false;
    }
}

bool  CombinedSRecord2Mem::FinishSegment( unsigned addr, unsigned len )
{
    return true;
}

bool CombinedSRecord2Mem::openFile(const char* fileName)
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
       file = fs;
       return true;
   }
   return false;
}
// private
FILE* CombinedSRecord2Mem::getFile()
{
    return file;
}

MaceBlob* CombinedSRecord2Mem::getNextImage()
{
    MaceBlob *m = images[0].getImage();
    return m;
}

////////////////////////////////////////////////////////////////////////////////
// CombinedSrecImageHeader stuff
////////////////////////////////////////////////////////////////////////////////
CombinedSrecImageHeader::CombinedSrecImageHeader(FILE *f, const char* initialData)
{
    file = f;
    // first copy the initial data
    memcpy(data, initialData, 16);

    char line[1000];
    unsigned int lineNum = 1;
    if (fgets( line, sizeof(line), file) != NULL)
    {
        ParseLine(lineNum, line);
    }

}
CombinedSrecImageHeader::~CombinedSrecImageHeader()
{
}
bool  CombinedSrecImageHeader::Data( const SRecordData *sRecData )
{
    memcpy(&data[16], sRecData->m_data, sRecData->m_dataLen);
}
unsigned char* CombinedSrecImageHeader::getData()
{
    unsigned char *retData = new unsigned char [32];
    memcpy(retData, data, 32);
    return retData;
}

////////////////////////////////////////////////////////////////////////////////
// CombinedSrecImageData stuff
////////////////////////////////////////////////////////////////////////////////

CombinedSrecImageData::CombinedSrecImageData(unsigned int addr)
:address(addr),
    blob(0)
{
    memset (header, 0, 32);
}
CombinedSrecImageData::~CombinedSrecImageData()
{
}
void CombinedSrecImageData::setNextAddr(unsigned int addr)
{
    nextAddress = addr;
}

void CombinedSrecImageData::setHeader(unsigned char* headerData)
{
    memcpy(header, headerData, 32);
    blob = new MaceBlob(headerData);
}
bool  CombinedSrecImageData::Data( const SRecordData *sRecData )
{
    // we want to stop before the next address
    blob->addData((unsigned char*)sRecData->m_data, sRecData->m_dataLen);

    if ( ((sRecData->m_addr + sRecData->m_dataLen) == nextAddress) || (sRecData->m_dataLen < 16))
    {
        return false;
    }
    else
    {
        return true;
    }
}
unsigned int CombinedSrecImageData::getAddress()
{
    return address;
}
unsigned int CombinedSrecImageData::getNextAddress()
{
    return nextAddress;
}
MaceBlob *CombinedSrecImageData::getImage()
{
    MaceBlob* m = new MaceBlob(*blob);
    return m;
}

// private:
//     FILE *file;
//     unsigned int address;
//     unsigned char header[32];
//     unsigned char* data;
//     unsigned int dataLength;
//     unsigned int index;
// };
