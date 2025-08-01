/**CFile****************************************************************

  FileName    [io.c]

  SystemName  [ABC: Logic synthesis and verification system.]

  PackageName [Command processing package.]

  Synopsis    [Command file.]

  Author      [Alan Mishchenko]
  
  Affiliation [UC Berkeley]

  Date        [Ver. 1.0. Started - June 20, 2005.]

  Revision    [$Id: io.c,v 1.00 2005/06/20 00:00:00 alanmi Exp $]

***********************************************************************/

#include "ioAbc.h"
#include "base/main/mainInt.h"
#include "aig/saig/saig.h"
#include "proof/abs/abs.h"
#include "sat/bmc/bmc.h"

#ifdef WIN32
#include <process.h> 
#define unlink _unlink
#else
#include <unistd.h>
#endif

ABC_NAMESPACE_IMPL_START

////////////////////////////////////////////////////////////////////////
///                        DECLARATIONS                              ///
////////////////////////////////////////////////////////////////////////

static int IoCommandRead        ( Abc_Frame_t * pAbc, int argc, char **argv );
static int IoCommandReadAiger   ( Abc_Frame_t * pAbc, int argc, char **argv );
static int IoCommandReadBaf     ( Abc_Frame_t * pAbc, int argc, char **argv );
static int IoCommandReadBblif   ( Abc_Frame_t * pAbc, int argc, char **argv );
static int IoCommandReadBlif    ( Abc_Frame_t * pAbc, int argc, char **argv );
static int IoCommandReadBlifMv  ( Abc_Frame_t * pAbc, int argc, char **argv );
static int IoCommandReadBench   ( Abc_Frame_t * pAbc, int argc, char **argv );
static int IoCommandReadCex     ( Abc_Frame_t * pAbc, int argc, char **argv );
static int IoCommandReadDsd     ( Abc_Frame_t * pAbc, int argc, char **argv );
static int IoCommandReadEdif    ( Abc_Frame_t * pAbc, int argc, char **argv );
static int IoCommandReadEqn     ( Abc_Frame_t * pAbc, int argc, char **argv );
static int IoCommandReadFins    ( Abc_Frame_t * pAbc, int argc, char **argv );
static int IoCommandReadInit    ( Abc_Frame_t * pAbc, int argc, char **argv );
static int IoCommandReadPla     ( Abc_Frame_t * pAbc, int argc, char **argv );
static int IoCommandReadPlaMo   ( Abc_Frame_t * pAbc, int argc, char **argv );
static int IoCommandReadTruth   ( Abc_Frame_t * pAbc, int argc, char **argv );
static int IoCommandReadCnf     ( Abc_Frame_t * pAbc, int argc, char **argv );
static int IoCommandReadVerilog ( Abc_Frame_t * pAbc, int argc, char **argv );
static int IoCommandReadStatus  ( Abc_Frame_t * pAbc, int argc, char **argv );
static int IoCommandReadGig     ( Abc_Frame_t * pAbc, int argc, char **argv );
static int IoCommandReadJson    ( Abc_Frame_t * pAbc, int argc, char **argv );
static int IoCommandReadJsonc   ( Abc_Frame_t * pAbc, int argc, char **argv );
static int IoCommandReadSF      ( Abc_Frame_t * pAbc, int argc, char **argv );
static int IoCommandReadRom     ( Abc_Frame_t * pAbc, int argc, char **argv );
static int IoCommandReadMM      ( Abc_Frame_t * pAbc, int argc, char **argv );
static int IoCommandReadMMGia   ( Abc_Frame_t * pAbc, int argc, char **argv );

static int IoCommandWrite       ( Abc_Frame_t * pAbc, int argc, char **argv );
static int IoCommandWriteHie    ( Abc_Frame_t * pAbc, int argc, char **argv );
static int IoCommandWriteAiger  ( Abc_Frame_t * pAbc, int argc, char **argv );
static int IoCommandWriteAigerCex( Abc_Frame_t * pAbc, int argc, char **argv );
static int IoCommandWriteBaf    ( Abc_Frame_t * pAbc, int argc, char **argv );
static int IoCommandWriteBblif  ( Abc_Frame_t * pAbc, int argc, char **argv );
static int IoCommandWriteBlif   ( Abc_Frame_t * pAbc, int argc, char **argv );
static int IoCommandWriteEdgelist( Abc_Frame_t * pAbc, int argc, char **argv );
static int IoCommandWriteBlifMv ( Abc_Frame_t * pAbc, int argc, char **argv );
static int IoCommandWriteBench  ( Abc_Frame_t * pAbc, int argc, char **argv );
static int IoCommandWriteBook   ( Abc_Frame_t * pAbc, int argc, char **argv );
static int IoCommandWriteCellNet( Abc_Frame_t * pAbc, int argc, char **argv );
static int IoCommandWriteCnf    ( Abc_Frame_t * pAbc, int argc, char **argv );
static int IoCommandWriteCnf2   ( Abc_Frame_t * pAbc, int argc, char **argv );
static int IoCommandWriteCex    ( Abc_Frame_t * pAbc, int argc, char **argv );
static int IoCommandWriteDot    ( Abc_Frame_t * pAbc, int argc, char **argv );
static int IoCommandWriteEqn    ( Abc_Frame_t * pAbc, int argc, char **argv );
static int IoCommandWriteGml    ( Abc_Frame_t * pAbc, int argc, char **argv );
static int IoCommandWriteHMetis ( Abc_Frame_t * pAbc, int argc, char **argv );
static int IoCommandWriteList   ( Abc_Frame_t * pAbc, int argc, char **argv );
static int IoCommandWritePla    ( Abc_Frame_t * pAbc, int argc, char **argv );
static int IoCommandWriteVerilog( Abc_Frame_t * pAbc, int argc, char **argv );
static int IoCommandWriteSortCnf( Abc_Frame_t * pAbc, int argc, char **argv );
static int IoCommandWriteTruth  ( Abc_Frame_t * pAbc, int argc, char **argv );
static int IoCommandWriteTruths ( Abc_Frame_t * pAbc, int argc, char **argv );
static int IoCommandWriteStatus ( Abc_Frame_t * pAbc, int argc, char **argv );
static int IoCommandWriteSmv    ( Abc_Frame_t * pAbc, int argc, char **argv );
static int IoCommandWriteJson   ( Abc_Frame_t * pAbc, int argc, char **argv );
static int IoCommandWriteResub  ( Abc_Frame_t * pAbc, int argc, char **argv );
static int IoCommandWriteMM     ( Abc_Frame_t * pAbc, int argc, char **argv );
static int IoCommandWriteMMGia  ( Abc_Frame_t * pAbc, int argc, char **argv );

extern void Abc_FrameCopyLTLDataBase( Abc_Frame_t *pAbc, Abc_Ntk_t * pNtk );

extern int glo_fMapped;

////////////////////////////////////////////////////////////////////////
///                     FUNCTION DEFINITIONS                         ///
////////////////////////////////////////////////////////////////////////

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Io_Init( Abc_Frame_t * pAbc )
{
    Cmd_CommandAdd( pAbc, "I/O", "read",          IoCommandRead,         1 );
    Cmd_CommandAdd( pAbc, "I/O", "read_aiger",    IoCommandReadAiger,    1 );
    Cmd_CommandAdd( pAbc, "I/O", "read_baf",      IoCommandReadBaf,      1 );
    Cmd_CommandAdd( pAbc, "I/O", "read_bblif",    IoCommandReadBblif,    1 );
    Cmd_CommandAdd( pAbc, "I/O", "read_blif",     IoCommandReadBlif,     1 );
    Cmd_CommandAdd( pAbc, "I/O", "read_blif_mv",  IoCommandReadBlifMv,   1 );
    Cmd_CommandAdd( pAbc, "I/O", "read_bench",    IoCommandReadBench,    1 );
    Cmd_CommandAdd( pAbc, "I/O", "read_cex",      IoCommandReadCex,      1 );
    Cmd_CommandAdd( pAbc, "I/O", "read_dsd",      IoCommandReadDsd,      1 );
    Cmd_CommandAdd( pAbc, "I/O", "read_formula",  IoCommandReadDsd,      1 );
//    Cmd_CommandAdd( pAbc, "I/O", "read_edif",     IoCommandReadEdif,     1 );
    Cmd_CommandAdd( pAbc, "I/O", "read_eqn",      IoCommandReadEqn,      1 );
    Cmd_CommandAdd( pAbc, "I/O", "read_fins",     IoCommandReadFins,     0 );
    Cmd_CommandAdd( pAbc, "I/O", "read_init",     IoCommandReadInit,     1 );
    Cmd_CommandAdd( pAbc, "I/O", "read_pla",      IoCommandReadPla,      1 );
    Cmd_CommandAdd( pAbc, "I/O", "read_plamo",    IoCommandReadPlaMo,    1 );
    Cmd_CommandAdd( pAbc, "I/O", "read_truth",    IoCommandReadTruth,    1 );
    Cmd_CommandAdd( pAbc, "I/O", "read_cnf",      IoCommandReadCnf,      1 );    
    Cmd_CommandAdd( pAbc, "I/O", "read_verilog",  IoCommandReadVerilog,  1 );
    Cmd_CommandAdd( pAbc, "I/O", "read_status",   IoCommandReadStatus,   0 );
    Cmd_CommandAdd( pAbc, "I/O", "&read_gig",     IoCommandReadGig,      0 );
    Cmd_CommandAdd( pAbc, "I/O", "read_json",     IoCommandReadJson,     0 );
    Cmd_CommandAdd( pAbc, "I/O", "read_jsonc",    IoCommandReadJsonc,    0 );
    Cmd_CommandAdd( pAbc, "I/O", "read_sf",       IoCommandReadSF,       0 );
    Cmd_CommandAdd( pAbc, "I/O", "read_rom",      IoCommandReadRom,      1 );
    Cmd_CommandAdd( pAbc, "I/O", "read_mm",       IoCommandReadMM,       1 );
    Cmd_CommandAdd( pAbc, "I/O", "&read_mm",      IoCommandReadMMGia,    1 );

    Cmd_CommandAdd( pAbc, "I/O", "write",         IoCommandWrite,        0 );
    Cmd_CommandAdd( pAbc, "I/O", "write_hie",     IoCommandWriteHie,     0 );
    Cmd_CommandAdd( pAbc, "I/O", "write_aiger",   IoCommandWriteAiger,   0 );
    Cmd_CommandAdd( pAbc, "I/O", "write_aiger_cex",   IoCommandWriteAigerCex,   0 );
    Cmd_CommandAdd( pAbc, "I/O", "write_baf",     IoCommandWriteBaf,     0 );
    Cmd_CommandAdd( pAbc, "I/O", "write_bblif",   IoCommandWriteBblif,   0 );
    Cmd_CommandAdd( pAbc, "I/O", "write_blif",    IoCommandWriteBlif,    0 );
    Cmd_CommandAdd( pAbc, "I/O", "write_blif_mv", IoCommandWriteBlifMv,  0 );
    Cmd_CommandAdd( pAbc, "I/O", "write_bench",   IoCommandWriteBench,   0 );
    Cmd_CommandAdd( pAbc, "I/O", "write_book",    IoCommandWriteBook,    0 );
    Cmd_CommandAdd( pAbc, "I/O", "write_cellnet", IoCommandWriteCellNet, 0 );
    Cmd_CommandAdd( pAbc, "I/O", "write_cex",     IoCommandWriteCex,     0 );
    Cmd_CommandAdd( pAbc, "I/O", "write_cnf",     IoCommandWriteCnf,     0 );
    Cmd_CommandAdd( pAbc, "I/O", "&write_cnf",    IoCommandWriteCnf2,    0 );
    Cmd_CommandAdd( pAbc, "I/O", "write_dot",     IoCommandWriteDot,     0 );
    Cmd_CommandAdd( pAbc, "I/O", "write_eqn",     IoCommandWriteEqn,     0 );
    Cmd_CommandAdd( pAbc, "I/O", "write_edgelist",IoCommandWriteEdgelist,    0 );
    Cmd_CommandAdd( pAbc, "I/O", "write_gml",     IoCommandWriteGml,     0 );
//    Cmd_CommandAdd( pAbc, "I/O", "write_list",    IoCommandWriteList,    0 );
    Cmd_CommandAdd( pAbc, "I/O", "write_hmetis",  IoCommandWriteHMetis,     0 );
    Cmd_CommandAdd( pAbc, "I/O", "write_pla",     IoCommandWritePla,     0 );
    Cmd_CommandAdd( pAbc, "I/O", "write_verilog", IoCommandWriteVerilog, 0 );
//    Cmd_CommandAdd( pAbc, "I/O", "write_verlib",  IoCommandWriteVerLib,  0 );
    Cmd_CommandAdd( pAbc, "I/O", "write_sorter_cnf", IoCommandWriteSortCnf,  0 );
    Cmd_CommandAdd( pAbc, "I/O", "write_truth",   IoCommandWriteTruth,   0 );
    Cmd_CommandAdd( pAbc, "I/O", "&write_truth",  IoCommandWriteTruths,  0 );
    Cmd_CommandAdd( pAbc, "I/O", "&write_truths", IoCommandWriteTruths,  0 );
    Cmd_CommandAdd( pAbc, "I/O", "write_status",  IoCommandWriteStatus,  0 );
    Cmd_CommandAdd( pAbc, "I/O", "write_smv",     IoCommandWriteSmv,     0 );
    Cmd_CommandAdd( pAbc, "I/O", "write_json",    IoCommandWriteJson,    0 );
    Cmd_CommandAdd( pAbc, "I/O", "&write_resub",  IoCommandWriteResub,   0 );
    Cmd_CommandAdd( pAbc, "I/O", "write_mm",      IoCommandWriteMM,      0 );
    Cmd_CommandAdd( pAbc, "I/O", "&write_mm",     IoCommandWriteMMGia,   0 );
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Io_End( Abc_Frame_t * pAbc )
{
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int IoCommandRead( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    char Command[1000];
    Abc_Ntk_t * pNtk;
    char * pFileName, * pTemp;
    int c, fCheck, fBarBufs, fReadGia;

    fCheck = 1;
    fBarBufs = 0;
    fReadGia = 0;
    glo_fMapped = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "mcbgh" ) ) != EOF )
    {
        switch ( c )
        {
            case 'm':
                glo_fMapped ^= 1;
                break;
            case 'c':
                fCheck ^= 1;
                break;
            case 'b':
                fBarBufs ^= 1;
                break;
            case 'g':
                fReadGia ^= 1;
                break;
            case 'h':
                goto usage;
            default:
                goto usage;
        }
    }
    if ( argc != globalUtilOptind + 1 )
        goto usage;
    // get the input file name
    pFileName = argv[globalUtilOptind];
    // fix the wrong symbol
    for ( pTemp = pFileName; *pTemp; pTemp++ )
        if ( *pTemp == '>' || *pTemp == '\\' )
            *pTemp = '/';
    // read libraries
    Command[0] = 0;
    assert( strlen(pFileName) < 900 );
    if ( !strcmp( Extra_FileNameExtension(pFileName), "genlib" )  )
        sprintf( Command, "read_genlib %s", pFileName );
    else if ( !strcmp( Extra_FileNameExtension(pFileName), "lib" ) )
        sprintf( Command, "read_lib %s", pFileName );
    else if ( !strcmp( Extra_FileNameExtension(pFileName), "scl" ) )
        sprintf( Command, "read_scl %s", pFileName );
    else if ( !strcmp( Extra_FileNameExtension(pFileName), "super" ) )
        sprintf( Command, "read_super %s", pFileName );
    else if ( !strcmp( Extra_FileNameExtension(pFileName), "constr" ) )
        sprintf( Command, "read_constr %s", pFileName );
    else if ( !strcmp( Extra_FileNameExtension(pFileName), "c" ) )
        sprintf( Command, "so %s", pFileName );
    else if ( !strcmp( Extra_FileNameExtension(pFileName), "script" ) )
        sprintf( Command, "so %s", pFileName );
    else if ( !strcmp( Extra_FileNameExtension(pFileName), "dsd" ) )
        sprintf( Command, "dsd_load %s", pFileName );
    if ( Command[0] )
    {
        Cmd_CommandExecute( pAbc, Command );
        return 0;
    }
    if ( fReadGia )
    {
        Abc_Ntk_t * pNtk = Io_ReadNetlist( pFileName, Io_ReadFileType(pFileName), fCheck );
        if ( pNtk )
        {
            Gia_Man_t * pGia = Abc_NtkFlattenHierarchyGia( pNtk, NULL, 0 );
            Abc_NtkDelete( pNtk );
            if ( pGia == NULL )
            {
                Abc_Print( 1, "Abc_CommandBlast(): Bit-blasting has failed.\n" );
                return 0;
            }
            Abc_FrameUpdateGia( pAbc, pGia );
        }
        return 0;
    }
    // check if the library is available
    if ( glo_fMapped && Abc_FrameReadLibGen() == NULL )
    {
        Abc_Print( 1, "Cannot read mapped design when the library is not given.\n" );
        return 0;
    }
    // read the file using the corresponding file reader
    if ( strstr(pFileName, ".") && !strcmp(strstr(pFileName, "."), ".s") ) 
    {
        char Command[1000];
        assert( strlen(pFileName) < 980 );
        sprintf( Command, "source -x %s", pFileName );
        if ( Cmd_CommandExecute( pAbc, Command ) )
        {
            fprintf( stdout, "Cannot execute command \"%s\".\n", Command );
            return 1;
        }
        return 0;
    }
    pNtk = Io_Read( pFileName, Io_ReadFileType(pFileName), fCheck, fBarBufs );
    if ( pNtk == NULL )
        return 0;
    if ( Abc_NtkPiNum(pNtk) == 0 )
    {
        Abc_Print( 0, "The new network has no primary inputs. It is recommended\n" );
        Abc_Print( 1, "to add a dummy PI to make sure all commands work correctly.\n" );
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtk );
    Abc_FrameCopyLTLDataBase( pAbc, pNtk );
    Abc_FrameClearVerifStatus( pAbc );
    return 0;

