 // Code to recreate the RACF def command for RACF resources
 #pragma linkage(IRRSEQ00 ,OS) 

// Include standard libraries 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>

#include <inttypes.h>

#define ASM_PREFIX " SYSSTATE ARCHLVL=2 \n" 

void printHex(FILE * pFile,void *buff, int length); 
int debug = 0;
#pragma pack(packed) 
typedef struct {
              char     lUserid;
              char     userid[8];
              char     flags;
              uint16_t offset;
              uint16_t count;
              char      first[0];
             } uAHeader_t ; 
             typedef struct {
             // short h3;
               char     name[8];
               char     flag;
               uint16_t count;
               char     data[0];
             } uASegment_t;
             typedef struct {
               char   name[8];
               char   flag;
               uint16_t   lField;
               char data[0];
             }uAField_t;
#pragma pack(reset)  
    
  // This structure is used for input and output
typedef struct {
          char eyecatcher[4];             // 'PXTR'
          uint32_t result_buffer_length;  // result buffer length
          uint8_t subpool;                // subpool of result buffer
          uint8_t version;                // parameter list version
          uint8_t reserved_1[2];          // reserved
          char class_name[8];             // class name - upper case, blank pad
          uint32_t lName;   // length of profile name
          char reserved_2[2];             // reserved
          char volume[6];                 // volume (for data set extract)
          uint32_t duplddsc;             // Duplicate discrete data set count.
          uint32_t flags;                 // see flag constants below
          uint32_t segment_count;         // number of segments
          char reserved_4[16];            // reserved
                                          // start of extracted data
          char data[256];                                
        } XtractParms_t;
typedef struct {  
          char  name[8];
          uint32_t flag;
          uint32_t count;
          uint32_t reserved16;
          uint32_t offset;
          char   Reserved24[16];
          char   next[0];                         
        } segment_t;   
        
typedef struct {
          char name[8];
          uint16_t type;
          char reserved10[2];
          uint32_t flag;
          uint32_t lData;
          char reserved20[4];
          uint32_t oData;
          //char reserved[4];
          char reserved28[16];
        } field_t;

  enum requestType_t {
      GENERALRESOURCE,
      USERID,
      DATASET
  };          
  typedef struct {
          char * pType;
          char * pWhich;
          int lWhich;
          int star;
          char myclass[8];
          int lClass;
          enum requestType_t rType;
          char firstXtractFunction ;
          char nextXtractFunction ; 
          char outFunctionCode; // alter general resource profile
          int debug;
        } inArgs; 
 
#define fGENERIC 0x10000000
#define ftRepeatingGroupHeader 0x1000 
#define ftRepeatingGroupMember 0x8000
#define flagBooleanValue 0x80000000

#define ftBoolean 0x2000
#include <constants.h>
#include <processOutput.h>  // issue the request and format the output
#include <processInput.h>  // issue the request and format the output
#include <doargs.h>  // issue the request and format the output
#include <mapit.h>   // convert from xtract format to write format
#include <doConnects.h>
#include <doPermits.h>
#include <blank.h>
/////////////
// MAIN
/////////////
// enum requestType_t resourceType;

