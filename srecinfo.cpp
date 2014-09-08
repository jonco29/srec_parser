/****************************************************************************
*
*  srecinfo.c
*
*  This program reads an S-Record file and prints out information about the
*  file.
*
****************************************************************************/

// ---- Include Files ------------------------------------------------------

#include <iostream>
#include <iomanip>

#include <string.h>
#include <stdlib.h>

#include "srec.h"

// ---- Public Variables ---------------------------------------------------
// ---- Private Constants and Types ----------------------------------------

// ---- Private Variables --------------------------------------------------
// ---- Private Function Prototypes ----------------------------------------

//**************************************************************************

class SRecordInfo : public SRecordParser
{
public:
             SRecordInfo();
   virtual  ~SRecordInfo();

protected:

   virtual  bool  FinishSegment( unsigned addr, unsigned len );
   virtual  bool  Header( const SRecordHeader *sRecHdr );
   virtual  bool  StartAddress( const SRecordData *sRecData );
};

//**************************************************************************

SRecordInfo::SRecordInfo()
{
   // Nothing to do
}

//**************************************************************************

SRecordInfo::~SRecordInfo()
{
   // Nothing to do
}

//**************************************************************************
// virtual

bool SRecordInfo::Header( const SRecordHeader *sRecHdr )
{
   std::cout << "Module: '" << sRecHdr->m_module  << "', ";
   std::cout << "Ver: '"    << sRecHdr->m_ver     << "', ";
   std::cout << "Rev: '"    << sRecHdr->m_rev     << "', ";
   std::cout << "Descr: '"  << sRecHdr->m_comment << "'" << std::endl;

   return true;
}

//**************************************************************************
// virtual

bool SRecordInfo::FinishSegment( unsigned addr, unsigned len )
{
   std::ios_base::fmtflags  svFlags = std::cout.flags();
   char                     svFill  = std::cout.fill();

   std::cout << "Segment Address: 0x"
             << std::setbase( 16 ) << std::setw( 6 ) << std::setfill( '0' ) << addr
             << " Len: 0x" 
             << std::setbase( 16 ) << std::setw( 6 ) << std::setfill( '0' ) << len
             << std::endl;

   std::cout.flags( svFlags );
   std::cout.fill( svFill );

   return true;
}

//**************************************************************************
// virtual

bool SRecordInfo::StartAddress( const SRecordData *sRecData )
{
   std::ios_base::fmtflags  svFlags = std::cout.flags();
   char                     svFill  = std::cout.fill();

   std::cout << "  Start Address: 0x"
             << std::setbase( 16 ) << std::setw( 6 ) << std::setfill( '0' ) << sRecData->m_addr
             << std::endl;

   std::cout.flags( svFlags );
   std::cout.fill( svFill );

   return true;
}

/* ---- Functions -------------------------------------------------------- */

/***************************************************************************
*
*  main
*
***************************************************************************/

int main( int argc, char **argv )
{
   if ( argc != 2 )
   {
      fprintf( stderr, "Usage: srecinfo srec-file\n" );
      exit( 1 );
   }

   SRecordInfo sRecInfo;

   sRecInfo.ParseFile( argv[ 1 ]);
   sRecInfo.Flush();

   exit( 0 );
   return 0;

} /* main */