usage:
    fprintf( pAbc->Err, "usage: read [-mcbgh] <file>\n" );
    fprintf( pAbc->Err, "\t         replaces the current network by the network read from <file>\n" );
    fprintf( pAbc->Err, "\t         by calling the parser that matches the extension of <file>\n" );
    fprintf( pAbc->Err, "\t         (to read a hierarchical design, use \"read_hie\")\n" );
    fprintf( pAbc->Err, "\t-m     : toggle reading mapped Verilog [default = %s]\n", glo_fMapped? "yes":"no" );
    fprintf( pAbc->Err, "\t-c     : toggle network check after reading [default = %s]\n", fCheck? "yes":"no" );
    fprintf( pAbc->Err, "\t-b     : toggle reading barrier buffers [default = %s]\n", fBarBufs? "yes":"no" );
    fprintf( pAbc->Err, "\t-g     : toggle reading and flattening into &-space [default = %s]\n", fBarBufs? "yes":"no" );
    fprintf( pAbc->Err, "\t-h     : prints the command summary\n" );
    fprintf( pAbc->Err, "\tfile   : the name of a file to read\n" );
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int IoCommandReadAiger( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Abc_Ntk_t * pNtk;
    char * pFileName;
    int fCheck;
    int c;

    fCheck = 1;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "ch" ) ) != EOF )
    {
        switch ( c )
        {
            case 'c':
                fCheck ^= 1;
                break;
            case 'h':
                goto usage;
            default:
                goto usage;
        }
    }
    if ( argc != globalUtilOptind + 1 )
        goto usage;
    // get the input file name
    pFileName = argv[globalUtilOptind];
    // read the file using the corresponding file reader
    pNtk = Io_Read( pFileName, IO_FILE_AIGER, fCheck, 0 );
    if ( pNtk == NULL )
        return 1;
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtk );
    Abc_FrameClearVerifStatus( pAbc );
    return 0;

usage:
    fprintf( pAbc->Err, "usage: read_aiger [-ch] <file>\n" );
    fprintf( pAbc->Err, "\t         reads the network in the AIGER format (http://fmv.jku.at/aiger)\n" );
    fprintf( pAbc->Err, "\t-c     : toggle network check after reading [default = %s]\n", fCheck? "yes":"no" );
    fprintf( pAbc->Err, "\t-h     : prints the command summary\n" );
    fprintf( pAbc->Err, "\tfile   : the name of a file to read\n" );
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int IoCommandReadBaf( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Abc_Ntk_t * pNtk;
    char * pFileName;
    int fCheck;
    int c;

    fCheck = 1;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "ch" ) ) != EOF )
    {
        switch ( c )
        {
            case 'c':
                fCheck ^= 1;
                break;
            case 'h':
                goto usage;
            default:
                goto usage;
        }
    }
    if ( argc != globalUtilOptind + 1 )
        goto usage;
    // get the input file name
    pFileName = argv[globalUtilOptind];
    // read the file using the corresponding file reader
    pNtk = Io_Read( pFileName, IO_FILE_BAF, fCheck, 0 );
    if ( pNtk == NULL )
        return 1;
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtk );
    Abc_FrameClearVerifStatus( pAbc );
    return 0;

usage:
    fprintf( pAbc->Err, "usage: read_baf [-ch] <file>\n" );
    fprintf( pAbc->Err, "\t         reads the network in Binary Aig Format (BAF)\n" );
    fprintf( pAbc->Err, "\t-c     : toggle network check after reading [default = %s]\n", fCheck? "yes":"no" );
    fprintf( pAbc->Err, "\t-h     : prints the command summary\n" );
    fprintf( pAbc->Err, "\tfile   : the name of a file to read\n" );
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int IoCommandReadBblif( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Abc_Ntk_t * pNtk;
    char * pFileName;
    int fCheck;
    int c;

    fCheck = 1;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "ch" ) ) != EOF )
    {
        switch ( c )
        {
            case 'c':
                fCheck ^= 1;
                break;
            case 'h':
                goto usage;
            default:
                goto usage;
        }
    }
    if ( argc != globalUtilOptind + 1 )
        goto usage;
    // get the input file name
    pFileName = argv[globalUtilOptind];
    // read the file using the corresponding file reader
    pNtk = Io_Read( pFileName, IO_FILE_BBLIF, fCheck, 0 );
    if ( pNtk == NULL )
        return 1;
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtk );
    Abc_FrameClearVerifStatus( pAbc );
    return 0;

usage:
    fprintf( pAbc->Err, "usage: read_bblif [-ch] <file>\n" );
    fprintf( pAbc->Err, "\t         reads the network in a binary BLIF format\n" );
    fprintf( pAbc->Err, "\t-c     : toggle network check after reading [default = %s]\n", fCheck? "yes":"no" );
    fprintf( pAbc->Err, "\t-h     : prints the command summary\n" );
    fprintf( pAbc->Err, "\tfile   : the name of a file to read\n" );
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int IoCommandReadBlif( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Abc_Ntk_t * pNtk;
    char * pFileName;
    int fReadAsAig;
    int fCheck;
    int fUseNewParser;
    int fSaveNames;
    int c;
    extern Abc_Ntk_t * Io_ReadBlifAsAig( char * pFileName, int fCheck );

    fCheck = 1;
    fReadAsAig = 0;
    fUseNewParser = 1;
    fSaveNames = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "nmach" ) ) != EOF )
    {
        switch ( c )
        {
            case 'n':
                fUseNewParser ^= 1;
                break;
            case 'm':
                fSaveNames ^= 1;
                break;
            case 'a':
                fReadAsAig ^= 1;
                break;
            case 'c':
                fCheck ^= 1;
                break;
            case 'h':
                goto usage;
            default:
                goto usage;
        }
    }
    if ( argc != globalUtilOptind + 1 )
        goto usage;
    // get the input file name
    pFileName = argv[globalUtilOptind];
    // read the file using the corresponding file reader
    if ( fReadAsAig )
        pNtk = Io_ReadBlifAsAig( pFileName, fCheck );
    else if ( fUseNewParser )
        pNtk = Io_Read( pFileName, IO_FILE_BLIF, fCheck, 0 );
    else
    {
        Abc_Ntk_t * pTemp;
        pNtk = Io_ReadBlif( pFileName, fCheck );
        if ( pNtk == NULL )
            return 1;
        if ( fSaveNames )
            Abc_NtkStartNameIds( pNtk );
        pNtk = Abc_NtkToLogic( pTemp = pNtk );
        if ( fSaveNames )
            Abc_NtkTransferNameIds( pTemp, pNtk );
        Abc_NtkDelete( pTemp );
    }

    if ( pNtk == NULL )
        return 1;
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtk );
    Abc_FrameClearVerifStatus( pAbc );
    return 0;

usage:
    fprintf( pAbc->Err, "usage: read_blif [-nmach] <file>\n" );
    fprintf( pAbc->Err, "\t         reads the network in binary BLIF format\n" );
    fprintf( pAbc->Err, "\t         (if this command does not work, try \"read\")\n" );
    fprintf( pAbc->Err, "\t-n     : toggle using old BLIF parser without hierarchy support [default = %s]\n", !fUseNewParser? "yes":"no" );
    fprintf( pAbc->Err, "\t-m     : toggle saving original circuit names into a file [default = %s]\n", fSaveNames? "yes":"no" );
    fprintf( pAbc->Err, "\t-a     : toggle creating AIG while reading the file [default = %s]\n", fReadAsAig? "yes":"no" );
    fprintf( pAbc->Err, "\t-c     : toggle network check after reading [default = %s]\n", fCheck? "yes":"no" );
    fprintf( pAbc->Err, "\t-h     : prints the command summary\n" );
    fprintf( pAbc->Err, "\tfile   : the name of a file to read\n" );
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int IoCommandReadBlifMv( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Abc_Ntk_t * pNtk;
    char * pFileName;
    int fCheck;
    int c;

    fCheck = 1;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "ch" ) ) != EOF )
    {
        switch ( c )
        {
            case 'c':
                fCheck ^= 1;
                break;
            case 'h':
                goto usage;
            default:
                goto usage;
        }
    }
    if ( argc != globalUtilOptind + 1 )
        goto usage;
    // get the input file name
    pFileName = argv[globalUtilOptind];
    // read the file using the corresponding file reader
    pNtk = Io_Read( pFileName, IO_FILE_BLIFMV, fCheck, 0 );
    if ( pNtk == NULL )
        return 1;
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtk );
    Abc_FrameClearVerifStatus( pAbc );
    return 0;

usage:
    fprintf( pAbc->Err, "usage: read_blif_mv [-ch] <file>\n" );
    fprintf( pAbc->Err, "\t         reads the network in BLIF-MV format\n" );
    fprintf( pAbc->Err, "\t         (if this command does not work, try \"read\")\n" );
    fprintf( pAbc->Err, "\t-c     : toggle network check after reading [default = %s]\n", fCheck? "yes":"no" );
    fprintf( pAbc->Err, "\t-h     : prints the command summary\n" );
    fprintf( pAbc->Err, "\tfile   : the name of a file to read\n" );
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int IoCommandReadBench( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Abc_Ntk_t * pNtk;
    char * pFileName;
    int fCheck;
    int c;

    fCheck = 1;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "ch" ) ) != EOF )
    {
        switch ( c )
        {
            case 'c':
                fCheck ^= 1;
                break;
            case 'h':
                goto usage;
            default:
                goto usage;
        }
    }
    if ( argc != globalUtilOptind + 1 )
        goto usage;
    // get the input file name
    pFileName = argv[globalUtilOptind];
    // read the file using the corresponding file reader
    pNtk = Io_Read( pFileName, IO_FILE_BENCH, fCheck, 0 );
    if ( pNtk == NULL )
        return 1;
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtk );
    Abc_FrameClearVerifStatus( pAbc );
    return 0;

usage:
    fprintf( pAbc->Err, "usage: read_bench [-ch] <file>\n" );
    fprintf( pAbc->Err, "\t         reads the network in BENCH format\n" );
    fprintf( pAbc->Err, "\t-c     : toggle network check after reading [default = %s]\n", fCheck? "yes":"no" );
    fprintf( pAbc->Err, "\t-h     : prints the command summary\n" );
    fprintf( pAbc->Err, "\tfile   : the name of a file to read\n" );
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_NtkReadCexFile( char * pFileName, Abc_Ntk_t * pNtk, Abc_Cex_t ** ppCex, Abc_Cex_t ** ppCexCare, int * pnFrames, int * fOldFormat, int xMode )
{
    FILE * pFile;
    Abc_Cex_t * pCex;
    Abc_Cex_t * pCexCare;
    Vec_Int_t * vNums;
    int c, nRegs = -1, nFrames = -1;
    pFile = fopen( pFileName, "r" );
    if ( pFile == NULL )
    {
        printf( "Cannot open log file for reading \"%s\".\n" , pFileName );
        return -1;
    }

    vNums = Vec_IntAlloc( 100 );
    int usedX = 0;
    *fOldFormat = 0;
    
    int MaxLine = 1000000;
    char *Buffer;
    int BufferLen = 0;
    int state = 0;
    int iPo = 0;
    nFrames = -1;
    int status = 0;
    int i;
    int nRegsNtk = 0;
    Abc_Obj_t * pObj;
    Abc_NtkForEachLatch( pNtk, pObj, i ) nRegsNtk++;

    Buffer = ABC_ALLOC( char, MaxLine );
    while ( fgets( Buffer, MaxLine, pFile ) != NULL )
    {
        if ( Buffer[0] == '#' || Buffer[0] == 'c' || Buffer[0] == 'f' || Buffer[0] == 'u' )
            continue;
        BufferLen = strlen(Buffer) - 1;
        Buffer[BufferLen] = '\0';
        if (state==0 && BufferLen>1) {
            // old format detected
            *fOldFormat = 1;
            state = 2;
            iPo = 0;
            status = 1;
        }
        if (state==1 && Buffer[0]!='b' && Buffer[0]!='j') {
            // old format detected, first line was actually register
            *fOldFormat = 1;
            state = 3;
            Vec_IntPush( vNums, status );
            status = 1;
        }
        if (Buffer[0] == '.' )
            break;
        switch(state) {
            case 0 :
                {
                    char c = Buffer[0];
                    if ( c == '0' || c == '1' || c == '2' ) {
                        status = c - '0' ;
                        state = 1;
                    } else if ( c == 'x' ) {
                        // old format with one x state latch
                        usedX = 1;
                        // set to 2 so we can Abc_LatchSetInitNone
                        // acts like 0 when setting bits
                        Vec_IntPush( vNums, 2 );
                        nRegs = Vec_IntSize(vNums);
                        state = 3;
                    } else {
                        printf( "ERROR: Bad aiger status line.\n" );
                        return -1;
                    }
                }
                break;
            case 1 :
                iPo = atoi(Buffer+1);
                state = 2;
                break;
            case 2 :
                for (i=0; i<BufferLen;i++) {
                    char c = Buffer[i];
                    if ( c == '0' || c == '1' )
                        Vec_IntPush( vNums, c - '0' );
                    else if ( c == 'x') {
                        usedX = 1;
                        // set to 2 so we can Abc_LatchSetInitNone
                        // acts like 0 when setting bits
                        Vec_IntPush( vNums, 2 );
                    }
                }
                nRegs = Vec_IntSize(vNums);
                if ( nRegs < nRegsNtk )
                {
                    printf( "WARNING: Register number is smaller than in Ntk. Appending.\n" );
                    for (i=0; i<nRegsNtk-nRegs;i++) {
                        Vec_IntPush( vNums, 0 );
                    }
                    nRegs = Vec_IntSize(vNums);
                }
                else if ( nRegs > nRegsNtk )
                {
                    printf( "WARNING: Register number is larger than in Ntk. Truncating.\n" );
                    Vec_IntShrink( vNums, nRegsNtk );
                    nRegs = nRegsNtk;
                }
                state = 3;
                break;
            default:
                for (i=0; i<BufferLen;i++) {
                    char c = Buffer[i];
                    if ( c == '0' || c == '1' )
                        Vec_IntPush( vNums, c - '0' );
                    else if ( c == 'x') {
                        usedX = 1;
                        Vec_IntPush( vNums, 2 );
                    }
                }
                nFrames++;
                break;
        }
    }
    fclose( pFile );

    if (usedX && !xMode)
        printf( "Warning: Using 0 instead of x in latches or primary inputs\n" );
    int iFrameCex = nFrames;
    if ( nRegs < 0 )
    {
        if (status == 0 || *fOldFormat == 0)
            printf( "Counter-example is not available.\n" );
        else
            printf( "ERROR: Cannot read register number.\n" );
        Vec_IntFree( vNums );
        return -1;
    }
    if ( nRegs != nRegsNtk )
    {
        printf( "ERROR: Register number not coresponding to Ntk.\n" );
        Vec_IntFree( vNums );
        return -1;
    }
    if ( Vec_IntSize(vNums)-nRegs == 0 )
    {
        printf( "ERROR: Cannot read counter example.\n" );
        Vec_IntFree( vNums );
        return -1;
    }
    if ( (Vec_IntSize(vNums)-nRegs) % (iFrameCex + 1) != 0 )
    {
        printf( "ERROR: Incorrect number of bits.\n" );
        Vec_IntFree( vNums );
        return -1;
    }
    int nPi = (Vec_IntSize(vNums)-nRegs)/(iFrameCex + 1);
    if ( nPi != Abc_NtkPiNum(pNtk) )
    {
        printf( "ERROR: Number of primary inputs not coresponding to Ntk.\n" );
        Vec_IntFree( vNums );
        return -1;
    }
    if ( iPo >= Abc_NtkPoNum(pNtk) )
    {
        printf( "WARNING: PO that failed verification not coresponding to Ntk, using first PO instead.\n" );
        iPo = 0;
    }
    Abc_NtkForEachLatch( pNtk, pObj, i ) {
        if ( Vec_IntEntry(vNums, i) == 1 )
            Abc_LatchSetInit1(pObj);
        else if ( Vec_IntEntry(vNums, i) == 2 && xMode )
            Abc_LatchSetInitNone(pObj);
        else
            Abc_LatchSetInit0(pObj);
    }

    pCex = Abc_CexAlloc( nRegs, nPi, iFrameCex + 1 );
    pCexCare = Abc_CexAlloc( nRegs, nPi, iFrameCex + 1);
    // the zero-based number of PO, for which verification failed
    // fails in Bmc_CexVerify if not less than actual number of PO
    pCex->iPo    = iPo;
    pCexCare->iPo = iPo;
    // the zero-based number of the time-frame, for which verificaiton failed
    pCex->iFrame = iFrameCex;
    pCexCare->iFrame = iFrameCex;
    assert( Vec_IntSize(vNums) == pCex->nBits );
    for ( c = 0; c < pCex->nBits; c++ ) {
        if ( Vec_IntEntry(vNums, c) == 1)
        {
            Abc_InfoSetBit( pCex->pData, c );
            Abc_InfoSetBit( pCexCare->pData, c );
        }
        else if ( Vec_IntEntry(vNums, c) == 2 && xMode )
        {
            // nothing to set
        }
        else
            Abc_InfoSetBit( pCexCare->pData, c );
    }

    Vec_IntFree( vNums );
    Abc_CexFreeP( ppCex );
    if ( ppCex )
        *ppCex = pCex;
    else
        Abc_CexFree( pCex );
    Abc_CexFreeP( ppCexCare );
    if ( ppCexCare )
        *ppCexCare = pCexCare;
    else
        Abc_CexFree( pCexCare );

    if ( pnFrames )
        *pnFrames = nFrames;
    return status;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/


int IoCommandReadCex( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Abc_Ntk_t * pNtk;
    Abc_Cex_t * pCex = NULL;
    Abc_Cex_t * pCexCare = NULL;
    char * pFileName;
    FILE * pFile;
    int fCheck = 1;
    int fXMode = 0;
    int c;
    int fOldFormat = 0;
    int verified;

    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "cxh" ) ) != EOF )
    {
        switch ( c )
        {
            case 'c':
                fCheck ^= 1;
                break;
            case 'x':
                fXMode ^= 1;
                break;
            case 'h':
                goto usage;
            default:
                goto usage;
        }
    }
    if ( argc != globalUtilOptind + 1 )
        goto usage;

    // get the input file name
    pFileName = argv[globalUtilOptind];
    if ( (pFile = fopen( pFileName, "r" )) == NULL )
    {
        fprintf( pAbc->Err, "Cannot open input file \"%s\". \n", pFileName );
        return 1;
    }
    fclose( pFile );

    pNtk = pAbc->pNtkCur;
    if ( pNtk == NULL )
    {
        fprintf( pAbc->Out, "Empty network.\n" );
        return 0;
    }
    Abc_FrameClearVerifStatus( pAbc );
    pAbc->Status = Abc_NtkReadCexFile( pFileName, pNtk, &pCex, &pCexCare, &pAbc->nFrames, &fOldFormat, fXMode);
    if ( fOldFormat && !fCheck )
        printf( "WARNING: Old witness format detected and checking is disabled. Reading might have failed.\n" );

    if ( fCheck && pAbc->Status==1) {
        extern Aig_Man_t * Abc_NtkToDar( Abc_Ntk_t * pNtk, int fExors, int fRegisters );
        Aig_Man_t * pAig = Abc_NtkToDar( pNtk, 0, 1 );

        verified = Bmc_CexCareVerify( pAig, pCex, pCexCare, false );
        if (!verified)
        {
            printf( "Checking CEX for any PO.\n" );
            int verified = Bmc_CexCareVerifyAnyPo( pAig, pCex, pCexCare, false );
            Aig_ManStop( pAig );
            if (verified < 0)
            {
                Abc_CexFreeP(&pCex);
                Abc_CexFreeP(&pCexCare);
                return 1;
            }
            pAbc->pCex->iPo = verified;
        }

        Abc_CexFreeP(&pCexCare);
        Abc_FrameReplaceCex( pAbc, &pCex );
    }
    return 0;

