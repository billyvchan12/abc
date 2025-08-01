/**CFile****************************************************************

  FileName    [ifCom.c]

  SystemName  [ABC: Logic synthesis and verification system.]

  PackageName [FPGA mapping based on priority cuts.]

  Synopsis    [Command handlers.]

  Author      [Alan Mishchenko]
  
  Affiliation [UC Berkeley]

  Date        [Ver. 1.0. Started - November 21, 2006.]

  Revision    [$Id: ifCom.c,v 1.00 2006/11/21 00:00:00 alanmi Exp $]

***********************************************************************/

#include "if.h"
#include "base/main/main.h"

ABC_NAMESPACE_IMPL_START


////////////////////////////////////////////////////////////////////////
///                        DECLARATIONS                              ///
////////////////////////////////////////////////////////////////////////

static int If_CommandReadLut ( Abc_Frame_t * pAbc, int argc, char **argv );
static int If_CommandPrintLut( Abc_Frame_t * pAbc, int argc, char **argv );
static int If_CommandReadBox ( Abc_Frame_t * pAbc, int argc, char **argv );
static int If_CommandPrintBox( Abc_Frame_t * pAbc, int argc, char **argv );
static int If_CommandWriteBox( Abc_Frame_t * pAbc, int argc, char **argv );
static int If_CommandPrintTim( Abc_Frame_t * pAbc, int argc, char **argv );

////////////////////////////////////////////////////////////////////////
///                     FUNCTION DEFINITIONS                         ///
////////////////////////////////////////////////////////////////////////

/**Function*************************************************************

  Synopsis    [Package initialization procedure.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void If_Init( Abc_Frame_t * pAbc )
{
    // set the default library
    If_LibLut_t s_LutLib = { "lutlib", 4, 0, {0,1,1,1,1}, {{0},{1},{1},{1},{1}} };
    Abc_FrameSetLibLut( If_LibLutDup(&s_LutLib) );

    Cmd_CommandAdd( pAbc, "FPGA mapping", "read_lut",   If_CommandReadLut,   0 ); 
    Cmd_CommandAdd( pAbc, "FPGA mapping", "print_lut",  If_CommandPrintLut,  0 ); 

    Cmd_CommandAdd( pAbc, "FPGA mapping", "read_box",   If_CommandReadBox,   0 ); 
    Cmd_CommandAdd( pAbc, "FPGA mapping", "print_box",  If_CommandPrintBox,  0 ); 
    Cmd_CommandAdd( pAbc, "FPGA mapping", "write_box",  If_CommandWriteBox,  0 ); 
    Cmd_CommandAdd( pAbc, "FPGA mapping", "print_tim",  If_CommandPrintTim,  0 ); 
}

/**Function*************************************************************

  Synopsis    [Package ending procedure.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void If_End( Abc_Frame_t * pAbc )
{
    If_LibLutFree( (If_LibLut_t *)   Abc_FrameReadLibLut() );
    If_LibBoxFree( (If_LibBox_t *)Abc_FrameReadLibBox() );
}

/**Function*************************************************************

  Synopsis    [Command procedure to read LUT libraries.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int If_CommandReadLut( Abc_Frame_t * pAbc, int argc, char **argv )
{
    FILE * pFile;
    FILE * pOut, * pErr;
    If_LibLut_t * pLib;
    Abc_Ntk_t * pNet;
    char * FileName;
    int fVerbose;
    int c;
	int k = 0;

    pNet = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set the defaults
    fVerbose = 1;
    Extra_UtilGetoptReset();
    while ( (c = Extra_UtilGetopt(argc, argv, "vh4567")) != EOF ) 
    {
        switch (c) 
        {
            case 'v':
                fVerbose ^= 1;
                break;
            case 'h':
                goto usage;
                break;
			case '4':
				 k = 4; 
				 break;
			case '5':
				 k = 5; 
				 break;
			case '6':
				 k = 6; 
				 break;
			case '7':
				 k = 7; 
				 break;
            default:
                goto usage;
        }
    }

    if (argc != globalUtilOptind+1) {
	 if (k == 4) {
    	If_LibLutFree( (If_LibLut_t *)Abc_FrameReadLibLut() );
    	If_LibLut_t s_LutLib = { "lutlib", 4, 0, {0,1,1,1,1}, {{0},{1},{1},{1},{1}} };
    	Abc_FrameSetLibLut( If_LibLutDup(&s_LutLib) );
		return 0;
	 }
	 if (k == 5) {
    	If_LibLutFree( (If_LibLut_t *)Abc_FrameReadLibLut() );
    	If_LibLut_t s_LutLib = { "lutlib", 5, 0, {0,1,1,1,1,2}, {{0},{1},{1},{1},{1},{1.2}} };
    	Abc_FrameSetLibLut( If_LibLutDup(&s_LutLib) );
		return 0;
	 }
     if (k == 6) {
    	If_LibLutFree( (If_LibLut_t *)Abc_FrameReadLibLut() );
    	If_LibLut_t s_LutLib = { "lutlib", 6, 0, {0,1,1,1,1,2,2}, {{0},{1},{1},{1},{1},{1.2},{1.2}} };
    	Abc_FrameSetLibLut( If_LibLutDup(&s_LutLib) );
		return 0;
	 }
	 if (k == 7) {
    	If_LibLutFree( (If_LibLut_t *)Abc_FrameReadLibLut() );
    	If_LibLut_t s_LutLib = { "lutlib", 7, 0, {0,1,1,1,1,2,2,2}, {{0},{1},{1},{1},{1},{1.2},{1.2},{1.2}} };
    	Abc_FrameSetLibLut( If_LibLutDup(&s_LutLib) );
		return 0;
	 }
     goto usage;
    }


    if ( argc != globalUtilOptind + 1 )
        goto usage;

    // get the input file name
    FileName = argv[globalUtilOptind];
    if ( (pFile = fopen( FileName, "r" )) == NULL )
    {
        fprintf( pErr, "Cannot open input file \"%s\". ", FileName );
        if ( (FileName = Extra_FileGetSimilarName( FileName, ".genlib", ".lib", ".gen", ".g", NULL )) )
            fprintf( pErr, "Did you mean \"%s\"?", FileName );
        fprintf( pErr, "\n" );
        return 1;
    }
    fclose( pFile );

    // set the new network
    pLib = If_LibLutRead( FileName, k );
    if ( pLib == NULL )
    {
        fprintf( pErr, "Reading LUT library has failed.\n" );
        goto usage;
    }
    // replace the current library
    If_LibLutFree( (If_LibLut_t *)Abc_FrameReadLibLut() );
    Abc_FrameSetLibLut( pLib );
    return 0;

usage:
    fprintf( pErr, "\nusage: read_lut [-vh]\n");
    fprintf( pErr, "\t          read the LUT library from the file\n" );  
    fprintf( pErr, "\t-v      : toggles enabling of verbose output [default = %s]\n", (fVerbose? "yes" : "no") );
    fprintf( pErr, "\t-h      : print the command usage\n");
    fprintf( pErr, "\t                                        \n");
    fprintf( pErr, "\t          File format for a LUT library:\n");
    fprintf( pErr, "\t          (the default library is shown)\n");
    fprintf( pErr, "\t                                        \n");
    fprintf( pErr, "\t          # The area/delay of k-variable LUTs:\n");
    fprintf( pErr, "\t          # k  area   delay\n");
    fprintf( pErr, "\t          1      1      1\n");
    fprintf( pErr, "\t          2      2      2\n");
    fprintf( pErr, "\t          3      4      3\n");
    fprintf( pErr, "\t          4      8      4\n");
    fprintf( pErr, "\t          5     16      5\n");
    fprintf( pErr, "\t          6     32      6\n");
    return 1;       /* error exit */
}

