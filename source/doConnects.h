    
////////////////////////////////////
//  mapit()
///////////////////////////////////
int doConnects(inArgs * pInArgs, XtractParms_t * pXParms)
{  

  segment_t * pInSegment;

  enum requestType_t resourceType;
  int lName =0;
  int lClass;
  char * pXtractBuffer = (char *) pXParms;
   

  //char output[40960]; // output buffer for building the output parameters
  //pOutH = (uAHeader_t *) &output[0];  

  if ( pInArgs -> rType != USERID)
    return 0;

  //for (int i=0;i <1;i++)  // ???
  // {
    // segment_t * pInSegment;
    lName = pXParms -> lName;

    //////////////////
    //  Start doing segments
    //////////////////
    pInSegment = (segment_t *)((char *) &(pXParms-> data) +lName);
    int lProfileName = pXParms-> lName;
    char * pProfileName  = (char *) & pXParms -> data;
    int iSegment; // iterator
    int segmentCount = pXParms ->segment_count;

    int lData;
    int oData;
    char * pData;
   
    for (iSegment= 0;iSegment < 1;iSegment ++) // only the first segment
    {  //segments
  
      int nFields = pInSegment -> count;
          
      if (debug > 0 )     
        printf("doConnects SEGMENT %8.8s nfields %i\n",pInSegment-> name, nFields);
      field_t * pInputField;
      pInputField = (field_t *) (pXtractBuffer  + pInSegment -> offset);
     
      for (int iField = 0; iField < nFields;iField++)
      {
        char * pString = pXtractBuffer+ pInputField -> oData;
        lData = pInputField -> lData;
        oData = pInputField -> oData;
        pData = pXtractBuffer+ pInputField -> oData;
        //printf("doconnect %4.4x  %i / %i %*.*s. \n",pInputField -> type, iField, nFields, lData,lData,pString);

        if (debug > 0)
          {
            fprintf(stdout,"donnit: %8.8s type %4.4x flag %8.8x lData %i oData %i\n",
                  pInputField -> name, 
                  pInputField -> type,  
                  pInputField -> flag,
                  lData, 
                  oData);
             if ( lData > 0) 
               printHex(stdout,pData,lData);      
          }

          if (pInputField -> type == ftRepeatingGroupHeader ) //  repeating group header
          {  
              if (debug > 0 )              
                printf("Field: %8.8s Repeating Value Header\n",pInputField -> name); 
              nFields = nFields + lData * oData;
          }  
          else
          {  

          #define IFBOOLEAN(a,b)  if ((strncmp(&pInputField -> name[0],a,8) ==0 ) \
               &&((pInputField -> type & ftBoolean) == ftBoolean) \
               &&((pInputField -> flag & flagBooleanValue) == flagBooleanValue ))  \
               printf(" %s ",#b) ;
          #define IFVALUE(a,b)  if ((strncmp(&pInputField -> name[0],a,8) ==0 ) \
               &&(pInputField -> type == ftRepeatingGroupMember) \
               &&( lData > 0 )) \
               { \              
                 printf(" %s(%*.*s) ",b,lData,lData, pData) ;  \
               }   
          //  // printf("--ldata %i\n",lData); \      
          int zz = 0;     
          #define IFNAME(a)  if (memcmp(&pInputField -> name,a,8) ==0) ;                     
          //          printf("??74 %s %8.8s %4.4x \n",#a,pInputField -> name,pInputField -> type);
          if (strncmp(&pInputField -> name[0],"CGROUP  ",8) ==0 )  
          {
            printf("\n");  // end of previous connect
            printf("CONNECT %*.*s  ",lProfileName,lProfileName,pProfileName);
          }              
          IFVALUE("CGROUP  ","GROUP")
          // CAUTHDA  connect date
          // COWNER CONNECT OWNER
          // CLJTIME ? missing
          // CLJDATE ?
          IFVALUE("CUACC   ","UACC")
          // CINITCT // count?  
          IFBOOLEAN("CADSP   ",ADSP)
          IFBOOLEAN("CSPECIAL",SPECIAL)
          IFBOOLEAN("COPER   ",OPERATIONS)
          IFVALUE("CGRPACC ","GRPACC")
          IFBOOLEAN("CAUDITOR",AUDITOR)
          IFVALUE("CREVOKE ","- \n    REVOKE")  // put this on a new line
          IFVALUE("CRESUME ","- \n    RESUME")  // and this on a new line
      }     
   
        pInputField ++;
      }
      // write out what we have, and set up for the next one  
      //processOutput(pInArgs -> outFunctionCode,(char *) pOutH);
     // pInSegment = (segment_t *) pInSegment -> next;// locate the next one

    }  // segments
    printf("\n");
    // funcCode = inArgs.nextXtractFunction;
  //}
  return 0;
}  