usage:
    fprintf( pAbc->Err, "usage: read_cex [-ch] <file>\n" );
    fprintf( pAbc->Err, "\t         reads the witness cex\n" );
    fprintf( pAbc->Err, "\t-c     : toggle check after reading [default = %s]\n", fCheck? "yes":"no" );
    fprintf( pAbc->Err, "\t-x     : read x bits for verification [default = %s]\n", fXMode? "yes":"no" );
    fprintf( pAbc->Err, "\t-h     : prints the command summary\n" );
    fprintf( pAbc->Err, "\tfile   : the name of a file to read\n" );
    return 1;
}
/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int IoCommandReadDsd( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Abc_Ntk_t * pNtk;
    char * pString;
    int fCheck;
    int c;
    extern Abc_Ntk_t * Io_ReadDsd( char * pFormula );

    fCheck = 1;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "ch" ) ) != EOF )
    {
        switch ( c )
        {
            case 'c':
                fCheck ^= 1;
                break;
            case 'h':
                goto usage;
            default:
                goto usage;
        }
    }
    if ( argc != globalUtilOptind + 1 )
        goto usage;
    // get the input file name
    pString = argv[globalUtilOptind];
    // read the file using the corresponding file reader
    pNtk = Io_ReadDsd( pString );
    if ( pNtk == NULL )
        return 1;
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtk );
    Abc_FrameClearVerifStatus( pAbc );
    return 0;

usage:
    fprintf( pAbc->Err, "usage: read_dsd [-h] <formula>\n" );
    fprintf( pAbc->Err, "\t          parses a formula representing DSD of a function\n" );
    fprintf( pAbc->Err, "\t-h      : prints the command summary\n" );
    fprintf( pAbc->Err, "\tformula : the formula representing disjoint-support decomposition (DSD)\n" );
    fprintf( pAbc->Err, "\t          Example of a formula: !(a*(b+CA(!d,e*f,c))*79B3(g,h,i,k))\n" );
    fprintf( pAbc->Err, "\t          where \'!\' is an INV, \'*\' is an AND, \'+\' is an XOR, \n" );
    fprintf( pAbc->Err, "\t          CA and 79B3 are hexadecimal representations of truth tables\n" );
    fprintf( pAbc->Err, "\t          (in this case CA=11001010 is truth table of MUX(Data0,Data1,Ctrl))\n" );
    fprintf( pAbc->Err, "\t          The lower chars (a,b,c,etc) are reserved for elementary variables.\n" );
    fprintf( pAbc->Err, "\t          The upper chars (A,B,C,etc) are reserved for hexadecimal digits.\n" );
    fprintf( pAbc->Err, "\t          No spaces are allowed in formulas. In parentheses, LSB goes first.\n" );
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int IoCommandReadEdif( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Abc_Ntk_t * pNtk;
    char * pFileName;
    int fCheck;
    int c;

    fCheck = 1;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "ch" ) ) != EOF )
    {
        switch ( c )
        {
            case 'c':
                fCheck ^= 1;
                break;
            case 'h':
                goto usage;
            default:
                goto usage;
        }
    }
    if ( argc != globalUtilOptind + 1 )
        goto usage;
    // get the input file name
    pFileName = argv[globalUtilOptind];
    // read the file using the corresponding file reader
    pNtk = Io_Read( pFileName, IO_FILE_EDIF, fCheck, 0 );
    if ( pNtk == NULL )
        return 1;
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtk );
    Abc_FrameClearVerifStatus( pAbc );
    return 0;

usage:
    fprintf( pAbc->Err, "usage: read_edif [-ch] <file>\n" );
    fprintf( pAbc->Err, "\t         reads the network in EDIF (works only for ISCAS benchmarks)\n" );
    fprintf( pAbc->Err, "\t-c     : toggle network check after reading [default = %s]\n", fCheck? "yes":"no" );
    fprintf( pAbc->Err, "\t-h     : prints the command summary\n" );
    fprintf( pAbc->Err, "\tfile   : the name of a file to read\n" );
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int IoCommandReadEqn( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Abc_Ntk_t * pNtk;
    char * pFileName;
    int fCheck;
    int c;

    fCheck = 1;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "ch" ) ) != EOF )
    {
        switch ( c )
        {
            case 'c':
                fCheck ^= 1;
                break;
            case 'h':
                goto usage;
            default:
                goto usage;
        }
    }
    if ( argc != globalUtilOptind + 1 )
        goto usage;
    // get the input file name
    pFileName = argv[globalUtilOptind];
    // read the file using the corresponding file reader
    pNtk = Io_Read( pFileName, IO_FILE_EQN, fCheck, 0 );
    if ( pNtk == NULL )
        return 1;
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtk );
    Abc_FrameClearVerifStatus( pAbc );
    return 0;

usage:
    fprintf( pAbc->Err, "usage: read_eqn [-ch] <file>\n" );
    fprintf( pAbc->Err, "\t         reads the network in equation format\n" );
    fprintf( pAbc->Err, "\t-c     : toggle network check after reading [default = %s]\n", fCheck? "yes":"no" );
    fprintf( pAbc->Err, "\t-h     : prints the command summary\n" );
    fprintf( pAbc->Err, "\tfile   : the name of a file to read\n" );
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int IoCommandReadFins( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    extern Vec_Int_t * Io_ReadFins( Abc_Ntk_t * pNtk, char * pFileName, int fVerbose );
    Abc_Ntk_t * pNtk = Abc_FrameReadNtk(pAbc);
    char * pFileName;
    int c, fVerbose = 0;

    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "vh" ) ) != EOF )
    {
        switch ( c )
        {
            case 'v':
                fVerbose ^= 1;
                break;
            case 'h':
                goto usage;
            default:
                goto usage;
        }
    }
    if ( argc != globalUtilOptind + 1 )
        goto usage;
    // get the input file name
    pFileName = argv[globalUtilOptind];
    // check current network
    if ( pNtk == NULL )
    {
        Abc_Print( -1, "Empty network.\n" );
        return 1;
    }
    // compute information and save it in the network
    Vec_IntFreeP( &pNtk->vFins );
    pNtk->vFins = Io_ReadFins( pNtk, pFileName, fVerbose );
    return 0;

usage:
    fprintf( pAbc->Err, "usage: read_fins [-vh] <file>\n" );
    fprintf( pAbc->Err, "\t         reads the network in equation format\n" );
    fprintf( pAbc->Err, "\t-v     : enable verbose output [default = %s].\n", fVerbose? "yes": "no" );
    fprintf( pAbc->Err, "\t-h     : prints the command summary\n" );
    fprintf( pAbc->Err, "\tfile   : the name of a file to read\n" );
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int IoCommandReadInit( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    char * pFileName;
    int c;

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "h" ) ) != EOF )
    {
        switch ( c )
        {
            case 'h':
                goto usage;
            default:
                goto usage;
        }
    }
    if ( argc != globalUtilOptind && argc != globalUtilOptind + 1 )
        goto usage;

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    // get the input file name
    if ( argc == globalUtilOptind + 1 )
        pFileName = argv[globalUtilOptind];
    else if ( pNtk->pSpec )
        pFileName = Extra_FileNameGenericAppend( pNtk->pSpec, ".init" );
    else
    {
        printf( "File name should be given on the command line.\n" );
        return 1;
    }

    // read the file using the corresponding file reader
    pNtk = Abc_NtkDup( pNtk );
    Io_ReadBenchInit( pNtk, pFileName );
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtk );
    Abc_FrameClearVerifStatus( pAbc );
    return 0;

usage:
    fprintf( pAbc->Err, "usage: read_init [-h] <file>\n" );
    fprintf( pAbc->Err, "\t         reads initial state of the network in BENCH format\n" );
    fprintf( pAbc->Err, "\t-h     : prints the command summary\n" );
    fprintf( pAbc->Err, "\tfile   : the name of a file to read\n" );
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int IoCommandReadPla( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Abc_Ntk_t * pNtk;
    char * pFileName;
    int c, fZeros = 0, fBoth = 0, fOnDc = 0, fSkipPrepro = 0, fCheck = 1;

    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "zbdxch" ) ) != EOF )
    {
        switch ( c )
        {
            case 'z':
                fZeros ^= 1;
                break;
            case 'b':
                fBoth ^= 1;
                break;
            case 'd':
                fOnDc ^= 1;
                break;
            case 'x':
                fSkipPrepro ^= 1;
                break;
            case 'c':
                fCheck ^= 1;
                break;
            case 'h':
                goto usage;
            default:
                goto usage;
        }
    }
    if ( argc != globalUtilOptind + 1 )
        goto usage;
    // get the input file name
    pFileName = argv[globalUtilOptind];
    // read the file using the corresponding file reader
    if ( fZeros || fBoth || fOnDc || fSkipPrepro )
    {
        Abc_Ntk_t * pTemp;
        pNtk = Io_ReadPla( pFileName, fZeros, fBoth, fOnDc, fSkipPrepro, fCheck );
        if ( pNtk == NULL )
        {
            printf( "Reading PLA file has failed.\n" );
            return 1;
        }
        pNtk = Abc_NtkToLogic( pTemp = pNtk );
        Abc_NtkDelete( pTemp );
    }
    else
        pNtk = Io_Read( pFileName, IO_FILE_PLA, fCheck, 0 );
    if ( pNtk == NULL )
        return 1;
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtk );
    Abc_FrameClearVerifStatus( pAbc );
    return 0;

usage:
    fprintf( pAbc->Err, "usage: read_pla [-zbdxch] <file>\n" );
    fprintf( pAbc->Err, "\t         reads the network in PLA\n" );
    fprintf( pAbc->Err, "\t-z     : toggle reading on-set and off-set [default = %s]\n", fZeros? "off-set":"on-set" );
    fprintf( pAbc->Err, "\t-b     : toggle reading both on-set and off-set as on-set [default = %s]\n", fBoth? "off-set":"on-set" );
    fprintf( pAbc->Err, "\t-d     : toggle reading both on-set and dc-set as on-set [default = %s]\n", fOnDc? "off-set":"on-set" );
    fprintf( pAbc->Err, "\t-x     : toggle reading Exclusive SOP rather than SOP [default = %s]\n", fSkipPrepro? "yes":"no" );
    fprintf( pAbc->Err, "\t-c     : toggle network check after reading [default = %s]\n", fCheck? "yes":"no" );
    fprintf( pAbc->Err, "\t-h     : prints the command summary\n" );
    fprintf( pAbc->Err, "\tfile   : the name of a file to read\n\n" );
    fprintf( pAbc->Err, "\t         Please note that the PLA parser is somewhat slow for large SOPs.\n" );
    fprintf( pAbc->Err, "\t         On the other hand, BLIF parser reads a 3M SOP and converts it into a 7.5K AIG in 1 sec:\n" );
    fprintf( pAbc->Err, "\t         abc 16> read test.blif; ps; bdd -s; ps; muxes; strash; ps\n" );
    fprintf( pAbc->Err, "\t         test                      : i/o =   25/    1  lat =    0  nd =     1  edge =     25  cube = 2910537  lev = 1\n" );
    fprintf( pAbc->Err, "\t         test                      : i/o =   25/    1  lat =    0  nd =     1  edge =     25  bdd  =    2937  lev = 1\n" );
    fprintf( pAbc->Err, "\t         test                      : i/o =   25/    1  lat =    0  and =     7514  lev = 48\n" );
    fprintf( pAbc->Err, "\t         abc 19> time\n" );
    fprintf( pAbc->Err, "\t         elapse: 1.05 seconds, total: 1.05 seconds\n" );
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int IoCommandReadPlaMo( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    extern Abc_Ntk_t * Mop_ManTest( char * pFileName, int fMerge, int fVerbose );
    Abc_Ntk_t * pNtk;
    int c, fMerge = 0, fVerbose = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "mvh" ) ) != EOF )
    {
        switch ( c )
        {
            case 'm':
                fMerge ^= 1;
                break;
            case 'v':
                fVerbose ^= 1;
                break;
            case 'h':
                goto usage;
            default:
                goto usage;
        }
    }
    if ( argc != globalUtilOptind + 1 )
        goto usage;
    // get the input file name
    pNtk = Mop_ManTest( argv[globalUtilOptind], fMerge, fVerbose );
    if ( pNtk == NULL )
        return 1;
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtk );
    Abc_FrameClearVerifStatus( pAbc );
    return 0;

usage:
    fprintf( pAbc->Err, "usage: read_plamo [-mvh] <file>\n" );
    fprintf( pAbc->Err, "\t         reads the network in multi-output PLA\n" );
    fprintf( pAbc->Err, "\t-m     : toggle dist-1 merge for cubes with identical outputs [default = %s]\n", fMerge? "yes":"no" );
    fprintf( pAbc->Err, "\t-v     : toggle printing verbose information [default = %s]\n", fVerbose? "yes":"no" );
    fprintf( pAbc->Err, "\t-h     : prints the command summary\n" );
    fprintf( pAbc->Err, "\tfile   : the name of a file to read\n" );
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int IoCommandReadTruth( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Abc_Ntk_t * pNtk;
    char * pStr = NULL;
    Vec_Ptr_t * vSops;
    int fHex  = 1;
    int fFile = 0;
    int c;

    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "xfh" ) ) != EOF )
    {
        switch ( c )
        {
            case 'x':
                fHex ^= 1;
                break;
            case 'f':
                fFile ^= 1;
                break;
            case 'h':
                goto usage;
            default:
                goto usage;
        }
    }

    if ( argc != globalUtilOptind + 1 )
        goto usage;

    if ( fFile )
    {
        FILE * pFile = fopen( argv[globalUtilOptind], "rb" );
        if ( pFile == NULL )
        {
            printf( "The file \"%s\" cannot be found.\n", argv[globalUtilOptind] );
            return 1;
        }
        else 
            fclose( pFile );
        pStr = Extra_FileReadContents( argv[globalUtilOptind] );
    }
    else
        pStr = argv[globalUtilOptind];

    // convert truth table to SOP
    if ( fHex )
        vSops = Abc_SopFromTruthsHex(pStr);
    else
        vSops = Abc_SopFromTruthsBin(pStr);
    if ( fFile )
        ABC_FREE( pStr );
    if ( Vec_PtrSize(vSops) == 0 )
    {
        Vec_PtrFreeFree( vSops );
        fprintf( pAbc->Err, "Reading truth table has failed.\n" );
        return 1;
    }

    pNtk = Abc_NtkCreateWithNodes( vSops );
    Vec_PtrFreeFree( vSops );
    if ( pNtk == NULL )
    {
        fprintf( pAbc->Err, "Deriving the network has failed.\n" );
        return 1;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtk );
    Abc_FrameClearVerifStatus( pAbc );
    return 0;

