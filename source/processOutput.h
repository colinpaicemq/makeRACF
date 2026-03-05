
int processOutput(char function, char * pData)
{
    char work_area[1024]; 
    int Option_word = 0; 
    int rc; 
    long SAF_RC,RACF_RC,RACF_RS; 
    SAF_RC=0 ; 
    long ALET = 0; 
    char subpool;

    char RACF_userid[9];
    char * ACEE_ptr = 0;
    RACF_userid[0]=0;  // ignore userid  
    int iFunc = function;
    if (debug > 0 )
    { 
      printf("ProcessOutput input function %2.2x\n", function);
      printHex(stdout,pData,128);
    }                                                
    char Out_message_subpool = 1;
    char * pHeader; // returned by program
    
     __asm(ASM_PREFIX
            " MODESET MODE=SUP \n"
            :
            :
            :"r0","r1","r15"); 
    rc=IRRSEQ00( 
            &work_area, 
            &ALET , &SAF_RC, 
            &ALET , &RACF_RC, 
            &ALET , &RACF_RS, 
            &function, 
            pData, 
            &RACF_userid, 
            &ACEE_ptr, 
            &Out_message_subpool, 
            &pHeader
            ); 
    __asm(ASM_PREFIX
            " MODESET MODE=PROB \n"
            :
            :
            :"r0","r1","r15");        
  
    // printf("OUT ADD|RESS  %8.8x\n",pHeader);
    if (SAF_RC > 0   )
      printf("ProcessOutput SAFRC %i RACFRC %i RACFRS %i function code %8.8x\n", 
        SAF_RC,RACF_RC,RACF_RS,iFunc)  ; 
    
    if ( SAF_RC + RACF_RC +RACF_RS == 0)
    {
      //
      //  printHex(stdout,pHeader,255);
      int y =  memcmp(pHeader+4,"RCMD",4);

      int l = 0;
      char * pOut = "";
      if (y == 0) // match
      {
        uint16_t outlen;
        memcpy(&outlen,pHeader + 16,2);
        pOut = pHeader+18;
        l = outlen;  // needs to be long 
        if (debug > 0)
          printf("%*.*s\n",l,l,pOut);
      }
      deblank(pOut,l); 
      // now delete the storage which was acquired for us. 

      //XtractParms_t * pXParms 
      //pXParms = (XtractParms_t *) pHeader;  
      int lDeleteme ;
      uint32_t outSubpool;
      memcpy(&lDeleteme,pHeader +8,4); // subpool + storage
      outSubpool = lDeleteme;
      outSubpool = outSubpool >>24; // move subpool to right hand end of the word
      // printHex(stdout,pHeader,64);
      lDeleteme = lDeleteme &0x00ffffff;  // remove the subpool from the top
      int freerc = 0;  
     
      asm( ASM_PREFIX 
            " STORAGE RELEASE,LENGTH=(%[l]),ADDR=(%[a]),SP=(%[s]),COND=YES,RTCD=%[rc] "
            : [rc] "+m"(freerc) //* output
            : [l]  "r"(lDeleteme), 
              [a]  "r"(pHeader),
              [s]  "r"(outSubpool)
            :"r0", "r1" , "r15" ); 
      if (freerc != 0)           
        printf("Output After Storage release rc %i %8.8x length %i\n",freerc,pHeader,lDeleteme);
      if (freerc == 4)
        printf("Meaning: Program error. Not all requested virtual storage was freed\n");
      return SAF_RC;                                                                 

    }
    else
    {
      printf("input\n");

      uAHeader_t  * pOutH;
      pOutH = (uAHeader_t *) pData;
      int iOffset = pOutH -> offset;
      
      printf("Data following, has a problem at offset %8.8x \n",iOffset);
      int iLength = 256;
      if (iOffset  > 240)
        iLength = iOffset +64;
      if (iLength > 1024) 
        iLength = 1024;
      printHex(stdout,pOutH,iLength);
      if (iLength < 1024)
      {
        char * pBad =  (char *) ((char *) pOutH + iOffset);
        printHex(stdout,pBad,128);  
      }
    }
    
    return 0;                                                                 
} 