/**Function*************************************************************

  Synopsis    [Command procedure to read LUT libraries.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int If_CommandPrintLut( Abc_Frame_t * pAbc, int argc, char **argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNet;
    int fVerbose;
    int c;

    pNet = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set the defaults
    fVerbose = 1;
    Extra_UtilGetoptReset();
    while ( (c = Extra_UtilGetopt(argc, argv, "vh")) != EOF ) 
    {
        switch (c) 
        {
            case 'v':
                fVerbose ^= 1;
                break;
            case 'h':
                goto usage;
                break;
            default:
                goto usage;
        }
    }

    if ( argc != globalUtilOptind )
        goto usage;

    // set the new network
    If_LibLutPrint( (If_LibLut_t *)Abc_FrameReadLibLut() );
    return 0;

usage:
    fprintf( pErr, "\nusage: print_lut [-vh]\n");
    fprintf( pErr, "\t          print the current LUT library\n" );  
    fprintf( pErr, "\t-v      : toggles enabling of verbose output [default = %s]\n", (fVerbose? "yes" : "no") );
    fprintf( pErr, "\t-h      : print the command usage\n");
    return 1;       /* error exit */
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int If_CommandReadBox( Abc_Frame_t * pAbc, int argc, char **argv )
{
    FILE * pFile;
    FILE * pOut, * pErr;
    If_LibBox_t * pLib;
    Abc_Ntk_t * pNet;
    char * FileName;
    int fExtended;
    int fVerbose;
    int c;

    pNet = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set the defaults
    fExtended = 0;
    fVerbose = 1;
    Extra_UtilGetoptReset();
    while ( (c = Extra_UtilGetopt(argc, argv, "evh")) != EOF ) 
    {
        switch (c) 
        {
            case 'e':
                fExtended ^= 1;
                break;
            case 'v':
                fVerbose ^= 1;
                break;
            case 'h':
                goto usage;
                break;
            default:
                goto usage;
        }
    }

    if ( argc != globalUtilOptind + 1 )
        goto usage;

    // get the input file name
    FileName = argv[globalUtilOptind];
    if ( (pFile = fopen( FileName, "r" )) == NULL )
    {
        fprintf( pErr, "Cannot open input file \"%s\". ", FileName );
        if ( (FileName = Extra_FileGetSimilarName( FileName, ".genlib", ".lib", ".gen", ".g", NULL )) )
            fprintf( pErr, "Did you mean \"%s\"?", FileName );
        fprintf( pErr, "\n" );
        return 1;
    }
    fclose( pFile );

    // set the new network
    pLib = fExtended ? If_LibBoxRead2( FileName ) : If_LibBoxRead( FileName );
    if ( pLib == NULL )
    {
        fprintf( pErr, "Reading box library has failed.\n" );
        goto usage;
    }
    // replace the current library
    If_LibBoxFree( (If_LibBox_t *)Abc_FrameReadLibBox() );
    Abc_FrameSetLibBox( pLib );
    return 0;

usage:
    fprintf( pErr, "\nusage: read_box [-evh]\n");
    fprintf( pErr, "\t          read the box library from the file\n" );  
    fprintf( pErr, "\t-e      : toggles reading extended format [default = %s]\n", (fExtended? "yes" : "no") );
    fprintf( pErr, "\t-v      : toggles enabling of verbose output [default = %s]\n", (fVerbose? "yes" : "no") );
    fprintf( pErr, "\t-h      : print the command usage\n");
    return 1;       /* error exit */
}

