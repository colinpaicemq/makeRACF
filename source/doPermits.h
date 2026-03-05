    
////////////////////////////////////
//  mapit()
///////////////////////////////////
int doPermits(inArgs * pInArgs, XtractParms_t * pXParms)
{  

  segment_t * pInSegment;

  enum requestType_t resourceType;
  int lName =0;
  int lClass;
  char * pXtractBuffer = (char *) pXParms;

  if ( pInArgs -> rType == USERID)
    return 0;

    lName = pXParms -> lName;

    //////////////////
    //  Start doing segments
    //////////////////
    pInSegment = (segment_t *)((char *) &(pXParms-> data) +lName);
    int lProfileName = pXParms-> lName;
    char * pProfileName  = (char *) & pXParms -> data;
    int lData;
    int oData;
    char * pData;
   

    int nFields = pInSegment -> count;
        
    if (debug > 0 )     
      printf("doPermits SEGMENT %8.8s nfields %i\n",pInSegment-> name, nFields);
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
        fprintf(stdout,"dopermits: %8.8s type %4.4x flag %8.8x lData %i oData %i\n",
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
        if (strncmp(&pInputField -> name[0],"ACLID   ",8) ==0 )  
        {
          printf("\n");  // end of previous connect
          printf("PERMIT %*.*s CLASS(%8.8s) ",lProfileName,lProfileName,pProfileName,pXParms ->class_name);
        }              
        IFVALUE("ACLACS   ","ACCESS")
        IFVALUE("ACLID   ","ID")
      }     
   
        pInputField ++;
    } // for each field

    printf("\n\n"); // end of record and a blank line
    // funcCode = inArgs.nextXtractFunction;
  //}
  return 0;
}  