usage:
    fprintf( pAbc->Err, "usage: read_truth [-xfh] <truth> <file>\n" );
    fprintf( pAbc->Err, "\t         creates network with node(s) having given truth table(s)\n" );
    fprintf( pAbc->Err, "\t-x     : toggles between bin and hex notation [default = %s]\n", fHex?  "hex":"bin" );
    fprintf( pAbc->Err, "\t-f     : toggles reading truth table(s) from file [default = %s]\n",      fFile? "yes": "no" );
    fprintf( pAbc->Err, "\t-h     : prints the command summary\n" );
    fprintf( pAbc->Err, "\ttruth  : truth table with most significant bit first (e.g. 1000 for AND(a,b))\n" );
    fprintf( pAbc->Err, "\tfile   : file name with the truth table\n" );
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int IoCommandReadCnf( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    extern Vec_Ptr_t * Io_FileReadCnf( char * pFileName, int fMulti );
    FILE * pFile;
    Abc_Ntk_t * pNtk;
    Vec_Ptr_t * vSops;
    int fMulti = 0;
    int c;

    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "mh" ) ) != EOF )
    {
        switch ( c )
        {
            case 'm':
                fMulti ^= 1;
                break;            
            case 'h':
                goto usage;
            default:
                goto usage;
        }
    }

    if ( argc != globalUtilOptind + 1 )
        goto usage;

    pFile = fopen( argv[globalUtilOptind], "rb" );
    if ( pFile == NULL )
    {
        printf( "The file \"%s\" cannot be found.\n", argv[globalUtilOptind] );
        return 1;
    }
    else 
        fclose( pFile );
    vSops = Io_FileReadCnf( argv[globalUtilOptind], fMulti );
    if ( Vec_PtrSize(vSops) == 0 )
    {
        Vec_PtrFreeFree( vSops );
        fprintf( pAbc->Err, "Reading CNF file has failed.\n" );
        return 1;
    }
    pNtk = Abc_NtkCreateWithNodes( vSops );
    Vec_PtrFreeFree( vSops );
    if ( pNtk == NULL )
    {
        fprintf( pAbc->Err, "Deriving the network has failed.\n" );
        return 1;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtk );
    Abc_FrameClearVerifStatus( pAbc );
    return 0;

usage:
    fprintf( pAbc->Err, "usage: read_cnf [-mh] <file>\n" );
    fprintf( pAbc->Err, "\t         creates network with one node\n" );
    fprintf( pAbc->Err, "\t-m     : toggles generating multi-output network [default = %s]\n", fMulti?  "yes":"no" );    
    fprintf( pAbc->Err, "\t-h     : prints the command summary\n" );
    fprintf( pAbc->Err, "\tfile   : file name with the truth table\n" );
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int IoCommandReadVerilog( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Abc_Ntk_t * pNtk;
    char * pFileName;
    int fCheck, fBarBufs;
    int c;

    fCheck = 1;
    fBarBufs = 0;
    glo_fMapped = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "mcbh" ) ) != EOF )
    {
        switch ( c )
        {
            case 'm':
                glo_fMapped ^= 1;
                break;
            case 'c':
                fCheck ^= 1;
                break;
            case 'b':
                fBarBufs ^= 1;
                break;
            case 'h':
                goto usage;
            default:
                goto usage;
        }
    }
    if ( argc != globalUtilOptind + 1 )
        goto usage;
    // get the input file name
    pFileName = argv[globalUtilOptind];
    // read the file using the corresponding file reader
    pNtk = Io_Read( pFileName, IO_FILE_VERILOG, fCheck, fBarBufs );
    if ( pNtk == NULL )
        return 1;
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtk );
    Abc_FrameClearVerifStatus( pAbc );
    return 0;

usage:
    fprintf( pAbc->Err, "usage: read_verilog [-mcbh] <file>\n" );
    fprintf( pAbc->Err, "\t         reads the network in Verilog (IWLS 2002/2005 subset)\n" );
    fprintf( pAbc->Err, "\t-m     : toggle reading mapped Verilog [default = %s]\n", glo_fMapped? "yes":"no" );
    fprintf( pAbc->Err, "\t-c     : toggle network check after reading [default = %s]\n", fCheck? "yes":"no" );
    fprintf( pAbc->Err, "\t-b     : toggle reading barrier buffers [default = %s]\n", fBarBufs? "yes":"no" );
    fprintf( pAbc->Err, "\t-h     : prints the command summary\n" );
    fprintf( pAbc->Err, "\tfile   : the name of a file to read\n" );
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int IoCommandReadStatus( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    char * pFileName;
    FILE * pFile;
    int c;
    extern int Abc_NtkReadLogFile( char * pFileName, Abc_Cex_t ** ppCex, int * pnFrames );

    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "h" ) ) != EOF )
    {
        switch ( c )
        {
            case 'h':
                goto usage;
            default:
                goto usage;
        }
    }
    if ( argc != globalUtilOptind + 1 )
    {
        goto usage;
    }

    // get the input file name
    pFileName = argv[globalUtilOptind];
    if ( (pFile = fopen( pFileName, "r" )) == NULL )
    {
        fprintf( pAbc->Err, "Cannot open input file \"%s\". \n", pFileName );
        return 1;
    }
    fclose( pFile );

    // set the new network
    Abc_FrameClearVerifStatus( pAbc );
    pAbc->Status = Abc_NtkReadLogFile( pFileName, &pAbc->pCex, &pAbc->nFrames );
    return 0;

usage:
    fprintf( pAbc->Err, "usage: read_status [-ch] <file>\n" );
    fprintf( pAbc->Err, "\t         reads verification log file\n" );
    fprintf( pAbc->Err, "\t-h     : prints the command summary\n" );
    fprintf( pAbc->Err, "\tfile   : the name of a file to read\n" );
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int IoCommandReadGig( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    extern Gia_Man_t * Gia_ManReadGig( char * pFileName );
    Gia_Man_t * pAig;
    char * pFileName;
    FILE * pFile;
    int c;

    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "h" ) ) != EOF )
    {
        switch ( c )
        {
            case 'h':
                goto usage;
            default:
                goto usage;
        }
    }
    if ( argc != globalUtilOptind + 1 )
    {
        goto usage;
    }

    // get the input file name
    pFileName = argv[globalUtilOptind];
    if ( (pFile = fopen( pFileName, "r" )) == NULL )
    {
        fprintf( pAbc->Err, "Cannot open input file \"%s\". \n", pFileName );
        return 1;
    }
    fclose( pFile );

    // set the new network
    pAig = Gia_ManReadGig( pFileName );
    Abc_FrameUpdateGia( pAbc, pAig );
    return 0;

usage:
    fprintf( pAbc->Err, "usage: &read_gig [-h] <file>\n" );
    fprintf( pAbc->Err, "\t         reads design in GIG format\n" );
    fprintf( pAbc->Err, "\t-h     : prints the command summary\n" );
    fprintf( pAbc->Err, "\tfile   : the name of a file to read\n" );
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int IoCommandReadJson( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    extern Vec_Wec_t * Json_Read( char * pFileName, Abc_Nam_t ** ppStrs );
    Vec_Wec_t * vObjs;
    Abc_Nam_t * pStrs;
    char * pFileName;
    FILE * pFile;
    int c;

    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "h" ) ) != EOF )
    {
        switch ( c )
        {
            case 'h':
                goto usage;
            default:
                goto usage;
        }
    }
    if ( argc != globalUtilOptind + 1 )
    {
        goto usage;
    }

    // get the input file name
    pFileName = argv[globalUtilOptind];
    if ( (pFile = fopen( pFileName, "r" )) == NULL )
    {
        fprintf( pAbc->Err, "Cannot open input file \"%s\". \n", pFileName );
        return 1;
    }
    fclose( pFile );

    // set the new network
    vObjs = Json_Read( pFileName, &pStrs );
    if ( vObjs == NULL )
        return 0;
    Abc_FrameSetJsonStrs( pStrs );
    Abc_FrameSetJsonObjs( vObjs );
    return 0;

usage:
    fprintf( pAbc->Err, "usage: read_json [-h] <file>\n" );
    fprintf( pAbc->Err, "\t         reads file in JSON format\n" );
    fprintf( pAbc->Err, "\t-h     : prints the command summary\n" );
    fprintf( pAbc->Err, "\tfile   : the name of a file to read\n" );
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int IoCommandReadJsonc( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    extern int Jsonc_ReadTest( char * pFileName );
    char * pFileName;
    FILE * pFile;
    int c;

    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "h" ) ) != EOF )
    {
        switch ( c )
        {
            case 'h':
                goto usage;
            default:
                goto usage;
        }
    }
    if ( argc != globalUtilOptind + 1 )
    {
        goto usage;
    }

    // get the input file name
    pFileName = argv[globalUtilOptind];
    if ( (pFile = fopen( pFileName, "r" )) == NULL )
    {
        fprintf( pAbc->Err, "Cannot open input file \"%s\". \n", pFileName );
        return 1;
    }
    fclose( pFile );

    Jsonc_ReadTest( pFileName );
    return 0;

usage:
    fprintf( pAbc->Err, "usage: read_jsonc [-h] <file>\n" );
    fprintf( pAbc->Err, "\t         reads file in JSON format\n" );
    fprintf( pAbc->Err, "\t-h     : prints the command summary\n" );
    fprintf( pAbc->Err, "\tfile   : the name of a file to read\n" );
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int IoCommandReadSF( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    extern void Io_TransformSF2PLA( char * pNameIn, char * pNameOut );

    Abc_Ntk_t * pNtk;
    FILE * pFile;
    char * pFileName, * pFileTemp = "_temp_sf_.pla";
    int c;

    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "h" ) ) != EOF )
    {
        switch ( c )
        {
            case 'h':
                goto usage;
            default:
                goto usage;
        }
    }
    if ( argc != globalUtilOptind + 1 )
    {
        goto usage;
    }

    // get the input file name
    pFileName = argv[globalUtilOptind];
    if ( (pFile = fopen( pFileName, "r" )) == NULL )
    {
        fprintf( pAbc->Err, "Cannot open input file \"%s\". \n", pFileName );
        return 1;
    }
    fclose( pFile );
    Io_TransformSF2PLA( pFileName, pFileTemp );
    pNtk = Io_Read( pFileTemp, IO_FILE_PLA, 1, 0 );
    unlink( pFileTemp );
    if ( pNtk == NULL )
        return 1;
    ABC_FREE( pNtk->pName );
    pNtk->pName = Extra_FileNameGeneric( pFileName );
    ABC_FREE( pNtk->pSpec );
    pNtk->pSpec = Abc_UtilStrsav( pFileName );
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtk );
    Abc_FrameClearVerifStatus( pAbc );

    return 0;

usage:
    fprintf( pAbc->Err, "usage: read_sf [-h] <file>\n" );
    fprintf( pAbc->Err, "\t         reads file in SF format\n" );
    fprintf( pAbc->Err, "\t-h     : prints the command summary\n" );
    fprintf( pAbc->Err, "\tfile   : the name of a file to read\n" );
    return 1;
}


/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int IoCommandReadRom( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    extern void Io_TransformROM2PLA( char * pNameIn, char * pNameOut );

    Abc_Ntk_t * pNtk;
    FILE * pFile;
    char * pFileName, * pFileTemp = "_temp_rom_.pla";
    int c;

    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "h" ) ) != EOF )
    {
        switch ( c )
        {
            case 'h':
                goto usage;
            default:
                goto usage;
        }
    }
    if ( argc != globalUtilOptind + 1 )
    {
        goto usage;
    }

    // get the input file name
    pFileName = argv[globalUtilOptind];
    if ( (pFile = fopen( pFileName, "r" )) == NULL )
    {
        fprintf( pAbc->Err, "Cannot open input file \"%s\". \n", pFileName );
        return 1;
    }
    fclose( pFile );
    Io_TransformROM2PLA( pFileName, pFileTemp );
    pNtk = Io_Read( pFileTemp, IO_FILE_PLA, 1, 0 );
    //unlink( pFileTemp );
    if ( pNtk == NULL )
        return 1;
    ABC_FREE( pNtk->pName );
    pNtk->pName = Extra_FileNameGeneric( pFileName );
    ABC_FREE( pNtk->pSpec );
    pNtk->pSpec = Abc_UtilStrsav( pFileName );
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtk );
    Abc_FrameClearVerifStatus( pAbc );

    return 0;

usage:
    fprintf( pAbc->Err, "usage: read_rom [-h] <file>\n" );
    fprintf( pAbc->Err, "\t         reads ROM file\n" );
    fprintf( pAbc->Err, "\t-h     : prints the command summary\n" );
    fprintf( pAbc->Err, "\tfile   : the name of a file to read\n" );
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int IoCommandReadMM( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    extern Abc_Ntk_t * Abc_NtkReadFromFile( char * pFileName );
    Abc_Ntk_t * pNtk; char * pFileName; int c;

    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "h" ) ) != EOF )
    {
        switch ( c )
        {
            case 'h':
                goto usage;
            default:
                goto usage;
        }
    }
    if ( argc != globalUtilOptind + 1 )
        goto usage;
    pFileName = argv[globalUtilOptind];
    pNtk = Abc_NtkReadFromFile( pFileName );
    if ( pNtk == NULL )
        return 0;
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtk );
    Abc_FrameClearVerifStatus( pAbc );
    return 0;

usage:
    fprintf( pAbc->Err, "usage: read_mm [-h] <file>\n" );
    fprintf( pAbc->Err, "\t         reads mapped network from file\n" );
    fprintf( pAbc->Err, "\t-h     : prints the command summary\n" );
    fprintf( pAbc->Err, "\tfile   : the name of a file to read\n" );
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int IoCommandReadMMGia( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    extern Abc_Ntk_t * Abc_NtkReadFromFile( char * pFileName );
    Abc_Ntk_t * pNtk; char * pFileName; int c;
    Gia_Man_t * pGia = NULL;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "h" ) ) != EOF )
    {
        switch ( c )
        {
            case 'h':
                goto usage;
            default:
                goto usage;
        }
    }
    if ( argc != globalUtilOptind + 1 )
        goto usage;
    pFileName = argv[globalUtilOptind];
    pNtk = Abc_NtkReadFromFile( pFileName );
    if ( pNtk == NULL )
        return 0;
    Abc_NtkToAig( pNtk );
    pGia = Abc_NtkAigToGia( pNtk, 0 );
    Abc_NtkDelete( pNtk );
    Abc_FrameUpdateGia( pAbc, pGia );
    Abc_FrameClearVerifStatus( pAbc );
    return 0;

usage:
    fprintf( pAbc->Err, "usage: &read_mm [-h] <file>\n" );
    fprintf( pAbc->Err, "\t         reads mapped network from file\n" );
    fprintf( pAbc->Err, "\t-h     : prints the command summary\n" );
    fprintf( pAbc->Err, "\tfile   : the name of a file to read\n" );
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int IoCommandWrite( Abc_Frame_t * pAbc, int argc, char **argv )
{
    char Command[1000];
    char * pFileName;
    int c;

    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "h" ) ) != EOF )
    {
        switch ( c )
        {
            case 'h':
                goto usage;
            default:
                goto usage;
        }
    }
    if ( argc != globalUtilOptind + 1 )
        goto usage;
    // get the output file name
    pFileName = argv[globalUtilOptind];
    // write libraries
    Command[0] = 0;
    assert( strlen(pFileName) < 900 );
    if ( !strcmp( Extra_FileNameExtension(pFileName), "genlib" )  )
        sprintf( Command, "write_genlib %s", pFileName );
    else if ( !strcmp( Extra_FileNameExtension(pFileName), "lib" ) )
        sprintf( Command, "write_lib %s", pFileName );
    else if ( !strcmp( Extra_FileNameExtension(pFileName), "dsd" ) )
        sprintf( Command, "dsd_save %s", pFileName );
    if ( Command[0] )
    {
        Cmd_CommandExecute( pAbc, Command );
        return 0;
    }
    if ( pAbc->pNtkCur == NULL )
    {
        fprintf( pAbc->Out, "Empty network.\n" );
        return 0;
    }
    // call the corresponding file writer
    Io_Write( pAbc->pNtkCur, pFileName, Io_ReadFileType(pFileName) );
    return 0;

usage:
    fprintf( pAbc->Err, "usage: write [-h] <file>\n" );
    fprintf( pAbc->Err, "\t         writes the current network into <file> by calling\n" );
    fprintf( pAbc->Err, "\t         the writer that matches the extension of <file>\n" );
    fprintf( pAbc->Err, "\t-h     : print the help massage\n" );
    fprintf( pAbc->Err, "\tfile   : the name of the file to write\n" );
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int IoCommandWriteHie( Abc_Frame_t * pAbc, int argc, char **argv )
{
    char * pBaseName, * pFileName;
    int c;

    glo_fMapped = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "mh" ) ) != EOF )
    {
        switch ( c )
        {
            case 'm':
                glo_fMapped ^= 1;
                break;
            case 'h':
                goto usage;
            default:
                goto usage;
        }
    }
    if ( pAbc->pNtkCur == NULL )
    {
        fprintf( pAbc->Out, "Empty network.\n" );
        return 0;
    }
    if ( argc != globalUtilOptind + 2 )
        goto usage;
    // get the output file name
    pBaseName = argv[globalUtilOptind];
    pFileName = argv[globalUtilOptind+1];
    // call the corresponding file writer
//    Io_Write( pAbc->pNtkCur, pFileName, Io_ReadFileType(pFileName) );
    Io_WriteHie( pAbc->pNtkCur, pBaseName, pFileName );
    return 0;

