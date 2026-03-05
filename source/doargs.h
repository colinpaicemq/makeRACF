void help();

void setup(inArgs * pInArgs,
          char * myclass,
          int lClass,
          char * type,
          enum requestType_t rType,
          char first,
          char next,
          char out )
      {
        
      memcpy(&pInArgs-> myclass[0],myclass,8); // preset this - it is ignored for data set
      pInArgs -> lClass = lClass;
      pInArgs -> pType = type;
      pInArgs -> rType = rType;
      pInArgs -> firstXtractFunction  = first;  //  extract Next dataset  resource
      pInArgs -> nextXtractFunction = next; 
      // pInArgs-> firstXtractFunction = 0x23;
      pInArgs -> outFunctionCode = out;; // alter dataset resource profile
    } 
int doArgs(int argc, char *argv??(??),inArgs * pInArgs)
{

  // printf("????%i %c\n",strlen(argv[1]),argv[1][0]);
  char blank = ' ';
  if ( argc < 3)
  {
    help();
    return 12;
  }
  pInArgs -> debug = 0;
  if ( argc > 3 )
    {
      pInArgs -> debug = strtol(argv[3],NULL,10);
      // printf("Debug %i\n", pInArgs -> debug );
    }  
 
  // else  
  if (strlen(argv[1]) == 1 &&  argv[1][0]== 'D' || argv[1][0]== 'd'   )
    setup(pInArgs,"DATASET ",7,"Dataset",DATASET,0x00,0x23,0x12);
    
    #ifdef no  

    {
      memcpy(&pInArgs-> myclass[0],"DATASET ",8); // preset this - it is ignored for data set
      pInArgs -> lClass = 7;
      pInArgs -> pType = "Dataset";
      pInArgs -> rType = DATASET;
      pInArgs -> firstXtractFunction  = 0x22;  //  extract Next dataset  resource
      pInArgs -> nextXtractFunction = 0x23;
      // pInArgs-> firstXtractFunction = 0x23;
      pInArgs -> outFunctionCode = 0x12; // alter dataset resource profile
    
    }
    #endif    
  else 
  if (strlen(argv[1]) == 1 &&( argv[1][0]== 'U' || argv[1][0]== 'u'   ))
    {
      int lClass = strlen(argv[2]);
      if (lClass > 8)
      {
        help();
        return 12;
      } 
      memcpy(&pInArgs-> myclass[0],"        ",8); // preset this
      memcpy(&pInArgs-> myclass[0],&argv[2][0],lClass); // what was specigied
      pInArgs -> lClass = lClass;
      pInArgs -> pType = "Userid";
      pInArgs -> rType = USERID;
      pInArgs -> outFunctionCode = 0x01; // for users
      pInArgs -> outFunctionCode = 0x03; // alter userid
      pInArgs -> firstXtractFunction  = 0x19;  // extract user
      pInArgs -> nextXtractFunction   = 0x1a;  // Next userid

    }
 
  else


  if (strlen(argv[2]) <= 8) 
  {
    int lClass = strlen(argv[1]);
    if (lClass > 8)
    {
        help();
        return 12;
    }
    char myClass[8]; 
    memcpy(&myClass[0],"        ",8); // preset this
    memcpy(&myClass,&argv[1][0],lClass); // what was specigied
    setup(pInArgs,myClass,lClass,"Resource",GENERALRESOURCE,0x1f,0x20,0x0e);

  }
  else
  {
     help();
    return 12;
  }
  //////////////////////////////////
  // now process what the object is
  //////////////////////////////////

  pInArgs -> pWhich = argv[2];
  pInArgs -> lWhich = strlen(argv[2]);
  //printf("==72a %s.%s\n",argv[1],argv[2]);
  //printf("===72==%i.%s.\n",pInArgs -> lWhich, pInArgs -> pWhich);
  // check to see if it is generic

  if (argv[2][0] == '*' )
  {
    printf("doargs has *\n");
    pInArgs -> star = 1;
    pInArgs -> pWhich = &blank;
    pInArgs -> lWhich = 1;
  }
  else
  if (argv[2][pInArgs -> lWhich-1] == '*' )
  {
    if (debug > 0)  
      printf("star string\n");
    pInArgs -> star = 1;
    pInArgs -> lWhich -= 1; // remove the * from the end
  }
  //printf("===79==%i\n",pInArgs -> lWhich);
  if (pInArgs -> debug > 0)
    printf("Do args output parameters:%s.%s. star %i debug %i\n",
          pInArgs -> pType,
          pInArgs -> pWhich,
          pInArgs -> star,
          pInArgs -> debug
        );
  return 0;  
  
}
void help()
{ 
  printf("Parameters\n");
  printf("type what <( <data> <debug > ");
  printf("Where:\n");
  printf("type is one of:\n");
  printf("   D|d  for Dataset\n");
  printf("   U|u  for userid\n");
  printf("   G|g  for group\n");
  printf("   SETR  for RACF options\n");
  printf("   class such as STARTED or started\n");
  printf("what is a resource name such as :\n");
  printf("   a userid COLIN, COLIN*, or *.  ");
  printf("      COLIN will do just that userid\n");
  printf("      COLIN* will do all those starting with COLIN\n");
  printf("      * All userids n");
  return;
}

#ifdef no
 if (strlen(argv[1]) == 4 && memcmp(argv[1],"SETR",4) == 0) 
  {
    pInArgs -> pType = argv[1];

    pInArgs -> rType = SETR; 

    pInArgs-> firstXtractFunction  = 0x16;  //  extract general resource
    pInArgs-> nextXtractFunction   = 0x16;
    pInArgs-> outFunctionCode = 0x15; // alter general resource profile
    pInArgs -> star = 0;
    pInArgs -> pWhich = &blank;
    pInArgs -> lWhich = 1;
    if (argc>2)
      pInArgs -> debug = strtol(argv[2],NULL,10);

    pInArgs -> debug = 1;


  }
#endif

  #ifdef delete
  if (strlen(argv[1]) == 1 && (argv[1][0]== 'G' || argv[1][0]== 'g'   ))
    {
      int lClass = strlen(argv[2]);
      if (lClass > 8)
      {
        help();
        return 12;
      } 
      memcpy(&pInArgs-> myclass[0],"        ",8); // preset this
      memcpy(&pInArgs-> myclass[0],&argv[2][0],lClass); // what was specigied
      pInArgs -> lClass = lClass;
      pInArgs -> pType = "Group";
      pInArgs -> rType = GROUP;
      pInArgs -> outFunctionCode = 0x06; // add
      pInArgs -> outFunctionCode = 0x08; // alter 
      pInArgs -> firstXtractFunction  = 0x1b;  // extract user
      pInArgs -> nextXtractFunction   = 0x1c;  // Next userid

    }
   //  (strlen(argv[1]) == 1 )
     
  else
  #endif


      #ifdef delete
    pInArgs -> pType = argv[1];
    //pInArgs -> pType = "Resource";  
    pInArgs -> rType = GENERALRESOURCE; 

    pInArgs-> firstXtractFunction  = 0x22;  //  extract general resource
    pInArgs-> nextXtractFunction   = 0x23;
    pInArgs-> outFunctionCode = 0x0E; // alter general resource profile 
    #endif