/**Function*************************************************************

  Synopsis    [Command procedure to read LUT libraries.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int If_CommandPrintBox( Abc_Frame_t * pAbc, int argc, char **argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNet;
    int fVerbose;
    int c;

    pNet = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set the defaults
    fVerbose = 1;
    Extra_UtilGetoptReset();
    while ( (c = Extra_UtilGetopt(argc, argv, "vh")) != EOF ) 
    {
        switch (c) 
        {
            case 'v':
                fVerbose ^= 1;
                break;
            case 'h':
                goto usage;
                break;
            default:
                goto usage;
        }
    }

    if ( argc != globalUtilOptind )
        goto usage;

    // set the new network
    If_LibBoxPrint( stdout, (If_LibBox_t *)Abc_FrameReadLibBox() );
    return 0;

usage:
    fprintf( pErr, "\nusage: print_box [-vh]\n");
    fprintf( pErr, "\t          print the current box library\n" );  
    fprintf( pErr, "\t-v      : toggles enabling of verbose output [default = %s]\n", (fVerbose? "yes" : "no") );
    fprintf( pErr, "\t-h      : print the command usage\n");
    return 1;       /* error exit */
}

/**Function*************************************************************

  Synopsis    [Command procedure to read LUT libraries.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int If_CommandWriteBox( Abc_Frame_t * pAbc, int argc, char **argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNet;
    int fVerbose;
    int c;

    pNet = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    fVerbose = 1;
    Extra_UtilGetoptReset();
    while ( (c = Extra_UtilGetopt(argc, argv, "vh")) != EOF ) 
    {
        switch (c) 
        {
            case 'v':
                fVerbose ^= 1;
                break;
            case 'h':
                goto usage;
                break;
            default:
                goto usage;
        }
    }

    if ( argc != globalUtilOptind+1 )
        goto usage;

    If_LibBoxWrite( argv[globalUtilOptind], (If_LibBox_t *)Abc_FrameReadLibBox() );
    return 0;

usage:
    fprintf( pErr, "\nusage: write_box [-vh] <file>\n");
    fprintf( pErr, "\t          write the current box library into a file\n" );  
    fprintf( pErr, "\t-v      : toggles enabling of verbose output [default = %s]\n", (fVerbose? "yes" : "no") );
    fprintf( pErr, "\t-h      : print the command usage\n");
    fprintf( pErr, "\t<file>  : the output file name\n");
    return 1;       /* error exit */
}


/**Function*************************************************************

  Synopsis    [Command procedure to read LUT libraries.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int If_CommandPrintTim( Abc_Frame_t * pAbc, int argc, char **argv )
{
    Gia_Man_t * pGia = Abc_FrameReadGia(pAbc);
    int c, fVerbose = 0;
    Extra_UtilGetoptReset();
    while ( (c = Extra_UtilGetopt(argc, argv, "vh")) != EOF ) 
    {
        switch (c) 
        {
            case 'v':
                fVerbose ^= 1;
                break;
            case 'h':
                goto usage;
                break;
            default:
                goto usage;
        }
    }
    if ( pGia == NULL )
    {
        Abc_Print( -1, "There is no AIG in the &-space.\n" );
        return 1;
    }
    if ( pGia->pManTime == NULL )
    {
        Abc_Print( -1, "The current AIG does not have a timing manager.\n" );
        return 1;
    }
    Tim_ManPrint( (Tim_Man_t *)pGia->pManTime );
    if ( fVerbose ) 
        Tim_ManPrintBoxCopy( (Tim_Man_t *)pGia->pManTime );
    return 0;

usage:
    Abc_Print( -2, "\nusage: print_tim [-vh]\n");
    Abc_Print( -2, "\t          print the timing manager\n" );  
    Abc_Print( -2, "\t-v      : toggles enabling of verbose output [default = %s]\n", (fVerbose? "yes" : "no") );
    Abc_Print( -2, "\t-h      : print the command usage\n");
    return 1;       /* error exit */
}

////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////


ABC_NAMESPACE_IMPL_END