usage:
    fprintf( pAbc->Err, "usage: write_hie [-h] <orig> <file>\n" );
    fprintf( pAbc->Err, "\t         writes the current network into <file> by calling\n" );
    fprintf( pAbc->Err, "\t         the hierarchical writer that matches the extension of <file>\n" );
    fprintf( pAbc->Err, "\t-m     : toggle reading mapped Verilog for <orig> [default = %s]\n", glo_fMapped? "yes":"no" );
    fprintf( pAbc->Err, "\t-h     : print the help massage\n" );
    fprintf( pAbc->Err, "\torig   : the name of the original file with the hierarchical design\n" );
    fprintf( pAbc->Err, "\tfile   : the name of the file to write\n" );
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int IoCommandWriteAiger( Abc_Frame_t * pAbc, int argc, char **argv )
{
    char * pFileName;
    int fWriteSymbols;
    int fCompact;
    int fUnique;
    int fVerbose;
    int c;

    fWriteSymbols = 0;
    fCompact      = 0;
    fUnique       = 0;
    fVerbose      = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "scuvh" ) ) != EOF )
    {
        switch ( c )
        {
            case 's':
                fWriteSymbols ^= 1;
                break;
            case 'c':
                fCompact ^= 1;
                break;
            case 'u':
                fUnique ^= 1;
                break;
            case 'v':
                fVerbose ^= 1;
                break;
            case 'h':
                goto usage;
            default:
                goto usage;
        }
    }
    if ( pAbc->pNtkCur == NULL )
    {
        fprintf( pAbc->Out, "Empty network.\n" );
        return 0;
    }
    if ( argc != globalUtilOptind + 1 )
        goto usage;
    // get the output file name
    pFileName = argv[globalUtilOptind];
    // call the corresponding file writer
    if ( !Abc_NtkIsStrash(pAbc->pNtkCur) )
    {
        fprintf( stdout, "Writing this format is only possible for structurally hashed AIGs.\n" );
        return 1;
    }
    if ( fUnique )
    {
        extern Aig_Man_t * Abc_NtkToDar( Abc_Ntk_t * pNtk, int fExors, int fRegisters );
        extern Abc_Ntk_t * Abc_NtkFromAigPhase( Aig_Man_t * pMan );
        Aig_Man_t * pAig = Abc_NtkToDar( pAbc->pNtkCur, 0, 1 );
        Aig_Man_t * pCan = Saig_ManDupIsoCanonical( pAig, fVerbose );
        Abc_Ntk_t * pTemp = Abc_NtkFromAigPhase( pCan );
        Aig_ManStop( pCan );
        Aig_ManStop( pAig );
        Io_WriteAiger( pTemp, pFileName, fWriteSymbols, fCompact, fUnique );
        Abc_NtkDelete( pTemp );
    }
    else
        Io_WriteAiger( pAbc->pNtkCur, pFileName, fWriteSymbols, fCompact, fUnique );
    return 0;

usage:
    fprintf( pAbc->Err, "usage: write_aiger [-scuvh] <file>\n" );
    fprintf( pAbc->Err, "\t         writes the network in the AIGER format (http://fmv.jku.at/aiger)\n" );
    fprintf( pAbc->Err, "\t-s     : toggle saving I/O names [default = %s]\n", fWriteSymbols? "yes" : "no" );
    fprintf( pAbc->Err, "\t-c     : toggle writing more compactly [default = %s]\n", fCompact? "yes" : "no" );
    fprintf( pAbc->Err, "\t-u     : toggle writing canonical AIG structure [default = %s]\n", fUnique? "yes" : "no" );
    fprintf( pAbc->Err, "\t-v     : toggle printing verbose information [default = %s]\n", fVerbose? "yes" : "no" );
    fprintf( pAbc->Err, "\t-h     : print the help massage\n" );
    fprintf( pAbc->Err, "\tfile   : the name of the file to write (extension .aig)\n" );
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int IoCommandWriteAigerCex( Abc_Frame_t * pAbc, int argc, char **argv )
{
    char * pFileName;
    int c;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "h" ) ) != EOF )
    {
        switch ( c )
        {
            case 'h':
                goto usage;
            default:
                goto usage;
        }
    }
    if ( pAbc->pCex == NULL )
    {
        fprintf( pAbc->Out, "There is no current CEX.\n" );
        return 0;
    }
    if ( argc != globalUtilOptind + 1 )
        goto usage;
    // get the output file name
    pFileName = argv[globalUtilOptind];
    Io_WriteAigerCex( pAbc->pCex, pAbc->pNtkCur, pAbc->pGia, pFileName );
    return 0;

usage:
    fprintf( pAbc->Err, "usage: write_aiger_cex [-h] <file>\n" );
    fprintf( pAbc->Err, "\t         writes the current CEX in the AIGER format (http://fmv.jku.at/aiger)\n" );
    fprintf( pAbc->Err, "\t-h     : print the help massage\n" );
    fprintf( pAbc->Err, "\tfile   : the name of the file to write\n" );
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int IoCommandWriteBaf( Abc_Frame_t * pAbc, int argc, char **argv )
{
    char * pFileName;
    int c;

    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "h" ) ) != EOF )
    {
        switch ( c )
        {
            case 'h':
                goto usage;
            default:
                goto usage;
        }
    }
    if ( pAbc->pNtkCur == NULL )
    {
        fprintf( pAbc->Out, "Empty network.\n" );
        return 0;
    }
    if ( argc != globalUtilOptind + 1 )
        goto usage;
    // get the output file name
    pFileName = argv[globalUtilOptind];
    // call the corresponding file writer
    Io_Write( pAbc->pNtkCur, pFileName, IO_FILE_BAF );
    return 0;

usage:
    fprintf( pAbc->Err, "usage: write_baf [-h] <file>\n" );
    fprintf( pAbc->Err, "\t         writes the network into a BLIF file\n" );
    fprintf( pAbc->Err, "\t-h     : print the help massage\n" );
    fprintf( pAbc->Err, "\tfile   : the name of the file to write (extension .baf)\n" );
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int IoCommandWriteBblif( Abc_Frame_t * pAbc, int argc, char **argv )
{
    char * pFileName;
    int c;

    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "h" ) ) != EOF )
    {
        switch ( c )
        {
            case 'h':
                goto usage;
            default:
                goto usage;
        }
    }
    if ( pAbc->pNtkCur == NULL )
    {
        fprintf( pAbc->Out, "Empty network.\n" );
        return 0;
    }
    if ( argc != globalUtilOptind + 1 )
        goto usage;
    // get the output file name
    pFileName = argv[globalUtilOptind];
    // call the corresponding file writer
    Io_Write( pAbc->pNtkCur, pFileName, IO_FILE_BBLIF );
    return 0;

usage:
    fprintf( pAbc->Err, "usage: write_bblif [-h] <file>\n" );
    fprintf( pAbc->Err, "\t         writes the network into a binary BLIF file\n" );
    fprintf( pAbc->Err, "\t-h     : print the help massage\n" );
    fprintf( pAbc->Err, "\tfile   : the name of the file to write (extension .bblif)\n" );
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int IoCommandWriteBlif( Abc_Frame_t * pAbc, int argc, char **argv )
{
    char * pFileName;
    char * pLutStruct = NULL;
    int c, fSpecial = 0;
    int fUseHie = 0;

    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "Sjah" ) ) != EOF )
    {
        switch ( c )
        {
            case 'S':
                if ( globalUtilOptind >= argc )
                {
                    Abc_Print( -1, "Command line switch \"-S\" should be followed by string.\n" );
                    goto usage;
                }
                pLutStruct = argv[globalUtilOptind];
                globalUtilOptind++;
                if ( strlen(pLutStruct) != 2 && strlen(pLutStruct) != 3 ) 
                {
                    Abc_Print( -1, "Command line switch \"-S\" should be followed by a 2- or 3-char string (e.g. \"44\" or \"555\").\n" );
                    goto usage;
                }
                break;
            case 'j':
                fSpecial ^= 1;
                break;
            case 'a':
                fUseHie ^= 1;
                break;
            case 'h':
                goto usage;
            default:
                goto usage;
        }
    }
    if ( pAbc->pNtkCur == NULL )
    {
        fprintf( pAbc->Out, "Empty network.\n" );
        return 0;
    }
    if ( argc != globalUtilOptind + 1 )
        goto usage;
    // get the output file name
    pFileName = argv[globalUtilOptind];
    // call the corresponding file writer
    if ( fSpecial || pLutStruct )
        Io_WriteBlifSpecial( pAbc->pNtkCur, pFileName, pLutStruct, fUseHie );
    else
        Io_Write( pAbc->pNtkCur, pFileName, IO_FILE_BLIF );
    return 0;

usage:
    fprintf( pAbc->Err, "usage: write_blif [-S str] [-jah] <file>\n" );
    fprintf( pAbc->Err, "\t         writes the network into a BLIF file\n" );
    fprintf( pAbc->Err, "\t-S str : string representing the LUT structure [default = %s]\n", pLutStruct ? pLutStruct : "not used" );  
    fprintf( pAbc->Err, "\t-j     : enables special BLIF writing [default = %s]\n", fSpecial? "yes" : "no" );;
    fprintf( pAbc->Err, "\t-a     : enables hierarchical BLIF writing for LUT structures [default = %s]\n", fUseHie? "yes" : "no" );;
    fprintf( pAbc->Err, "\t-h     : print the help massage\n" );
    fprintf( pAbc->Err, "\tfile   : the name of the file to write (extension .blif)\n" );
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int IoCommandWriteBlifMv( Abc_Frame_t * pAbc, int argc, char **argv )
{
    char * pFileName;
    int c;

    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "h" ) ) != EOF )
    {
        switch ( c )
        {
            case 'h':
                goto usage;
            default:
                goto usage;
        }
    }
    if ( pAbc->pNtkCur == NULL )
    {
        fprintf( pAbc->Out, "Empty network.\n" );
        return 0;
    }
    if ( argc != globalUtilOptind + 1 )
        goto usage;
    // get the output file name
    pFileName = argv[globalUtilOptind];
    // call the corresponding file writer
    Io_Write( pAbc->pNtkCur, pFileName, IO_FILE_BLIFMV );
    return 0;

usage:
    fprintf( pAbc->Err, "usage: write_blif_mv [-h] <file>\n" );
    fprintf( pAbc->Err, "\t         writes the network into a BLIF-MV file\n" );
    fprintf( pAbc->Err, "\t-h     : print the help massage\n" );
    fprintf( pAbc->Err, "\tfile   : the name of the file to write (extension .mv)\n" );
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int IoCommandWriteBench( Abc_Frame_t * pAbc, int argc, char **argv )
{
    char * pFileName;
    int fUseLuts;
    int c;

    fUseLuts = 1;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "lh" ) ) != EOF )
    {
        switch ( c )
        {
            case 'l':
                fUseLuts ^= 1;
                break;
            case 'h':
                goto usage;
            default:
                goto usage;
        }
    }
    if ( pAbc->pNtkCur == NULL )
    {
        fprintf( pAbc->Out, "Empty network.\n" );
        return 0;
    }
    if ( argc != globalUtilOptind + 1 )
        goto usage;
    // get the output file name
    pFileName = argv[globalUtilOptind];
    // call the corresponding file writer
    if ( !fUseLuts )
        Io_Write( pAbc->pNtkCur, pFileName, IO_FILE_BENCH );
    else if ( pAbc->pNtkCur )
    {
        Abc_Ntk_t * pNtkTemp;
        pNtkTemp = Abc_NtkToNetlist( pAbc->pNtkCur );
        Abc_NtkToAig( pNtkTemp );
        Io_WriteBenchLut( pNtkTemp, pFileName );
        Abc_NtkDelete( pNtkTemp );
    }
    else
        printf( "There is no current network.\n" );
    return 0;

usage:
    fprintf( pAbc->Err, "usage: write_bench [-lh] <file>\n" );
    fprintf( pAbc->Err, "\t         writes the network in BENCH format\n" );
    fprintf( pAbc->Err, "\t-l     : toggle using LUTs in the output [default = %s]\n", fUseLuts? "yes" : "no" );
    fprintf( pAbc->Err, "\t-h     : print the help massage\n" );
    fprintf( pAbc->Err, "\tfile   : the name of the file to write (extension .bench)\n" );
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int IoCommandWriteBook( Abc_Frame_t * pAbc, int argc, char **argv )
{
    char * pFileName;
        int c;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "h" ) ) != EOF )
    {
        switch ( c )
        {
            case 'h':
                goto usage;
            default:
                goto usage;
        }
    }
    if ( argc != globalUtilOptind + 1 )
        goto usage;
    // get the output file name
    pFileName = argv[globalUtilOptind];
        // call the corresponding file writer
    Io_Write( pAbc->pNtkCur, pFileName, IO_FILE_BOOK );
    return 0;

usage:
    fprintf( pAbc->Err, "usage: write_book [-h] <file> [-options]\n" );
    fprintf( pAbc->Err, "\t-h     : prints the help massage\n" );
    fprintf( pAbc->Err, "\tfile   : the name of the file to write (extension .aux, .nodes, .nets)\n" );
    fprintf( pAbc->Err, "\t\n" );
    fprintf( pAbc->Err, "\tThis command is developed by Myungchul Kim (University of Michigan).\n" );
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int IoCommandWriteCellNet( Abc_Frame_t * pAbc, int argc, char **argv )
{
    Abc_Ntk_t * pNtk;
    char * pFileName;
    int c;
    extern void Io_WriteCellNet( Abc_Ntk_t * pNtk, char * pFileName );

    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "h" ) ) != EOF )
    {
        switch ( c )
        {
            case 'h':
                goto usage;
            default:
                goto usage;
        }
    }
    if ( pAbc->pNtkCur == NULL )
    {
        fprintf( pAbc->Out, "Empty network.\n" );
        return 0;
    }
    if ( argc != globalUtilOptind + 1 )
        goto usage;
    pNtk = pAbc->pNtkCur;
    // get the output file name
    pFileName = argv[globalUtilOptind];
    // call the corresponding file writer
    if ( !Abc_NtkIsLogic(pNtk) )
    {
        fprintf( pAbc->Out, "The network should be a logic network (if it an AIG, use command \"logic\")\n" );
        return 0;
    }
    Io_WriteCellNet( pNtk, pFileName );
    return 0;

usage:
    fprintf( pAbc->Err, "usage: write_cellnet [-h] <file>\n" );
    fprintf( pAbc->Err, "\t         writes the network is the cellnet format\n" );
    fprintf( pAbc->Err, "\t-h     : print the help massage\n" );
    fprintf( pAbc->Err, "\tfile   : the name of the file to write\n" );
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int IoCommandWriteCnf( Abc_Frame_t * pAbc, int argc, char **argv )
{
    char * pFileName;
    int c;
    int fNewAlgo;
    int fFastAlgo;
    int fAllPrimes;
    int fChangePol;
    int fVerbose;
    extern Abc_Ntk_t * Abc_NtkDarToCnf( Abc_Ntk_t * pNtk, char * pFileName, int fFastAlgo, int fChangePol, int fVerbose );

    fNewAlgo = 1;
    fFastAlgo = 0;
    fAllPrimes = 0;
    fChangePol = 1;
    fVerbose = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "nfpcvh" ) ) != EOF )
    {
        switch ( c )
        {
            case 'n':
                fNewAlgo ^= 1;
                break;
            case 'f':
                fFastAlgo ^= 1;
                break;
            case 'p':
                fAllPrimes ^= 1;
                break;
            case 'c':
                fChangePol ^= 1;
                break;
            case 'v':
                fVerbose ^= 1;
                break;
            case 'h':
                goto usage;
            default:
                goto usage;
        }
    }
    if ( pAbc->pNtkCur == NULL )
    {
        fprintf( pAbc->Out, "Empty network.\n" );
        return 0;
    }
    if ( argc != globalUtilOptind + 1 )
        goto usage;
    // get the output file name
    pFileName = argv[globalUtilOptind];
    // check if the feature will be used
    if ( Abc_NtkIsStrash(pAbc->pNtkCur) && fAllPrimes )
    {
        fAllPrimes = 0;
        printf( "Warning: Selected option to write all primes has no effect when deriving CNF from AIG.\n" );
    }
    // call the corresponding file writer
    if ( fFastAlgo )
        Abc_NtkDarToCnf( pAbc->pNtkCur, pFileName, 1, fChangePol, fVerbose );
    else if ( fNewAlgo )
        Abc_NtkDarToCnf( pAbc->pNtkCur, pFileName, 0, fChangePol, fVerbose );
    else if ( fAllPrimes )
        Io_WriteCnf( pAbc->pNtkCur, pFileName, 1 );
    else
        Io_Write( pAbc->pNtkCur, pFileName, IO_FILE_CNF );
    return 0;

usage:
    fprintf( pAbc->Err, "usage: write_cnf [-nfpcvh] <file>\n" );
    fprintf( pAbc->Err, "\t         generates CNF for the miter (see also \"&write_cnf\")\n" );
    fprintf( pAbc->Err, "\t-n     : toggle using new algorithm [default = %s]\n", fNewAlgo? "yes" : "no" );
    fprintf( pAbc->Err, "\t-f     : toggle using fast algorithm [default = %s]\n", fFastAlgo? "yes" : "no" );
    fprintf( pAbc->Err, "\t-p     : toggle using all primes to enhance implicativity [default = %s]\n", fAllPrimes? "yes" : "no" );
    fprintf( pAbc->Err, "\t-c     : toggle adjasting polarity of internal variables [default = %s]\n", fChangePol? "yes" : "no" );
    fprintf( pAbc->Err, "\t-v     : toggle printing verbose information [default = %s]\n", fVerbose? "yes" : "no" );
    fprintf( pAbc->Err, "\t-h     : print the help massage\n" );
    fprintf( pAbc->Err, "\tfile   : the name of the file to write\n" );
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int IoCommandWriteCnf2( Abc_Frame_t * pAbc, int argc, char **argv )
{
    extern void Jf_ManDumpCnf( Gia_Man_t * p, char * pFileName, int fVerbose );
    extern void Mf_ManDumpCnf( Gia_Man_t * p, char * pFileName, int nLutSize, int fCnfObjIds, int fAddOrCla, int fVerbose );
    FILE * pFile;
    char * pFileName;
    int nLutSize    = 8;
    int fNewAlgo    = 1;
    int fCnfObjIds  = 0;
    int fAddOrCla   = 1;
    int c, fVerbose = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "Kaiovh" ) ) != EOF )
    {
        switch ( c )
        {
            case 'K':
                if ( globalUtilOptind >= argc )
                {
                    Abc_Print( -1, "Command line switch \"-K\" should be followed by an integer.\n" );
                    goto usage;
                }
                nLutSize = atoi(argv[globalUtilOptind]);
                globalUtilOptind++;
                break;
            case 'a':
                fNewAlgo ^= 1;
                break;
            case 'i':
                fCnfObjIds ^= 1;
                break;
            case 'o':
                fAddOrCla ^= 1;
                break;
            case 'v':
                fVerbose ^= 1;
                break;
            case 'h':
                goto usage;
            default:
                goto usage;
        }
    }
    if ( pAbc->pGia == NULL )
    {
        Abc_Print( -1, "IoCommandWriteCnf2(): There is no AIG.\n" );
        return 1;
    } 
    if ( Gia_ManRegNum(pAbc->pGia) > 0 )
    {
        Abc_Print( -1, "IoCommandWriteCnf2(): Works only for combinational miters.\n" );
        return 0;
    }
    if ( nLutSize < 3 || nLutSize > 8 )
    {
        Abc_Print( -1, "IoCommandWriteCnf2(): Invalid LUT size (%d).\n", nLutSize );
        return 0;
    }
    if ( !fNewAlgo && !Sdm_ManCanRead() )
    {
        Abc_Print( -1, "IoCommandWriteCnf2(): Cannot input precomputed DSD information.\n" );
        return 0;
    }
    if ( argc != globalUtilOptind + 1 )
        goto usage;
    // get the input file name
    pFileName = argv[globalUtilOptind];
    pFile = fopen( pFileName, "wb" );
    if ( pFile == NULL )
    {
        printf( "Cannot open file \"%s\" for writing.\n", pFileName );
        return 0;
    }
    fclose( pFile );
    if ( fNewAlgo )
        Mf_ManDumpCnf( pAbc->pGia, pFileName, nLutSize, fCnfObjIds, fAddOrCla, fVerbose );
    else
        Jf_ManDumpCnf( pAbc->pGia, pFileName, fVerbose );
    return 0;

