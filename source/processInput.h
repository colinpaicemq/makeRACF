// issue the r_admin extract request
void printXParms(char * title, char function, XtractParms_t * pXParms )
{

  // if
  //printf(">>>>>>>>>>>%s\n",cFunction(function)) ;
  if (memcmp((char *) pXParms,"PXTR",4) == 0 )
    printf("%s 0x%2.2x %s. lBuff %i class %8.8s Dup %i flag %8.8x seg %i Profile %*.*s.\n",
        title,
        function, cFunction(function),
        pXParms-> result_buffer_length,
        pXParms ->class_name,
       // pXParms -> lName,
        pXParms -> duplddsc,
        pXParms -> flags,
        pXParms -> segment_count,
        pXParms -> lName,
        pXParms -> lName,
        pXParms -> data);
  else
  if (memcmp((char *) pXParms,"RXTR",4) == 0 )
   printf("%s %2.2x SETR seg \n",
        title,
        function,
        pXParms -> segment_count);

}

int processInput(char function, XtractParms_t * pXParms)
{
    char work_area[1024];
    // int Option_word = 0;
    int rc;
    long SAF_RC,RACF_RC,RACF_RS;
    SAF_RC=0 ;
    long ALET = 0;
    long subpool;
    char * pResponse;

    //char Function_code = 0x1f; // Extract a general resource profile
    char RACF_userid[9];
    char * ACEE_ptr = 0;
    RACF_userid[0]=0;  // ignore userid

    char Out_message_subpool = 1;
    //char * pHeader; // returned by program
    //printf("Input\n");
    //printHex(stdout,pIn,70);

    if (debug > 0)
      printXParms(">>processInput",function,pXParms );

    rc=IRRSEQ00(
            &work_area,
            &ALET , &SAF_RC,
            &ALET , &RACF_RC,
            &ALET , &RACF_RS,
            &function,
            (char * ) pXParms,
            &RACF_userid,
            &ACEE_ptr,
            &Out_message_subpool,
            &pResponse
            );
    //printf("input irrseq00\n");
    //printHex(stdout,pResponse,128);


    int iFunc = function;
    if (SAF_RC != 0)
    {
     fprintf(stderr,"Xtract SAFRC %i RACFRC %i RACFRS %i function code %s\n",
     SAF_RC,RACF_RC,RACF_RS,cFunction(iFunc));
     // printHex(stdout,pIn,256);
     return SAF_RC;
    }
    XtractParms_t * pXOutParms = (XtractParms_t *) pResponse;




    int lDeleteme = pXOutParms -> result_buffer_length;
    if (debug > 2)
    {
      printf("processInput: Returned data\n");
      printHex(stdout,pResponse,lDeleteme);
    }
    memcpy((char *) pXParms, pResponse, lDeleteme);
    if ( debug > 0)
       printXParms("<<processInput",function,pXParms );


    // now delete the temporary buffer which was allocated.
    // printf("lDeleteme %8.8x\n",lDeleteme);
    // memcpy(&subpool,pResponse +8,4); // subpool + storage

    // subpool = subpool >>24; // move subpool to right hand end of the word
    // printf("subpool %8.8x\n",subpool);
    subpool = Out_message_subpool ;

    int freerc = 0;
    asm( ASM_PREFIX
            " STORAGE RELEASE,LENGTH=(%[l]),ADDR=(%[a]),SP=(%[s]),COND=YES,RTCD=%[rc] "
            : [rc] "+m"(freerc) //* output
            : [l]  "r"(lDeleteme),
              [s]  "r"(subpool),
              [a]  "r"(pResponse)
            :"r0", "r1" , "r15" );
  if (freerc != 0)
    printf("Input Storage release rc %i %8.8x length %i sp %8.8x\n",freerc,pResponse,lDeleteme,subpool);
  if (freerc == 4)
    printf("Meaning: Program error. Not all requested virtual storage was freed\n");




  return SAF_RC;
}
