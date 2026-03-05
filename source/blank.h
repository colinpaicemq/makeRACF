 //   Code to generate a pass ticket 
 #pragma linkage(IRRSEQ00 ,OS) 
 // #pragma runopts(POSIX(ON)) 
 /*Include standard libraries */ 
  #include <stdio.h> 
  #include <stdlib.h> 
  #include <string.h> 
//  #include <stdarg.h> 
//  #include <iconv.h> 
//  #define _ISOC99_SOURCE
#include <inttypes.h>


//void printHex(FILE * pFile,void *buff, int length); 
// we cannot use sscan or strtok because we have to worry about quoted strings
                                                                       
int deblank( char * p, int lIn) 
  { 
    //char * p = "NAME        ('ADC DB') DFLTGRP     (TEST) NOAUDITOR      WHEN( TIME        (ANYTIME))";
    if (debug > 0) 
    {
      printf("Blank in\n");
      printHex(stdout,p,lIn);
   }
    char * pIn;
    char * pOut;
    char * pEnd = p + lIn;
    char out[1024];
    int blankCount = 0;
    char * pStartOut = &out[0];

    //int lIn = strlen(p);
    // int i;
    int inQuote = 0;
    int inBlank = 0;
    int lLine = 60;
    int inBra = 0;
    char * blanks = "                       ";// for indenting outout

    //int pos;
    pOut = pStartOut;

    for (pIn = p;pIn < pEnd; pIn++)
    {
        if (*pIn == '\'')
           {
            if (inQuote == 0)
               inQuote = 1;
            else
            { 
               // handle 'abc''zyz'
               if (*(pIn+1) == '\'') // we are a double quote
               {
                  *pOut = *pIn;
                  pOut += 1; 
                  pIn += 1;
                  // inQuote = 1 stays the same    
               }
               else
                inQuote = 0;
            }   
            
            *pOut = *pIn;
            pOut += 1;
            inBlank  = 0;
             
           }
        else if (*pIn == ' ')
            {
               blankCount ++;
               if (inQuote != 0)
                {
                   * pOut = *pIn; // move it anyway
                   pOut += 1;
                } 
               else // not in a quote
                {
                  int curLength = pOut - pStartOut ;
                  //printf("69 
                  // Curlength %i\n",curLength);
                  if ( curLength >  lLine) 
                  { // add a blank, end of line and print it
                    *pOut = ' ';
                    pOut++;

                    *pOut = '-';
                    pOut ++;
                    *pOut = 0;
                    printf("Line is too long \n"); 
                    printf("%s\n",pStartOut );  // the output string so far
                    pOut = pStartOut;
                    *pOut  = ' '; // put some blanks on the front of the line 
                    pOut ++;
                    *pOut  = ' ';
                    pOut ++;
                  }
                  // move first blank
                  if (inBlank == 0)
                  {
                      // first time move a blank 
                     * pOut = *pIn; 
                      pOut += 1;
                     inBlank = 1;
                  }
                  else; // dont move it  
                }   
                        
                
            } 
         else if (*pIn == '(')
         {
            inBra +=1;
            //printf("==103 %10.10s\n",pIn);
            //printf("==104 %2.10s\n",pOut - 10);
            // we want to keep the blank in 
            // ALTDSD (ADCD.S0W1.HZSPDATA) 
            // so go back one char once past the first
            if (*(pOut -1) == ' ' && blankCount == 0) 
                                            
              pOut -= 1;  // replace the blank
            *pOut = *pIn;
            pOut += 1;
            inBlank = 0;
         }
          else if (*pIn == ')')
         {
            inBra -=1;
            *pOut = *pIn;
            pOut += 1;
         }
         else
         {  // not quote and not blank
          //  printf("101 %20.20s inBlank %i inquote %i\n",pIn,inBlank,inQuote );
           //int curLength = pOut - pStartOut ;
           //       printf("=103 Curlength %i\n",curLength);
            if (inBlank == 1 && *pIn  != '(' && inQuote == 0  )
            { // put new keyword on a new line 
               //  printf("===90 invlabkline__");
           //      printf("==105\n"); 
                  *pOut = '-';
                  pOut ++;
                  *pOut = 0; 
                  printf("%s\n",pStartOut);
                  
                  memcpy(pStartOut,blanks,2+2*inBra);
                  pOut = pStartOut + 2 + 2*inBra;
            }
            *pOut = *pIn;    // move the data
            pOut += 1;
            inBlank = 0;
         }
    }

    *pOut = 0;
    printf("%s\n",pStartOut);
   return 0;     
}                      