usage:
    fprintf( pAbc->Err, "usage: &write_cnf [-Kaiovh] <file>\n" );
    fprintf( pAbc->Err, "\t           writes CNF produced by a new generator\n" );
    fprintf( pAbc->Err, "\t-K <num> : the LUT size (3 <= num <= 8) [default = %d]\n", nLutSize );
    fprintf( pAbc->Err, "\t-a       : toggle using new algorithm [default = %s]\n", fNewAlgo? "yes" : "no" );
    fprintf( pAbc->Err, "\t-i       : toggle using AIG object IDs as CNF variables [default = %s]\n", fCnfObjIds? "yes" : "no" );
    fprintf( pAbc->Err, "\t-o       : toggle adding OR clause for the outputs [default = %s]\n", fAddOrCla? "yes" : "no" );
    fprintf( pAbc->Err, "\t-v       : toggle printing verbose information [default = %s]\n", fVerbose? "yes" : "no" );
    fprintf( pAbc->Err, "\t-h       : print the help massage\n" );
    fprintf( pAbc->Err, "\tfile     : the name of the file to write\n" );
    fprintf( pAbc->Err, "\n" );
    fprintf( pAbc->Err, "\t           CNF variable mapping rules:\n" );
    fprintf( pAbc->Err, "\n" );
    fprintf( pAbc->Err, "\t           Assume CNF has N variables, with variable IDs running from 0 to N-1.\n" );
    fprintf( pAbc->Err, "\t           Variable number 0 is not used in the CNF.\n" );
    fprintf( pAbc->Err, "\t           Variables 1, 2, 3,... <nPOs> represent POs in their natural order.\n" );
    fprintf( pAbc->Err, "\t           Variables N-<nPIs>, N-<nPIs>+1, N-<nPIs>+2, ... N-1, represent PIs in their natural order.\n" );
    fprintf( pAbc->Err, "\t           The internal variables are ordered in a reverse topological order from outputs to inputs.\n" );
    fprintf( pAbc->Err, "\t           That is, smaller variable IDs tend to be closer to the outputs, while larger\n" );
    fprintf( pAbc->Err, "\t           variable IDs tend to be closer to the inputs. It was found that this ordering\n" );
    fprintf( pAbc->Err, "\t           leads to faster SAT solving for hard UNSAT CEC problems.\n" );
    return 1;
}


/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int IoCommandWriteDot( Abc_Frame_t * pAbc, int argc, char **argv )
{
    char * pFileName;
    int c;

    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "h" ) ) != EOF )
    {
        switch ( c )
        {
            case 'h':
                goto usage;
            default:
                goto usage;
        }
    }
    if ( pAbc->pNtkCur == NULL )
    {
        fprintf( pAbc->Out, "Empty network.\n" );
        return 0;
    }
    if ( argc != globalUtilOptind + 1 )
        goto usage;
    // get the output file name
    pFileName = argv[globalUtilOptind];
    // call the corresponding file writer
    Io_Write( pAbc->pNtkCur, pFileName, IO_FILE_DOT );
    return 0;

usage:
    fprintf( pAbc->Err, "usage: write_dot [-h] <file>\n" );
    fprintf( pAbc->Err, "\t         writes the current network into a DOT file\n" );
    fprintf( pAbc->Err, "\t-h     : print the help massage\n" );
    fprintf( pAbc->Err, "\tfile   : the name of the file to write\n" );
    return 1;
}

ABC_NAMESPACE_IMPL_END

#include "proof/fra/fra.h"

ABC_NAMESPACE_IMPL_START

int Abc_NtkCheckSpecialPi( Abc_Ntk_t * pNtk )
{
    Abc_Obj_t * pObj;  int i;
    Abc_NtkForEachPi( pNtk, pObj, i )
        if ( !strcmp(Abc_ObjName(pObj), "_abc_190121_abc_") )
            return 1;
    return 0;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Abc_NtkDumpOneCexSpecial( FILE * pFile, Abc_Ntk_t * pNtk, Abc_Cex_t * pCex )
{
    Abc_Cex_t * pCare = NULL; int i, f; Abc_Obj_t * pObj;
    extern Aig_Man_t * Abc_NtkToDar( Abc_Ntk_t * pNtk, int fExors, int fRegisters );
    Aig_Man_t * pAig = Abc_NtkToDar( pNtk, 0, 1 );
    //fprintf( pFile, "# FALSIFYING OUTPUTS:");                                       
    //fprintf( pFile, " %s", Abc_ObjName(Abc_NtkCo(pNtk, pCex->iPo)) ); 
    pCare = Bmc_CexCareMinimize( pAig, Saig_ManPiNum(pAig), pCex, 4, 0, 0 );
    Aig_ManStop( pAig );
    if( pCare == NULL )   
    {
        printf( "Counter-example minimization has failed.\n" ); 
        return;
    }
    // output flop values (unaffected by the minimization)
    Abc_NtkForEachLatch( pNtk, pObj, i )
        fprintf( pFile, "CEX: %s@0=%c\n", Abc_ObjName(Abc_ObjFanout0(pObj)), '0'+!Abc_LatchIsInit0(pObj) );
    // output PI values (while skipping the minimized ones)
    for ( f = 0; f <= pCex->iFrame; f++ )
        Abc_NtkForEachPi( pNtk, pObj, i )
            if ( !pCare || Abc_InfoHasBit(pCare->pData, pCare->nRegs+pCare->nPis*f + i) )
                fprintf( pFile, "CEX: %s@%d=%c\n", Abc_ObjName(pObj), f, '0'+Abc_InfoHasBit(pCex->pData, pCex->nRegs+pCex->nPis*f + i) );
    Abc_CexFreeP( &pCare );
}


extern Abc_Cex_t * Bmc_CexInnerStates( Gia_Man_t * p, Abc_Cex_t * pCex, Abc_Cex_t ** ppCexImpl, int fVerbose );
extern Abc_Cex_t * Bmc_CexEssentialBits( Gia_Man_t * p, Abc_Cex_t * pCexState, Abc_Cex_t * pCexCare, int fVerbose );
extern Abc_Cex_t * Bmc_CexCareBits( Gia_Man_t * p, Abc_Cex_t * pCexState, Abc_Cex_t * pCexImpl, Abc_Cex_t * pCexEss, int fFindAll, int fVerbose );

void Abc_NtkDumpOneCex( FILE * pFile, Abc_Ntk_t * pNtk, Abc_Cex_t * pCex, 
    int fPrintFull, int fNames, int fUseFfNames, int fMinimize, int fUseOldMin, int fCexInfo,
    int fCheckCex, int fUseSatBased, int fHighEffort, int fAiger, int fVerbose, int fExtended )
{
    Abc_Cex_t * pCare = NULL;
    Abc_Obj_t * pObj;
    int i, f;
    if ( fPrintFull )
    {
        extern Aig_Man_t * Abc_NtkToDar( Abc_Ntk_t * pNtk, int fExors, int fRegisters );
        Aig_Man_t * pAig = Abc_NtkToDar( pNtk, 0, 1 );
        Abc_Cex_t * pCexFull = Saig_ManExtendCex( pAig, pCex );
        Aig_ManStop( pAig );
        // output PI values (while skipping the minimized ones)
        assert( pCexFull->nBits == Abc_NtkCiNum(pNtk) * (pCex->iFrame + 1) );
        for ( f = 0; f <= pCex->iFrame; f++ )
            Abc_NtkForEachCi( pNtk, pObj, i )
                fprintf( pFile, "%s@%d=%c ", Abc_ObjName(pObj), f, '0'+Abc_InfoHasBit(pCexFull->pData, Abc_NtkCiNum(pNtk)*f + i) );
        Abc_CexFreeP( &pCexFull );
    }
    else
    {
        if ( fNames )
        {
            fprintf( pFile, "# FALSIFYING OUTPUTS:");
            fprintf( pFile, " %s", Abc_ObjName(Abc_NtkCo(pNtk, pCex->iPo)) );
        }
        if ( fMinimize )
        {
            extern Aig_Man_t * Abc_NtkToDar( Abc_Ntk_t * pNtk, int fExors, int fRegisters );
            Aig_Man_t * pAig = Abc_NtkToDar( pNtk, 0, 1 );
            if ( fUseOldMin )
            {
                pCare = Saig_ManCbaFindCexCareBits( pAig, pCex, 0, fVerbose );
                if ( fCheckCex )
                    Bmc_CexCareVerify( pAig, pCex, pCare, fVerbose );
            }
            else if ( fUseSatBased )
            {
                if ( Abc_NtkPoNum( pNtk ) == 1 )
                    pCare = Bmc_CexCareSatBasedMinimize( pAig, Saig_ManPiNum(pAig), pCex, fHighEffort, fCheckCex, fVerbose );
                else
                    printf( "SAT-based CEX minimization requires having a single PO.\n" );
            }
            else if ( fCexInfo )
            {
                Gia_Man_t * p = Gia_ManFromAigSimple( pAig );
                Abc_Cex_t * pCexImpl   = NULL;
                Abc_Cex_t * pCexStates = Bmc_CexInnerStates( p, pCex, &pCexImpl, fVerbose );
                Abc_Cex_t * pCexCare   = Bmc_CexCareBits( p, pCexStates, pCexImpl, NULL, 1, fVerbose );
                Abc_Cex_t * pCexEss;

                if ( fCheckCex && !Bmc_CexVerify( p, pCex, pCexCare ) )
                    printf( "Counter-example care-set verification has failed.\n" );

                pCexEss = Bmc_CexEssentialBits( p, pCexStates, pCexCare, fVerbose );

                // pCare is pCexMin from Bmc_CexTest
                pCare = Bmc_CexCareBits( p, pCexStates, pCexImpl, pCexEss, 0, fVerbose );

                if ( fCheckCex && !Bmc_CexVerify( p, pCex, pCare ) )
                    printf( "Counter-example min-set verification has failed.\n" );
                Abc_CexFreeP( &pCexStates );
                Abc_CexFreeP( &pCexImpl );
                Abc_CexFreeP( &pCexCare );
                Abc_CexFreeP( &pCexEss );
            }
            else
                pCare = Bmc_CexCareMinimize( pAig, Saig_ManPiNum(pAig), pCex, 4, fCheckCex, fVerbose );
            Aig_ManStop( pAig );
            if(pCare == NULL)
                printf( "Counter-example minimization has failed.\n" ); 
        }
        if (fNames)
        {
                fprintf( pFile, "\n");
                fprintf( pFile, "# COUNTEREXAMPLE LENGTH: %u\n", pCex->iFrame+1);
        }
        if ( fNames && fUseFfNames && Abc_NtkCheckSpecialPi(pNtk) )
        {
            int * pValues;
            int nXValues = 0, iFlop = 0, iPivotPi = -1;
            Abc_NtkForEachPi( pNtk, pObj, iPivotPi )
                if ( !strcmp(Abc_ObjName(pObj), "_abc_190121_abc_") )
                    break;
            if ( iPivotPi == Abc_NtkPiNum(pNtk) )
            {
                fprintf( stdout, "IoCommandWriteCex(): Cannot find special PI required by switch \"-z\".\n" );
                return;
            }
            // count X-valued flops
            for ( i = iPivotPi+1; i < Abc_NtkPiNum(pNtk); i++ )
                if ( Abc_ObjName(Abc_NtkPi(pNtk, i))[0] == 'x' )
                    nXValues++;
            // map X-valued flops into auxiliary PIs
            pValues = ABC_FALLOC( int, Abc_NtkPiNum(pNtk) );
            for ( i = iPivotPi+1; i < Abc_NtkPiNum(pNtk); i++ )
                if ( Abc_ObjName(Abc_NtkPi(pNtk, i))[0] == 'x' )
                    pValues[i] = iPivotPi - nXValues + iFlop++;
            assert( iFlop == nXValues );
            // write flop values
            for ( i = iPivotPi+1; i < Abc_NtkPiNum(pNtk); i++ )
                if ( pValues[i] == -1 )
                    fprintf( pFile, "%s@0=%c\n", Abc_ObjName(Abc_NtkPi(pNtk, i))+1, Abc_ObjName(Abc_NtkPi(pNtk, i))[0] );
                else if ( Abc_InfoHasBit(pCare->pData, pCare->nRegs + pValues[i]) )
                    fprintf( pFile, "%s@0=%c\n", Abc_ObjName(Abc_NtkPi(pNtk, i))+1, '0'+Abc_InfoHasBit(pCex->pData, pCex->nRegs + pValues[i]) );
            ABC_FREE( pValues );
            // output PI values (while skipping the minimized ones)
            for ( f = 0; f <= pCex->iFrame; f++ )
                Abc_NtkForEachPi( pNtk, pObj, i )
                {
                    if ( i == iPivotPi - nXValues ) break;
                    if ( !pCare || Abc_InfoHasBit(pCare->pData, pCare->nRegs+pCare->nPis*f + i) )
                        fprintf( pFile, "%s@%d=%c\n", Abc_ObjName(pObj), f, '0'+Abc_InfoHasBit(pCex->pData, pCex->nRegs+pCex->nPis*f + i) );
                }
        }
        else
        {
            if (fExtended && fAiger && !fNames) {
                fprintf( pFile, "1\n");
                fprintf( pFile, "b%d\n", pCex->iPo);
            }
            // output flop values (unaffected by the minimization)
            Abc_NtkForEachLatch( pNtk, pObj, i )
                if ( fNames )
                    fprintf( pFile, "%s@0=%c\n", Abc_ObjName(Abc_ObjFanout0(pObj)), '0'+!Abc_LatchIsInit0(pObj) );
                else
                    fprintf( pFile, "%c", '0'+!Abc_LatchIsInit0(pObj) );
            if ( !fNames && fAiger)
                fprintf( pFile, "\n");
            // output PI values (while skipping the minimized ones)
            for ( f = 0; f <= pCex->iFrame; f++ ) {
                Abc_NtkForEachPi( pNtk, pObj, i )
                    if ( !pCare || Abc_InfoHasBit(pCare->pData, pCare->nRegs+pCare->nPis*f + i) )
                        if ( fNames )
                            fprintf( pFile, "%s@%d=%c\n", Abc_ObjName(pObj), f, '0'+Abc_InfoHasBit(pCex->pData, pCex->nRegs+pCex->nPis*f + i) );
                        else
                            fprintf( pFile, "%c", '0'+Abc_InfoHasBit(pCex->pData, pCex->nRegs+pCex->nPis*f + i) );
                    else if ( !fNames )
                        fprintf( pFile, "x");
                if ( !fNames && fAiger)
                    fprintf( pFile, "\n");
            }
            if (fExtended && fAiger && !fNames)
                fprintf( pFile, ".\n");
        }
        Abc_CexFreeP( &pCare );
    }
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int IoCommandWriteCex( Abc_Frame_t * pAbc, int argc, char **argv )
{
    Abc_Ntk_t * pNtk;
    char * pFileName;
    int c, fNames  = 0;
    int fMinimize  = 0;
    int fUseSatBased = 0;
    int fHighEffort = 0;
    int fUseOldMin = 0;
    int fCheckCex  = 0;
    int forceSeq   = 0;
    int fAiger     = 0;
    int fPrintFull = 0;
    int fUseFfNames = 0;
    int fVerbose   = 0;
    int fCexInfo   = 0;
    int fExtended  = 0;

    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "snmueocafzvhxt" ) ) != EOF )
    {
        switch ( c )
        {
            case 's':
                forceSeq ^= 1;
                break;
            case 'n':
                fNames ^= 1;
                break;
            case 'm':
                fMinimize ^= 1;
                break;
            case 'u':
                fUseSatBased ^= 1;
                break;
            case 'e':
                fHighEffort ^= 1;
                break;
            case 'o':
                fUseOldMin ^= 1;
                break;
            case 'c':
                fCheckCex ^= 1;
                break;
            case 'a':
                fAiger ^= 1;
                break;
            case 'f':
                fPrintFull ^= 1;
                break;
            case 'z':
                fUseFfNames ^= 1;
                break;
            case 'v':
                fVerbose ^= 1;
                break;
            case 'x':
                fCexInfo ^= 1;
                break;
            case 't':
                fExtended ^= 1;
                break;
            case 'h':
                goto usage;
            default:
                goto usage;
        }
    }
    pNtk = pAbc->pNtkCur;
    if ( pNtk == NULL )
    {
        fprintf( pAbc->Out, "Empty network.\n" );
        return 0;
    }
    if ( pNtk->pModel == NULL && pAbc->pCex == NULL && pAbc->vCexVec == NULL )
    {
        fprintf( pAbc->Out, "Counter-example is not available.\n" );
        return 0;
    }
    if ( argc != globalUtilOptind + 1 )
    {
        printf( "File name is missing on the command line.\n" );
        goto usage;
    }

    // get the input file name
    pFileName = argv[globalUtilOptind];
    // write the counter-example into the file
    if ( pAbc->pCex || pAbc->vCexVec )
    { 
        Abc_Cex_t * pCex = pAbc->pCex;
        FILE * pFile; int i;
        /*
        Abc_NtkForEachLatch( pNtk, pObj, i )
            if ( !Abc_LatchIsInit0(pObj) )
            {
                fprintf( stdout, "IoCommandWriteCex(): The init-state should be all-0 for counter-example to work.\n" );
                fprintf( stdout, "Run commands \"undc\" and \"zero\" and then rerun the equivalence check.\n" );
                return 1;
            }
        */
        pFile = fopen( pFileName, "w" );
        if ( pFile == NULL )
        {
            fprintf( stdout, "IoCommandWriteCex(): Cannot open the output file \"%s\".\n", pFileName );
            return 1;
        }
        if ( pAbc->pCex )
        {
            Abc_NtkDumpOneCex( pFile, pNtk, pCex, 
                fPrintFull, fNames, fUseFfNames, fMinimize, fUseOldMin, fCexInfo,
                fCheckCex, fUseSatBased, fHighEffort, fAiger, fVerbose, fExtended );
        }
        else if ( pAbc->vCexVec )
        {
            Vec_PtrForEachEntry( Abc_Cex_t *, pAbc->vCexVec, pCex, i )
            {
                if ( pCex == NULL )
                    continue;
                fprintf( pFile, "#\n#\n# CEX for output %d\n#\n", i );
                Abc_NtkDumpOneCex( pFile, pNtk, pCex, 
                    fPrintFull, fNames, fUseFfNames, fMinimize, fUseOldMin, fCexInfo,
                    fCheckCex, fUseSatBased, fHighEffort, fAiger, fVerbose, fExtended );
            }
        }
        fprintf( pFile, "# DONE\n" ); 
        fclose( pFile );
    }
    else
    {
        Abc_Obj_t * pObj;
        FILE * pFile = fopen( pFileName, "w" );
        int i;
        if ( pFile == NULL )
        {
            fprintf( stdout, "IoCommandWriteCex(): Cannot open the output file \"%s\".\n", pFileName );
            return 1;
        }
        if ( fNames )
        {
            const char *cycle_ctr = forceSeq?"@0":"";
            Abc_NtkForEachPi( pNtk, pObj, i )
//                fprintf( pFile, "%s=%c\n", Abc_ObjName(pObj), '0'+(pNtk->pModel[i]==1) );
                fprintf( pFile, "%s%s=%c\n", Abc_ObjName(pObj), cycle_ctr, '0'+(pNtk->pModel[i]==1) );
        }
        else
        {
            Abc_NtkForEachPi( pNtk, pObj, i )
                fprintf( pFile, "%c", '0'+(pNtk->pModel[i]==1) );
        }
        fprintf( pFile, "\n" );
        fclose( pFile );
    }

    return 0;

