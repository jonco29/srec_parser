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
#include <vector>
#include <fstream>
#include <string>
#include <time.h>

#include "srec.h"
#include "SRecMem.h"

/* ---- Public Variables ------------------------------------------------- */
/* ---- Private Constants and Types -------------------------------------- */

/* ---- Private Variables ------------------------------------------------ */
/* ---- Private Function Prototypes -------------------------------------- */

//**************************************************************************

class SRecord2C : public SRecordParser
{
public:
            SRecord2C();
   virtual ~SRecord2C();

   bool     OpenOutputFile( const char *fileName, const char *srcFileName, const char *prefix );
   bool     CloseOutputFile();

protected:

   virtual  bool  FinishSegment( unsigned addr, unsigned len );
   virtual  bool  Header( const SRecordHeader *sRecHdr );
   virtual  bool  Data( const SRecordData *sRecData );
   virtual  bool  StartAddress( const SRecordData *sRecData );
   virtual  bool  StartSegment( unsigned addr );

private:

   std::vector< SRecMemBlock >   m_memBlock;
   unsigned                      m_startAddr;
   unsigned                      m_dataOffset;
   unsigned                      m_segIdx;

   std::ofstream                 m_outputFile;
  
   std::string                   m_prefix;
};

//**************************************************************************

SRecord2C::SRecord2C()
{
   m_startAddr = 0;
   m_dataOffset = 0;
   m_segIdx = 0;
}

//**************************************************************************

SRecord2C::~SRecord2C()
{
   CloseOutputFile();
}

//**************************************************************************

bool SRecord2C::CloseOutputFile()
{
   if ( !m_outputFile.is_open() )
   {
      return true;
   }

   if ( !Flush() )
   {
      return false;
   }

   m_outputFile << "SRecMemBlock " << m_prefix << "_SRecMemBlock[ " 
                << m_memBlock.size() << " ] =" << std::endl;

   m_outputFile << "{" << std::endl;
   for ( int idx = 0; idx < m_memBlock.size(); idx++ ) 
   {
      m_outputFile << "    { 0x" << std::setw( 6 ) << m_memBlock[ idx ].m_loadAddr
                   << ", " << m_prefix << "_SRecMemData_"
                   << idx << "_" << std::setw( 6 ) << m_memBlock[ idx ].m_loadAddr
                   << ", 0x" << std::setw( 4 ) << m_memBlock[ idx ].m_dataLen 
                   << " }," << std::endl;
   }
   m_outputFile << "};" << std::endl;
   m_outputFile << std::endl;

   m_outputFile << "SRecMem " << m_prefix << "_SRecMem = { 0x"
                << std::setw( 6 ) << m_startAddr << ", " 
                << std::setbase( 10 ) << m_memBlock.size() << ", "
                << m_prefix << "_SRecMemBlock };" << std::endl;

   m_outputFile.close();

   return true;
}

//**************************************************************************
// virtual

bool SRecord2C::Data( const SRecordData *sRecData )
{
   for ( int i = 0; i < sRecData->m_dataLen; i++ )
   {
      if ( m_dataOffset >= 16 )
      {
         m_outputFile << std::endl;
         m_dataOffset = 0;
      }
      
      if ( m_dataOffset == 0 )
      {
         m_outputFile << "   /* 0x" << std::setw( 6 ) << sRecData->m_addr + i 
                      << " */ ";
      }

      m_outputFile << " 0x" << std::setw( 2 ) << (unsigned)sRecData->m_data[ i ] << ",";
      m_dataOffset++;
   }

   return true;
}

//**************************************************************************
// virtual

bool SRecord2C::FinishSegment( unsigned addr, unsigned len )
{
   m_outputFile << std::endl;
   m_outputFile << "};" << std::endl;
   m_outputFile << std::endl;

   SRecMemBlock  sRecMemBlock;

   sRecMemBlock.m_loadAddr = addr;
   sRecMemBlock.m_data     = NULL;
   sRecMemBlock.m_dataLen  = len;
   
   m_memBlock.push_back( sRecMemBlock );

   m_segIdx++;

   return true;
}

//**************************************************************************
// virtual

bool SRecord2C::Header( const SRecordHeader *sRecHdr )
{
   m_outputFile << "/***************************************************************************" << std::endl;
   m_outputFile << "*" << std::endl;
   m_outputFile << "*  Module: " << sRecHdr->m_module  << std::endl;
   m_outputFile << "*     Ver: " << sRecHdr->m_ver     << std::endl;
   m_outputFile << "*     Rev: " << sRecHdr->m_rev     << std::endl;
   m_outputFile << "* Comment: " << sRecHdr->m_comment << std::endl;
   m_outputFile << "*" << std::endl;
   m_outputFile << "****************************************************************************/" << std::endl;
   m_outputFile << std::endl;

   return true;
}

//**************************************************************************

bool SRecord2C::OpenOutputFile( const char *fileName, const char *srcFileName, const char *prefix )
{
   m_prefix = prefix;

   m_outputFile.open( fileName );
   if ( !m_outputFile )
   {
      std::cerr << "Unable to open '" << fileName << "' for writing." << std::endl;
      return false;
   }

   time_t   currTime;

   time( &currTime );

   m_outputFile << "/***************************************************************************" << std::endl;
   m_outputFile << "*" << std::endl;
   m_outputFile << "* This file was automatically generated by SRec2C from " << srcFileName << std::endl;
   m_outputFile << "*" << std::endl;
   m_outputFile << "* Generated on " << ctime( &currTime );
   m_outputFile << "*" << std::endl;
   m_outputFile << "****************************************************************************/" << std::endl;
   m_outputFile << std::endl;
   m_outputFile << "#include \"SRecMem.h\"" << std::endl;
   m_outputFile << std::endl;

   m_outputFile << std::setfill( '0' );
   m_outputFile << std::setbase( 16 );

   m_segIdx = 0;

   return true;
}

//**************************************************************************
// virtual

bool SRecord2C::StartAddress( const SRecordData *sRecData )
{
   m_startAddr = sRecData->m_addr;

   return true;
}

//**************************************************************************
// virtual

bool SRecord2C::StartSegment( unsigned addr )
{
   m_dataOffset = 0;

   m_outputFile << "/*" << std::endl;
   m_outputFile << " * Address: 0x" << std::setw( 6 ) << addr << std::endl;
   m_outputFile << " */" << std::endl;
   m_outputFile << std::endl;
   m_outputFile << "unsigned char " << m_prefix << "_SRecMemData_" 
                << m_segIdx << "_" << std::setw( 6 ) << addr << "[] = " << std::endl;
   m_outputFile << "{" << std::endl;

   return true;
}

/* ---- Functions -------------------------------------------------------- */

/***************************************************************************
*
*  main
*
****************************************************************************/

int main( int argc, char **argv )
{
   if ( argc != 4 )
   {
      fprintf( stderr, "Usage: srec2c srec-file c-file prefix\n" );
      exit( 1 );
   }

   SRecord2C sRec2C;

   if ( !sRec2C.OpenOutputFile( argv[ 2 ], argv[ 1 ], argv[ 3 ]))
   {
      exit( 1 );
   }

   sRec2C.ParseFile( argv[ 1 ]);
   sRec2C.CloseOutputFile();

   exit( 0 );
   return 0;

} /* main */

