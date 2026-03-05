# makeRACF
Extract RACF profile and make a command string
You pass a profile name, and the program generates the RACF command
to make that profile.  For example
Extract U COLIN gives
```
ADDUSER COLIN

CONNECT COLIN   GROUP(IZUADMIN)  UACC(READ)  SPECIAL  AUDITOR  - 
    REVOKE(01/01/27)  - 
    RESUME(01/02/27) 
CONNECT COLIN   GROUP(IZUUSER)  UACC(NONE) 
CONNECT COLIN   GROUP(SYS1)  UACC(NONE)  - 
    REVOKE(01/01/27)  - 
    RESUME(01/02/27) 
CONNECT COLIN   GROUP(TEST)  UACC(NONE) 
CONNECT COLIN   GROUP(ZWEADMIN)  UACC(NONE) 
ALTUSER -
  COLIN -
  OWNER (COLIN) -
  NOADSP -
  NOOPERATIONS -
  NOGRPACC -
  NAME ('CCPAICE') -
  DFLTGRP (TEST) -
  DATA ('COLIN''S WITH A QUOTE') -
  NOAUDITOR -
  CLAUTH (CSFSERV) -
  NOREST -
-
  NOROAUDIT -
  WHEN( -
    DAYS (SUNDAY -
      MONDAY -
      TUESDAY -
      WEDNESDAY -
      THURSDAY -
      FRIDAY -
      SATURDAY) -
    TIME (ANYTIME))
ALTUSER -
  COLIN -
  TSO (ACCTNUM ('ACCT#') -
    COMMAND ('ex ''colin.zlogon.clist''') -
    PROC (ISPFPROC) -
    SIZE (2096128) -
    MAXSIZE (2096128) -
    USERDATA (0000) -
    UNIT (3390))
ALTUSER -
  COLIN -
  OMVS (UID (990021) -
    HOME ('/u/tmp/zowet/colin') -
    PROGRAM ('/u/zopen/usr/local/bin/bash') -
    MMAPAREAMAX (16777216) -
    SHMEMMAX (300M))
```