usage:
    fprintf( pAbc->Err, "usage: write_cex [-snmueocfzvh] <file>\n" );
    fprintf( pAbc->Err, "\t         saves counter-example (CEX) derived by \"sat\", \"iprove\", \"dprove\", etc\n" );
    fprintf( pAbc->Err, "\t         the output file <file> contains values for each PI in natural order\n" );
    fprintf( pAbc->Err, "\t-s     : always report a sequential CEX (cycle 0 for comb) [default = %s]\n", forceSeq? "yes": "no" );
    fprintf( pAbc->Err, "\t-n     : write input names into the file [default = %s]\n", fNames? "yes": "no" );
    fprintf( pAbc->Err, "\t-m     : minimize CEX by dropping don't-care values [default = %s]\n", fMinimize? "yes": "no" );
    fprintf( pAbc->Err, "\t-u     : use fast SAT-based CEX minimization [default = %s]\n", fUseSatBased? "yes": "no" );
    fprintf( pAbc->Err, "\t-e     : use high-effort SAT-based CEX minimization [default = %s]\n", fHighEffort? "yes": "no" );
    fprintf( pAbc->Err, "\t-o     : use old CEX minimization algorithm [default = %s]\n", fUseOldMin? "yes": "no" );
    fprintf( pAbc->Err, "\t-x     : minimize using algorithm from cexinfo command [default = %s]\n", fCexInfo? "yes": "no" );
    fprintf( pAbc->Err, "\t-c     : check generated CEX using ternary simulation [default = %s]\n", fCheckCex? "yes": "no" );
    fprintf( pAbc->Err, "\t-a     : print cex in AIGER 1.9 format [default = %s]\n", fAiger? "yes": "no" );
    fprintf( pAbc->Err, "\t-t     : extended header info when cex in AIGER 1.9 format [default = %s]\n", fAiger? "yes": "no" );
    fprintf( pAbc->Err, "\t-f     : enable printing flop values in each timeframe [default = %s]\n", fPrintFull? "yes": "no" );  
    fprintf( pAbc->Err, "\t-z     : toggle using saved flop names [default = %s]\n", fUseFfNames? "yes": "no" );  
    fprintf( pAbc->Err, "\t-v     : enable verbose output [default = %s]\n", fVerbose? "yes": "no" );  
    fprintf( pAbc->Err, "\t-h     : print the help massage\n" );
    fprintf( pAbc->Err, "\t<file> : the name of the file to write\n" );
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int IoCommandWriteEqn( Abc_Frame_t * pAbc, int argc, char **argv )
{
    char * pFileName;
    int c;

    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "h" ) ) != EOF )
    {
        switch ( c )
        {
            case 'h':
                goto usage;
            default:
                goto usage;
        }
    }
    if ( pAbc->pNtkCur == NULL )
    {
        fprintf( pAbc->Out, "Empty network.\n" );
        return 0;
    }
    if ( argc != globalUtilOptind + 1 )
        goto usage;
    // get the output file name
    pFileName = argv[globalUtilOptind];
    // call the corresponding file writer
    Io_Write( pAbc->pNtkCur, pFileName, IO_FILE_EQN );
    return 0;

usage:
    fprintf( pAbc->Err, "usage: write_eqn [-h] <file>\n" );
    fprintf( pAbc->Err, "\t         writes the current network in the equation format\n" );
    fprintf( pAbc->Err, "\t-h     : print the help massage\n" );
    fprintf( pAbc->Err, "\tfile   : the name of the file to write\n" );
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int IoCommandWriteEdgelist( Abc_Frame_t * pAbc, int argc, char **argv )
{
    char * pFileName;
    int c, fSpecial = 0;

    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "Nh" ) ) != EOF )
    {
        switch ( c )
        {
            case 'N':
                fSpecial ^= 1;
                break;
            /*
            case 'a':
                fUseHie ^= 1;
                break;
            case 'h':
                goto usage;
            */
            default:
                goto usage;
        }
    }
    if ( pAbc->pNtkCur == NULL )
    {
        fprintf( pAbc->Out, "Empty network.\n" );
        return 0;
    }
    if ( argc != globalUtilOptind + 1 )
        goto usage;
    // get the output file name
    pFileName = argv[globalUtilOptind];
    // call the corresponding file writer
    if ( fSpecial ) // keep original naming
        Io_WriteEdgelist( pAbc->pNtkCur, pFileName, 1, 0, 0, fSpecial); //last option is fName
    else
        Io_WriteEdgelist( pAbc->pNtkCur, pFileName, 1, 0, 0, fSpecial); //last option is fName
    return 0;

usage:
    fprintf( pAbc->Err, "usage: write_edgelist [-N] <file>\n" );
    fprintf( pAbc->Err, "\t         writes the network into edgelist file\n" );
    fprintf( pAbc->Err, "\t         part of Verilog-2-PyG (PyTorch Geometric). more details https://github.com/ycunxi/Verilog-to-PyG \n" );
    fprintf( pAbc->Err, "\t-N     : toggle keeping original naming of the netlist in edgelist (default=False)\n");  
    fprintf( pAbc->Err, "\t-h     : print the help massage\n" );
    fprintf( pAbc->Err, "\tfile   : the name of the file to write (extension .el)\n" );
    return 1;
}


/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int IoCommandWriteHMetis( Abc_Frame_t * pAbc, int argc, char **argv )
{
    char * pFileName;
    int fVerbose;
    int fSkipPo;
    int fWeightEdges;
    int c;

    fSkipPo       = 1;
    fWeightEdges  = 0;
    fVerbose      = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "swvh" ) ) != EOF )
    {
        switch ( c )
        {
            case 's':
                fSkipPo ^= 1;
                break;
            case 'w':
                fWeightEdges ^= 1;
                break;
            case 'v':
                fVerbose ^= 1;
                break;
            case 'h':
                goto usage;
            default:
                goto usage;
        }
    }
    if ( pAbc->pNtkCur == NULL )
    {
        fprintf( pAbc->Out, "Empty network.\n" );
        return 0;
    }
    if ( argc != globalUtilOptind + 1 )
        goto usage;
    // get the output file name
    pFileName = argv[globalUtilOptind];
    // call the corresponding file writer
    if ( !Abc_NtkIsStrash(pAbc->pNtkCur) )
    {
        fprintf( stdout, "Writing this format is only possible for structurally hashed AIGs.\n" );
        return 1;
    }
    Io_WriteHMetis( pAbc->pNtkCur, pFileName, fSkipPo, fWeightEdges, fVerbose );
    return 0;

usage:
    fprintf( pAbc->Err, "usage: write_hmetis <file>\n" );
    fprintf( pAbc->Err, "\t         writes the network in the hMetis format (https://karypis.github.io/glaros/files/sw/hmetis/manual.pdf)\n" );
    fprintf( pAbc->Err, "\t-s     : skip PO as sink explictly [default = %s]\n", fSkipPo? "yes" : "no" );
    fprintf( pAbc->Err, "\t-w     : simulate weight on hyperedges [default = %s]\n", fWeightEdges? "yes" : "no" );
    fprintf( pAbc->Err, "\t-v     : toggle printing verbose information [default = %s]\n", fVerbose? "yes" : "no" );
    fprintf( pAbc->Err, "\t-h     : print the help massage\n" );
    fprintf( pAbc->Err, "\tfile   : the name of the file to write\n" );
    return 1;
}



/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int IoCommandWriteGml( Abc_Frame_t * pAbc, int argc, char **argv )
{
    char * pFileName;
    int c;

    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "h" ) ) != EOF )
    {
        switch ( c )
        {
            case 'h':
                goto usage;
            default:
                goto usage;
        }
    }
    if ( pAbc->pNtkCur == NULL )
    {
        fprintf( pAbc->Out, "Empty network.\n" );
        return 0;
    }
    if ( argc != globalUtilOptind + 1 )
        goto usage;
    // get the output file name
    pFileName = argv[globalUtilOptind];
    // call the corresponding file writer
    Io_Write( pAbc->pNtkCur, pFileName, IO_FILE_GML );
    return 0;

usage:
    fprintf( pAbc->Err, "usage: write_gml [-h] <file>\n" );
    fprintf( pAbc->Err, "\t         writes network using graph representation formal GML\n" );
    fprintf( pAbc->Err, "\t-h     : print the help massage\n" );
    fprintf( pAbc->Err, "\tfile   : the name of the file to write\n" );
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int IoCommandWriteList( Abc_Frame_t * pAbc, int argc, char **argv )
{
    char * pFileName;
    int fUseHost;
    int c;

    printf( "This command currently does not work.\n" );
    return 0;

    fUseHost = 1;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "nh" ) ) != EOF )
    {
        switch ( c )
        {
            case 'n':
                fUseHost ^= 1;
                break;
            case 'h':
                goto usage;
            default:
                goto usage;
        }
    }
    if ( pAbc->pNtkCur == NULL )
    {
        fprintf( pAbc->Out, "Empty network.\n" );
        return 0;
    }
    if ( argc != globalUtilOptind + 1 )
        goto usage;
/*
    if ( !Abc_NtkIsSeq(pAbc->pNtkCur) )
    {
        fprintf( stdout, "IoCommandWriteList(): Can write adjacency list for sequential AIGs only.\n" );
        return 0;
    }
*/
    // get the input file name
    pFileName = argv[globalUtilOptind];
    // write the file
    Io_WriteList( pAbc->pNtkCur, pFileName, fUseHost );
    return 0;

usage:
    fprintf( pAbc->Err, "usage: write_list [-nh] <file>\n" );
    fprintf( pAbc->Err, "\t         writes network using graph representation formal GML\n" );
    fprintf( pAbc->Err, "\t-n     : toggle writing host node [default = %s]\n", fUseHost? "yes":"no" );
    fprintf( pAbc->Err, "\t-h     : print the help massage\n" );
    fprintf( pAbc->Err, "\tfile   : the name of the file to write\n" );
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int IoCommandWritePla( Abc_Frame_t * pAbc, int argc, char **argv )
{
    extern int Io_WriteMoPlaM( Abc_Ntk_t * pNtk, char * pFileName, int nMints );
    char * pFileName;
    int c, fUseMoPla = 0, nMints = 0;

    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "Mmh" ) ) != EOF )
    {
        switch ( c )
        {
            case 'M':
                if ( globalUtilOptind >= argc )
                {
                    Abc_Print( -1, "Command line switch \"-M\" should be followed by an integer.\n" );
                    goto usage;
                }
                nMints = atoi(argv[globalUtilOptind]);
                globalUtilOptind++;
                break;
            case 'm':
                fUseMoPla ^= 1;
                break;
            case 'h':
                goto usage;
            default:
                goto usage;
        }
    }
    if ( pAbc->pNtkCur == NULL )
    {
        fprintf( pAbc->Out, "Empty network.\n" );
        return 0;
    }
    if ( argc != globalUtilOptind + 1 )
        goto usage;
    // get the output file name
    pFileName = argv[globalUtilOptind];
    // call the corresponding file writer
    if ( nMints )
    {
        if ( Abc_NtkIsBddLogic(pAbc->pNtkCur) )
            Io_WriteMoPlaM( pAbc->pNtkCur, pFileName, nMints );
        else
        {
            Abc_Ntk_t * pStrash = Abc_NtkStrash( pAbc->pNtkCur, 0, 0, 0 );
            Io_WriteMoPlaM( pStrash, pFileName, nMints );
            Abc_NtkDelete( pStrash );
        }
    }
    else
        Io_Write( pAbc->pNtkCur, pFileName, fUseMoPla ? IO_FILE_MOPLA : IO_FILE_PLA );
    return 0;

usage:
    fprintf( pAbc->Err, "usage: write_pla [-M <num>] [-mh] <file>\n" );
    fprintf( pAbc->Err, "\t           writes the collapsed network into a PLA file\n" );
    fprintf( pAbc->Err, "\t-M <num> : the number of on-set minterms to write [default = %d]\n", nMints );
    fprintf( pAbc->Err, "\t-m       : toggle writing multi-output PLA [default = %s]\n", fUseMoPla? "yes":"no" );
    fprintf( pAbc->Err, "\t-h       : print the help massage\n" );
    fprintf( pAbc->Err, "\tfile     : the name of the file to write\n" );
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int IoCommandWriteVerilog( Abc_Frame_t * pAbc, int argc, char **argv )
{
    extern void Io_WriteVerilogLut( Abc_Ntk_t * pNtk, char * pFileName, int nLutSize, int fFixed, int fNoModules, int fNewInterface );
    char * pFileName;
    int c, fFixed = 0, fOnlyAnds = 0, fNoModules = 0, fNewInterface = 0;
    int nLutSize = -1;

    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "Kfamnh" ) ) != EOF )
    {
        switch ( c )
        {
            case 'K':
                if ( globalUtilOptind >= argc )
                {
                    Abc_Print( -1, "Command line switch \"-K\" should be followed by an integer.\n" );
                    goto usage;
                }
                nLutSize = atoi(argv[globalUtilOptind]);
                globalUtilOptind++;
                if ( nLutSize < 2 || nLutSize > 6 )
                    goto usage;
                break;
            case 'f':
                fFixed ^= 1;
                break;
            case 'a':
                fOnlyAnds ^= 1;
                break;
            case 'm':
                fNoModules ^= 1;
                break;
            case 'n':
                fNewInterface ^= 1;
                break;                
            case 'h':
                goto usage;
            default:
                goto usage;
        }
    }
    if ( pAbc->pNtkCur == NULL )
    {
        fprintf( pAbc->Out, "Empty network.\n" );
        return 0;
    }
    if ( argc != globalUtilOptind + 1 )
        goto usage;
    if ( fFixed )
        nLutSize = 6;
    // get the output file name
    pFileName = argv[globalUtilOptind];
    // call the corresponding file writer
    if ( nLutSize >= 2 && nLutSize <= 6 )
        Io_WriteVerilogLut( pAbc->pNtkCur, pFileName, nLutSize, fFixed, fNoModules, fNewInterface );
    else
    {
        Abc_Ntk_t * pNtkTemp = Abc_NtkToNetlist( pAbc->pNtkCur );
        if ( !Abc_NtkHasAig(pNtkTemp) && !Abc_NtkHasMapping(pNtkTemp) )
            Abc_NtkToAig( pNtkTemp );
        Io_WriteVerilog( pNtkTemp, pFileName, fOnlyAnds, fNewInterface );
        Abc_NtkDelete( pNtkTemp );
    }
    return 0;