int main( int argc, char *argv??(??)) 
{ 

  int inputRC;
  char * myClass ; 
   
  char extractBuffer[1024*1024]; // one MBish buffer should be big enough
  char * pXtractBuffer = &extractBuffer[0];

  int getLimit = 1000;
  int rc;
  inArgs myInArgs;
  rc =doArgs(argc,argv,&myInArgs);
  if (rc != 0)
    return rc;

  debug = myInArgs.debug;
 

  //////////////////////////////////////
  //  SET UP THE REQUEST
  ////////////////////////////////////


  //lMyProfile = strlen(myProfile);
  XtractParms_t * pXParms;
  // overlay the XPARMS over the buffer
  pXParms = ( XtractParms_t *) &extractBuffer[0];
  //  XtractParms_t XParms;
  memset(pXParms,0,sizeof(XtractParms_t));
  memcpy(pXParms -> eyecatcher,"PXTR",4);
  pXParms -> result_buffer_length = sizeof(XtractParms_t);
  pXParms -> version = 0;


  memcpy(&pXParms -> class_name,myInArgs.myclass,8);
  strcpy(&pXParms -> data[0],myInArgs.pWhich);
  pXParms -> lName = myInArgs.lWhich;
  pXParms -> flags = 0x00000000; 
 
  char funcCode = myInArgs.firstXtractFunction; // initial value - it changes  at end of the loop
  ////////////////////////////////////////////////////////////
  //  MAIN LOOP                                             //
  ////////////////////////////////////////////////////////////
  //
  // char * pCharXParms;
  // int lName =0;

  // pCharXParms = (char *) & XParms;
  // XParms. flags = 0x10000000; 
  // funcCode = myInArgs.firstXtractFunction;
  int doGeneric[2] = {0x00000000,fGENERIC}; // dont, do, generics

  for (int iGeneric = 0; iGeneric < 2; iGeneric ++)
  {  
    if (myInArgs.rType == USERID && iGeneric > 0)  // eg second time round the loop
      break; // we do not have generic userids
    strcpy(&pXParms -> data[0],myInArgs.pWhich);
    pXParms -> lName = myInArgs.lWhich;
    funcCode = myInArgs.firstXtractFunction;  // for both specific and generic
    
    for (int i=0;i <  getLimit;i++)
    {  
      inputRC = processInput(funcCode,  pXParms);
      funcCode = myInArgs.nextXtractFunction;   // set this up for next time
      if (inputRC!= 0 )
      {
        if (i == 0)
          continue; // first time we may get not found - so do the generic
        else
        {
          printf("Return from processInput %i\n",inputRC);
          break;
        }
      }  
        // pCharXParms = pXtractBuffer;
      pXParms  = (XtractParms_t * ) pXtractBuffer;
      pXParms -> flags = doGeneric[iGeneric]; // retrieve generic or not
      int lName = pXParms -> lName;

      if (debug > 0)
      {
        printf("extract:Post Input lname %i lwhich %i\n",lName,myInArgs.lWhich);
        printf("extract:Flag  %8.8x data %8.8s\n",pXParms -> flags,pXParms -> data );
        printf("extract:Which:%s.:%*.*s.\n",
          myInArgs.pWhich,
          pXParms -> lName, 
          pXParms -> lName,
          pXParms -> data );
      }  
      if (lName >= myInArgs.lWhich
          &&  memcmp(myInArgs.pWhich,pXParms -> data,myInArgs.lWhich ) < 0 )
      {
        if (debug > 0)
          printf("Extract exit loop\n");
        break; 
      }
      // resourceType = myInArgs.rType;
      rc = mapit(&myInArgs, pXParms);
    // funcCode = myInArgs.nextXtractFunction;
    if (myInArgs.star == 0) // one off
      break;
    funcCode = myInArgs.nextXtractFunction; 
    }
    if (iGeneric > 0 && inputRC  != 0)
        break;
           
  } 
}
// change % to ?
#define ASCII_STRING \
                "................" \
                "................" \
                " !\"#$%&'()*+,-./" \
                "0123456789:;<=>?" \
                "@ABCDEFGHIJKLMNO" \
                "PQRSTUVWXYZ[\\][_" \
                "'abcdefghijklmno" \
                "pqrstuvwxyz{ }~." \
                "................" \
                "................" \
                "................" \
                "................" \
                "................" \
                "................" \
                "................" \
                "................"


#define ASCII_STRING2 \ 
                "................" \ 
                "................" \ 
                " !?#¢?&'()*+,-./" \  
                "0123456789:;<=>?" \ 
                "@ABCDEFGHIJKLMNO" \ 
                "PQRSTUVWXYZ[?][_" \ 
            "'abcdefghijklmno" \ 
            "pqrstuvwxyz{ }~." \ 
            "................" \ 
            "................" \ 
            "................" \ 
            "................" \ 
            "................" \ 
            "................" \ 
            "................" \ 
            "................" 
const char ascii_tab[256] = ASCII_STRING ; 
      char other_tab[256]; 
void printHex(FILE * pFile,void *buff, int length) 
{ 

  char sepchars[2] = {' ',' '}; 
  int  i, j; 
  unsigned char *pbuff = (unsigned char*)buff; 
  long bytesleft; 
  char printchar; 
  for( i=0; i<length; i+=16, pbuff+=16 ) 
  { 
    if( (bytesleft=length-i) >= 16 ) 
            bytesleft=16; 
    /********************************************************** 
     * First print a row of hex chars. 
     **********************************************************/ 
    fprintf(pFile,"%8.8X :",i); 
    for( j=0; j<bytesleft; j++ ) { 
            if(j % 4 == 0) fprintf(pFile," %2.2X",*(pbuff+j)); 
            else fprintf(pFile,"%2.2X",*(pbuff+j)); 
    } 
    /********************************************************** 
    * This pads the hex display with blanks if this is the 
    * last row and it's less than 16 bytes long. 
    **********************************************************/ 
    for( j=bytesleft; j<16; j++ ) { 
            if(j % 4 == 0) fprintf(pFile,"   "); 
            else fprintf(pFile,"  "); 
    } 
    /*********************************************************** 
     * Next print the characters themselves (if they're 
     * printable). 
     ********************************************************/ 
  fprintf(pFile," %c", sepchars[0]); 
  for( j=0; j<bytesleft; j++ ) { 
          printchar = isprint(*(pbuff+j)) ? *(pbuff+j) : '.'; 
          fprintf(pFile,"%c",printchar); 
  } 
  /********************************************************** 
   * ..and pad the last row with blanks if necessary. 
   **********************************************************/ 
  for( j=bytesleft; j<16; j++ ) fprintf(pFile," "); 
  fprintf(pFile,"%c",sepchars[1]); 
  for( j=0; j<bytesleft; j++ ) { 
          int zz = *(pbuff+j);
          //fprintf(stderr,"%2.2x %c %i %c\n",*(pbuff+j),ascii_tab[*(pbuff+j)],zz,ascii_tab[zz]);
          fprintf(pFile,"%c", ascii_tab[zz]); 
  //        fprintf(pFile,"%c", ascii_tab[*(pbuff+j)]); 
  } 
  /********************************************************** 
   * ..and pad the last row with blanks if necessary. 
   **********************************************************/ 
  for( j=bytesleft; j<16; j++ ) fprintf(pFile," "); 
  fprintf(pFile,"%c\n",sepchars[1]); 
  } 
  return; 
}  



