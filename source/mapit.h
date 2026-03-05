
/////////////////////
//  addField 
////////////////////      
uAField_t  *  addField(char * pInputFieldName,char flag,int lData,char * pString, 
                       uASegment_t * pOutS,
                       uAField_t * pOutF
                      )
{  
  memcpy(&pOutF-> name,pInputFieldName,8); 
  if (debug > 0)                     
     printf("ADDFIELD  %8.8s:%*.*s\n",pInputFieldName,lData,lData,pString);
  pOutF -> flag = flag;  // add it
  pOutF -> lField =  lData;
  if (lData > 0)
      memcpy(&pOutF-> data,pString,lData);  
  pOutS -> count += 1;
  return (uAField_t  * )  (( (char *) pOutF) +sizeof(uAField_t) + lData); // point to next space  
}  

// extendField eg add days to the existing string of days.  Return the updated next pointer
uAField_t  *  extendField(int lData,
                          char * pString, 
                           uAField_t * pTempF
                          )
{  
  char * pData;
  if (debug > 0)                     
     printf("ExtendFIELD  %8.8s: %i %*.*s\n",pTempF-> name,lData,lData,lData,pString);

  // pTempF -> lField =  lData;
  pData = &pTempF-> data[pTempF -> lField];
  // point to next 
  *pData = ' '; // add a blank
  pData ++; // and point to the next
  if (lData > 0)
      memcpy(pData,pString,lData);  
  pTempF -> lField = pTempF -> lField+  1 + lData; // blank plus the data
  return (uAField_t  * )  (( (char *) pTempF) +sizeof(uAField_t) + pTempF -> lField); // point to next space  
}  