usage:
    fprintf( pAbc->Err, "usage: write_verilog [-K num] [-famnh] <file>\n" );
    fprintf( pAbc->Err, "\t         writes the current network in Verilog format\n" );
    fprintf( pAbc->Err, "\t-K num : write the network using instances of K-LUTs (2 <= K <= 6) [default = not used]\n" );
    fprintf( pAbc->Err, "\t-f     : toggle using fixed format [default = %s]\n", fFixed? "yes":"no" );
    fprintf( pAbc->Err, "\t-a     : toggle writing expressions with only ANDs (without XORs and MUXes) [default = %s]\n", fOnlyAnds? "yes":"no" );
    fprintf( pAbc->Err, "\t-m     : toggle writing additional modules [default = %s]\n", !fNoModules? "yes":"no" );
    fprintf( pAbc->Err, "\t-n     : toggle writing generic PO names and assign-statements [default = %s]\n", fNewInterface? "yes":"no" );
    fprintf( pAbc->Err, "\t-h     : print the help massage\n" );
    fprintf( pAbc->Err, "\tfile   : the name of the file to write\n" );
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int IoCommandWriteSortCnf( Abc_Frame_t * pAbc, int argc, char **argv )
{
    char * pFileName;
    int c;
    int nVars = 16;
    int nQueens = 4;
    extern void Abc_NtkWriteSorterCnf( char * pFileName, int nVars, int nQueens );

    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "NQh" ) ) != EOF )
    {
        switch ( c )
        {
            case 'N':
                if ( globalUtilOptind >= argc )
                {
                    fprintf( stdout, "Command line switch \"-N\" should be followed by an integer.\n" );
                    goto usage;
                }
                nVars = atoi(argv[globalUtilOptind]);
                globalUtilOptind++;
                if ( nVars <= 0 ) 
                    goto usage;
                break;
            case 'Q':
                if ( globalUtilOptind >= argc )
                {
                    fprintf( stdout, "Command line switch \"-Q\" should be followed by an integer.\n" );
                    goto usage;
                }
                nQueens = atoi(argv[globalUtilOptind]);
                globalUtilOptind++;
                if ( nQueens <= 0 ) 
                    goto usage;
                break;
            case 'h':
                goto usage;
            default:
                goto usage;
        }
    }
    if ( argc != globalUtilOptind + 1 )
        goto usage;
    // get the output file name
    pFileName = argv[globalUtilOptind];
    Abc_NtkWriteSorterCnf( pFileName, nVars, nQueens );
    // call the corresponding file writer
    return 0;

usage:
    fprintf( pAbc->Err, "usage: write_sorter_cnf [-N <num>] [-Q <num>] <file>\n" );
    fprintf( pAbc->Err, "\t         writes CNF for the sorter\n" );
    fprintf( pAbc->Err, "\t-N num : the number of sorter bits [default = %d]\n", nVars );
    fprintf( pAbc->Err, "\t-Q num : the number of bits to be asserted to 1 [default = %d]\n", nQueens );
    fprintf( pAbc->Err, "\t-h     : print the help massage\n" );
    fprintf( pAbc->Err, "\tfile   : the name of the file to write\n" );
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int IoCommandWriteTruth( Abc_Frame_t * pAbc, int argc, char **argv )
{
    Vec_Int_t * vTruth;
    Abc_Ntk_t * pNtk = pAbc->pNtkCur;
    Abc_Obj_t * pNode;
    char * pFileName;
    FILE * pFile;
    unsigned * pTruth;
    int fHex = 1;
    int fReverse = 0;
    int c;

    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "xrh" ) ) != EOF )
    {
        switch ( c )
        {
            case 'x':
                fHex ^= 1;
                break;
            case 'r':
                fReverse ^= 1;
                break;
            case 'h':
                goto usage;
            default:
                goto usage;
        }
    }
    if ( pAbc->pNtkCur == NULL )
    {
        printf( "Current network is not available.\n" );
        return 0;
    }
    if ( !Abc_NtkIsLogic(pNtk) )
    {
        printf( "Current network should not an AIG. Run \"logic\".\n" );
        return 0;
    }
    if ( Abc_NtkPoNum(pNtk) != 1 )
    {
        printf( "Current network should have exactly one primary output.\n" );
        return 0;
    }
    if ( Abc_NtkNodeNum(pNtk) != 1 )
    {
        printf( "Current network should have exactly one node.\n" );
        return 0;
    }
    pNode = Abc_ObjFanin0( Abc_NtkPo(pNtk, 0) );
    if ( Abc_ObjFaninNum(pNode) == 0 )
    { 
        printf( "Can only write logic function with 0 inputs.\n" );
        return 0;
    }
    if ( Abc_ObjFaninNum(pNode) > 16 )
    { 
        printf( "Can only write logic function with no more than 16 inputs.\n" );
        return 0;
    }
    if ( argc != globalUtilOptind + 1 )
        goto usage;
    // get the input file name
    pFileName = argv[globalUtilOptind];
    // convert to logic
    Abc_NtkToAig( pNtk );
    vTruth = Vec_IntAlloc( 0 );
    pTruth = Hop_ManConvertAigToTruth( (Hop_Man_t *)pNtk->pManFunc, (Hop_Obj_t *)pNode->pData, Abc_ObjFaninNum(pNode), vTruth, fReverse );
    pFile = fopen( pFileName, "w" );
    if ( pFile == NULL )
    {
        Vec_IntFree( vTruth );
        printf( "Cannot open file \"%s\" for writing.\n", pFileName );
        return 0;
    }
    if ( fHex )
        Extra_PrintHex2( pFile, pTruth, Abc_ObjFaninNum(pNode) );
    else
        Extra_PrintBinary( pFile, pTruth, 1<<Abc_ObjFaninNum(pNode) );
    fclose( pFile );
    Vec_IntFree( vTruth );
    return 0;

usage:
    fprintf( pAbc->Err, "usage: write_truth [-xrh] <file>\n" );
    fprintf( pAbc->Err, "\t         writes truth table into a file\n" );
    fprintf( pAbc->Err, "\t-x     : toggles between bin and hex representation [default = %s]\n", fHex?  "hex":"bin" );
    fprintf( pAbc->Err, "\t-r     : toggle reversing bits in the truth table [default = %s]\n", fReverse? "yes":"no" );
    fprintf( pAbc->Err, "\t-h     : print the help massage\n" );
    fprintf( pAbc->Err, "\tfile   : the name of the file to write\n" );
    return 1;
}


/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int IoCommandWriteTruths( Abc_Frame_t * pAbc, int argc, char **argv )
{
    Gia_Obj_t * pObj;
    char * pFileName;
    FILE * pFile;
    word * pTruth;
    int nBytes;
    int fReverse = 0;
    int fHex = 1;
    int fBinaryFile = 0;
    int c, i;
 
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "rxbh" ) ) != EOF )
    {
        switch ( c )
        {
            case 'r':
                fReverse ^= 1;
                break;
            case 'x':
                fHex ^= 1;
                break;
            case 'b':
                fBinaryFile ^= 1;
                break;
            case 'h':
                goto usage;
            default:
                goto usage;
        }
    }
    if ( pAbc->pGia == NULL )
    {
        Abc_Print( -1, "IoCommandWriteTruths(): There is no AIG.\n" );
        return 1;
    } 
    if ( Gia_ManPiNum(pAbc->pGia) > 16 )
    {
        Abc_Print( -1, "IoCommandWriteTruths(): Can write truth tables up to 16 inputs.\n" );
        return 0;
    }
    if ( Gia_ManPiNum(pAbc->pGia) < 3 )
    {
        Abc_Print( -1, "IoCommandWriteTruths(): Can write truth tables for 3 inputs or more.\n" );
        return 0;
    }
    if ( argc != globalUtilOptind + 1 )
        goto usage;
    // get the input file name
    pFileName = argv[globalUtilOptind];
    // convert to logic
    pFile = fopen( pFileName, "wb" );
    if ( pFile == NULL )
    {
        printf( "Cannot open file \"%s\" for writing.\n", pFileName );
        return 0;
    }
    nBytes = 8 * Abc_Truth6WordNum( Gia_ManPiNum(pAbc->pGia) );
    Gia_ManForEachCo( pAbc->pGia, pObj, i )
    {
        pTruth = Gia_ObjComputeTruthTable( pAbc->pGia, pObj );
        if ( fBinaryFile )
            fwrite( pTruth, nBytes, 1, pFile );
        else if ( fHex )
            Extra_PrintHex( pFile, (unsigned *)pTruth, Gia_ManPiNum(pAbc->pGia) ), fprintf( pFile, "\n" );
        else
            Extra_PrintBinary( pFile, (unsigned *)pTruth, 1 << Gia_ManPiNum(pAbc->pGia) ), fprintf( pFile, "\n" );
    }
    fclose( pFile );
    return 0;

usage:
    fprintf( pAbc->Err, "usage: &write_truth [-rxbh] <file>\n" );
    fprintf( pAbc->Err, "\t         writes truth tables of each PO of GIA manager into a file\n" );
    fprintf( pAbc->Err, "\t-r     : toggle reversing bits in the truth table [default = %s]\n", fReverse? "yes":"no" );
    fprintf( pAbc->Err, "\t-x     : toggle writing in the hex notation [default = %s]\n", fHex? "yes":"no" );
    fprintf( pAbc->Err, "\t-b     : toggle using binary file format [default = %s]\n", fBinaryFile? "yes":"no" );
    fprintf( pAbc->Err, "\t-h     : print the help massage\n" );
    fprintf( pAbc->Err, "\tfile   : the name of the file to write\n" );
    return 1;
}


/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int IoCommandWriteStatus( Abc_Frame_t * pAbc, int argc, char **argv )
{
    char * pFileName;
    int c;
    extern void Abc_NtkWriteLogFile( char * pFileName, Abc_Cex_t * pCex, int Status, int nFrames, char * pCommand );

    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "h" ) ) != EOF )
    {
        switch ( c )
        {
            case 'h':
                goto usage;
            default:
                goto usage;
        }
    }
    if ( argc != globalUtilOptind + 1 )
        goto usage;
    // get the input file name
    pFileName = argv[globalUtilOptind];
    Abc_NtkWriteLogFile( pFileName, pAbc->pCex, pAbc->Status, pAbc->nFrames, NULL );
    return 0;

usage:
    fprintf( pAbc->Err, "usage: write_status [-h] <file>\n" );
    fprintf( pAbc->Err, "\t         writes verification log file\n" );
    fprintf( pAbc->Err, "\t-h     : print the help massage\n" );
    fprintf( pAbc->Err, "\tfile   : the name of the file to write\n" );
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int IoCommandWriteSmv( Abc_Frame_t * pAbc, int argc, char **argv )
{
    char * pFileName;
    int c;

    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "h" ) ) != EOF )
    {
        switch ( c )
        {
            case 'h':
                goto usage;
            default:
                goto usage;
        }
    }
    if ( pAbc->pNtkCur == NULL )
    {
        fprintf( pAbc->Out, "Empty network.\n" );
        return 0;
    }
    if ( argc != globalUtilOptind + 1 )
        goto usage;
    // get the output file name
    pFileName = argv[globalUtilOptind];
    // call the corresponding file writer
    Io_Write( pAbc->pNtkCur, pFileName, IO_FILE_SMV );
    return 0;

usage:
    fprintf( pAbc->Err, "usage: write_smv [-h] <file>\n" );
    fprintf( pAbc->Err, "\t         write the network in SMV format\n" );
    fprintf( pAbc->Err, "\t-h     : print the help message\n" );
    fprintf( pAbc->Err, "\tfile   : the name of the file to write (extension .smv)\n" );
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int IoCommandWriteJson( Abc_Frame_t * pAbc, int argc, char **argv )
{
    extern void Json_Write( char * pFileName, Abc_Nam_t * pStr, Vec_Wec_t * vObjs );
    extern void Json_Extract( char * pFileName, Abc_Nam_t * pStr, Vec_Wec_t * vObjs );
    int c, fExtract = 0;
    char * pFileName;

    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "ch" ) ) != EOF )
    {
        switch ( c )
        {
            case 'c':
                fExtract ^= 1;
                break;
            case 'h':
                goto usage;
            default:
                goto usage;
        }
    }
    if ( Abc_FrameReadJsonStrs(Abc_FrameReadGlobalFrame()) == NULL )
    {
        fprintf( pAbc->Out, "No JSON info is available.\n" );
        return 0;
    }
    if ( argc != globalUtilOptind + 1 )
        goto usage;
    pFileName = argv[globalUtilOptind];
    if ( fExtract )
        Json_Extract( pFileName, Abc_FrameReadJsonStrs(Abc_FrameReadGlobalFrame()), Abc_FrameReadJsonObjs(Abc_FrameReadGlobalFrame()) );
    else
        Json_Write( pFileName, Abc_FrameReadJsonStrs(Abc_FrameReadGlobalFrame()), Abc_FrameReadJsonObjs(Abc_FrameReadGlobalFrame()) );
    return 0;

usage:
    fprintf( pAbc->Err, "usage: write_json [-ch] <file>\n" );
    fprintf( pAbc->Err, "\t         write the network in JSON format\n" );
    fprintf( pAbc->Err, "\t-c     : output extracted version\n" );
    fprintf( pAbc->Err, "\t-h     : print the help message\n" );
    fprintf( pAbc->Err, "\tfile   : the name of the file to write (extension .json)\n" );
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int IoCommandWriteResub( Abc_Frame_t * pAbc, int argc, char **argv )
{
    extern void Gia_ManWriteResub( Gia_Man_t * p, char * pFileName );
    char * pFileName;
    int c;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "ch" ) ) != EOF )
    {
        switch ( c )
        {
            case 'h':
                goto usage;
            default:
                goto usage;
        }
    }
    if ( argc != globalUtilOptind + 1 )
        goto usage;
    pFileName = argv[globalUtilOptind];
    if ( pAbc->pGia == NULL )
    {
        Abc_Print( -1, "IoCommandWriteResub(): There is no AIG.\n" );
        return 1;
    }
    if ( Gia_ManCiNum(pAbc->pGia) > 20 )
    {
        Abc_Print( -1, "IoCommandWriteResub(): The number of inputs is wrong.\n" );
        return 1;
    }
    Gia_ManWriteResub( pAbc->pGia, pFileName );
    return 0;

usage:
    fprintf( pAbc->Err, "usage: &write_resub [-ch] <file>\n" );
    fprintf( pAbc->Err, "\t         write the network in resub format\n" );
    fprintf( pAbc->Err, "\t-h     : print the help message\n" );
    fprintf( pAbc->Err, "\tfile   : the name of the file to write (extension .json)\n" );
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int IoCommandWriteMM( Abc_Frame_t * pAbc, int argc, char **argv )
{
    extern int Abc_NtkWriteToFile( char * pFileName, Abc_Ntk_t * pNtk );
    Abc_Ntk_t * pNtk = Abc_FrameReadNtk(pAbc);
    char * pFileName = NULL; int c;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "h" ) ) != EOF )
    {
        switch ( c )
        {
            case 'h':
                goto usage;
            default:
                goto usage;
        }
    }
    if ( argc != globalUtilOptind + 1 )
        goto usage;
    pFileName = argv[globalUtilOptind];
    if ( pNtk == NULL )
    {
        Abc_Print( -1, "IoCommandWriteMM(): There is no current network.\n" );
        return 1;
    }
    if ( !Abc_NtkIsMappedLogic(pNtk) )
    {
        Abc_Print( -1, "IoCommandWriteMM(): The current network is not mapped.\n" );
        return 1;
    }
    Abc_NtkWriteToFile( pFileName, pNtk );
    return 0;

usage:
    fprintf( pAbc->Err, "usage: write_mm [-h] <file>\n" );
    fprintf( pAbc->Err, "\t         write mapped network into a file\n" );
    fprintf( pAbc->Err, "\t-h     : print the help message\n" );
    fprintf( pAbc->Err, "\tfile   : the name of the file to write\n" );
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int IoCommandWriteMMGia( Abc_Frame_t * pAbc, int argc, char **argv )
{
    extern Abc_Ntk_t * Abc_NtkFromCellMappedGia( Gia_Man_t * p, int fUseBuffs );
    extern int Abc_NtkWriteToFile( char * pFileName, Abc_Ntk_t * pNtk );
    Abc_Ntk_t * pNtk = NULL;
    char * pFileName = NULL; int c;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "h" ) ) != EOF )
    {
        switch ( c )
        {
            case 'h':
                goto usage;
            default:
                goto usage;
        }
    }
    if ( argc != globalUtilOptind + 1 )
        goto usage;
    pFileName = argv[globalUtilOptind];
    if ( pAbc->pGia == NULL )
    {
        Abc_Print( -1, "IoCommandWriteMMGia(): There is no current AIG.\n" );
        return 1;
    }
    if ( !Gia_ManHasCellMapping(pAbc->pGia) )
    {
        Abc_Print( -1, "IoCommandWriteMMGia(): Current AIG is not mapped.\n" );
        return 1;
    }
    pNtk = Abc_NtkFromCellMappedGia( pAbc->pGia, 0 );
    Abc_NtkWriteToFile( pFileName, pNtk );
    Abc_NtkDelete( pNtk );
    return 0;

usage:
    fprintf( pAbc->Err, "usage: &write_mm [-h] <file>\n" );
    fprintf( pAbc->Err, "\t         write cell mapped current AIG into a file\n" );
    fprintf( pAbc->Err, "\t-h     : print the help message\n" );
    fprintf( pAbc->Err, "\tfile   : the name of the file to write\n" );
    return 1;
}

////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////


ABC_NAMESPACE_IMPL_END