//////////////
//  Make segment
//////////////
uAField_t * makeSegment(uASegment_t * pOutS,
                          char *  name,
                          char flag,
                          uAHeader_t * pOutH // increment number of segments
                       )
{  
  if (debug > 0) 
    printf(">>>>>>>>>>>>>>>>>>>>>MAKESEGMENT %8.8s\n",name); 
  memcpy(&pOutS -> name,name,8);
  pOutS -> flag = flag ;   // create the segment
  pOutS -> count = 0;
  pOutH -> count ++;
  return (uAField_t *)& pOutS -> data[0];
}        
////////////////////////////////////
//  mapit()
///////////////////////////////////
int mapit(inArgs * pInArgs, XtractParms_t * pXParms)
{  
  uAHeader_t  * pOutH;
  uASegment_t * pOutS;
  uAField_t   * pOutF;
  uAField_t *   pWhenDays = 0;
  uAField_t *   pCLAUTH = 0;


  segment_t * pInSegment;

  enum requestType_t resourceType;
  int lName =0;
  int lClass;
  char * pXtractBuffer = (char *) pXParms;
   

  char output[40960]; // output buffer for building the output parameters
  pOutH = (uAHeader_t *) &output[0];  

  char * pGeneric = "";

  if ((pXParms -> flags & fGENERIC)   == fGENERIC )
    pGeneric =  "GENERIC";  

  resourceType = pInArgs -> rType;
  if (resourceType == GENERALRESOURCE)
  { // put the class name in the first field
    printf("RDEFINE %8.8s %*.*s %s\n",pInArgs-> myclass,
    pXParms-> lName,
    pXParms-> lName,
    pXParms -> data,
    pGeneric);

    }
    else 
    if (resourceType == USERID)
    {
    printf("ADDUSER %*.*s\n",
    pXParms-> lName,
    pXParms-> lName,
    pXParms -> data);  
    // printf("===Before doconnects\n");
    doConnects(pInArgs,pXParms);    
    // printf("===After doconnects\n");
    
    }
    else 
    if (resourceType == DATASET)
    {
    printf("ADDSD %*.*s %s\n",
    pXParms-> lName,
    pXParms-> lName,
    pXParms -> data,
    pGeneric);    
    }
     
  ///here 

  lName = pXParms -> lName;

  resourceType = pInArgs -> rType;
  if (resourceType == GENERALRESOURCE)
  { // put the class name in the first field
    pOutH -> lUserid  = lClass;
    pOutH -> lUserid  = pInArgs-> lClass; 
    memcpy(&pOutH-> userid,pInArgs-> myclass,8);
    }
    else 
    if (resourceType == USERID)
    {
    // put the userid in the first field
      pOutH -> lUserid  = pXParms -> lName;
      memcpy(&pOutH-> userid,& pXParms -> data[0],pXParms -> lName);
      //pConnectOutH -> lUserid  = pXParms -> lName;
      //memcpy(&pConnectOutH-> userid,& pXParms -> data[0],pXParms -> lName);      
    
    }
    else 
    if (resourceType == DATASET)
    {
    pOutH -> lUserid  = pInArgs-> lClass;  // class is "dataset ""
    memcpy(&pOutH-> userid,pInArgs-> myclass,8);
    // printHex(stdout,pOutH,8);
    }
    else

    {
    printf("mapit 102: Unsupport Resource type %i\n",resourceType);
    return 8;
    }
    
  pOutH -> flags = 0xe0; // no run, retcmds - add quote
  pOutH -> count = 0;  
  ///////////////////
  //  Start doing segments
  //////////////////
  pInSegment = (segment_t *)((char *) &(pXParms-> data) +lName);
  int lProfileName = pXParms-> lName;
  char * pProfileName  = (char *) & pXParms -> data;
  int iSegment; // iterator
  int segmentCount = pXParms ->segment_count;

  pOutF = (uAField_t *) &(pOutH -> first );  // preset this for the pOutS 
  //pConnectOutF = (uAField_t *) &(pConnectOutH -> first );  // preset this for the pOutS 
  int lData;
  int oData;
  for (iSegment= 0;iSegment < segmentCount;iSegment ++)
  {  //segments

    int nFields = pInSegment -> count;
    pOutH -> count = 0; // preset the output
    pOutF = (uAField_t *) &(pOutH -> first );  // preset this for the pOutS   
    //pConnectOutH -> count = 0; // preset the output
    //pConnectOutF = (uAField_t *) &(pConnectOutH -> first );  // preset this for the pOutS 
    
    if (debug > 0 )     
      printf("SEGMENT %8.8s nfields %i\n",pInSegment-> name, nFields);
    field_t * pInputField;
    pInputField = (field_t *) (pXtractBuffer  + pInSegment -> offset);
    pOutS = (uASegment_t *) pOutF;

    if (resourceType == GENERALRESOURCE ||  DATASET == resourceType)
    {
      if (memcmp(pInSegment-> name,"BASE    ",8) !=0 ) // match 
      {
        pOutF = makeSegment(pOutS,"BASE    ",'Y',pOutH);
        pOutF = addField("PROFILE ",'Y',lProfileName,pProfileName,pOutS,pOutF);
        pOutS = (uASegment_t *) pOutF; // move it on...
        pOutF = makeSegment(pOutS,pInSegment-> name,'Y',pOutH);   
      }
      else
      { 
        pOutF = makeSegment(pOutS,"BASE    ",'Y',pOutH);
        pOutF = addField("PROFILE ",'Y',lProfileName,pProfileName,pOutS,pOutF);    
      }
    }
    else 
    if (resourceType == USERID)
    {
      pOutF = makeSegment(pOutS,pInSegment-> name,'Y',pOutH); 
      //pConnectOutF = makeSegment(pConnectOutS,pInSegment-> name,'Y',pConnectOutH); 
    }   
    else
    {
      printf("mapit 173: Unsupported resource type\n");
      return 8;
    }
    //  for each field...    
    for (int iField = 0; iField < nFields;iField++)
    {
      char * pString = pXtractBuffer+ pInputField -> oData;
      lData = pInputField -> lData;
      oData = pInputField -> oData;
      if (debug > 1)
        {
          fprintf(stderr,"mapit: type %4.4x flag %8.8x lData %i oData %i\n",
                pInputField -> type,  
                pInputField -> flag,
                lData, 
                oData);
        }
      if (pInputField -> type == ftRepeatingGroupHeader ) //  repeating group header
      {  
        if (debug > 0 )              
          printf("Field: %8.8s Repeating Value Header\n",pInputField -> name); 
        nFields = nFields + lData * oData;
      }  
      else 
      if  (pInputField ->  flag && 0x40000000 == 0x40000000 )
      { 
        if (debug > 0 ) 
        {  
            printf("Field %8.8s Output only %*.*s\n",pInputField -> name,lData,lData,pString);
                      
        }
      }    
      else
     

      if (pInputField -> type == 0 ) // 
      { 
        if (debug > 0 )            
            printf("Field: %8.8s Value %*.*s.\n",pInputField -> name,lData,lData,pString); 
        //if (oo == 0)
          pOutF = addField(pInputField -> name,'Y',pInputField -> lData,pString,pOutS,pOutF);    
      }  
      else
      if (pInputField -> type == ftRepeatingGroupMember ) //  member repeating group
      {  

        char value = 'Y';
        //if (memcmp(pInputField -> name,"CLAUTH  ",8)==0)
        //  value = 'A'; // add it

        if (debug > 0 )              
          printf("Field: %8.8s Repeating Value %*.*s.\n",pInputField -> name,lData,lData,pString); 
        if (memcmp(&pInputField -> name,"CLAUTH  ",8) ==0 )
        {

            if (pCLAUTH == 0    )
            {
              //fprintf(stderr,"WHENDAYS, but pWHENDAYS is null\n");
              pCLAUTH  = pOutF;
              pOutF = addField("CLAUTH  ",'A',pInputField -> lData,pString,pOutS,pOutF);   // A for Add 
                                                     //Y not allowed on Alter
            }
            else
              pOutF = extendField(lData,pString,pCLAUTH);
        }
        else

        if (memcmp(&pInputField -> name,"WHENDAYS",8) ==0 )
        {

            if (pWhenDays == 0    )
            {
              //fprintf(stderr,"WHENDAYS, but pWHENDAYS is null\n");
              pWhenDays = pOutF;
              pOutF = addField("WHENDAYS",'Y',pInputField -> lData,pString,pOutS,pOutF);   
            }
            else
              pOutF = extendField(lData,pString,pWhenDays);
        }
        else
        {
          pOutF = addField(pInputField -> name,value,pInputField -> lData,pString,pOutS,pOutF); 
        }  
      }  
      else 
      if (pInputField -> type == ftBoolean) // boolean
      {
          if (debug > 0 )   
            printf("Field %8.8s Boolean\n",pInputField -> name);
          char flag;
          if  (pInputField -> flag && 0x8000000 == 0x8000000)  // true
            flag = 'Y';  // True
          else 
            flag = 'N';  // false
          if (memcmp(pInputField -> name,"MFAFLBK  ",8)==0)
            fprintf(stderr,"Userid field MFAFBLK %c skipped\n",flag);
        
          else
            pOutF = addField(pInputField -> name,flag,0,pString,pOutS,pOutF); 
      }
      pInputField ++;
    }
    // write out what we have, and set up for the next one  
    processOutput(pInArgs -> outFunctionCode,(char *) pOutH);
    pInSegment = (segment_t *) pInSegment -> next;// locate the next one

  }  // segments

 if (resourceType == GENERALRESOURCE)
  { // put the class name in the first field
    doPermits(pInArgs,pXParms); 
    } 
  // funcCode = inArgs.nextXtractFunction;
 // }
  return 0;
}  