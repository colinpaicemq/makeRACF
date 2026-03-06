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
# To install it

Either 

## use github

- clone the repository 
- use the Unix command cp extract  "//'COLIN.LOAD(EXTRACT)'"  to 
create the loadmole from the file.
- apf authorise the PDS.

## Use XMIT

- Download the extract.xmit.bin to your workstation
- Upload it to z/OS using RECFM FB lRECL 80 BLKSIZE 3200, bin
- Use tso receive indsn(...)

## execute it

The PDS needs to be APF authorised.

Ive used the operator command

```
setprog apf,add,dsn=COLIN.LOAD,SMS
```

Use JCL

```
//IBMSTA   JOB 1,MSGCLASS=H 
//S1 EXEC PGM=E2,PARM='U START1 0' 
//STEPLIB DD DISP=SHR,DSN=COLIN.LOAD 
//SYSPRINT DD DISP=SHR,DSN=COLIN.MIG.JCL(USTART1) 
//SYSOUT   DD SYSOUT=* 
//SYSERR   DD SYSOUT=* 
```

Where the parameters are

### for a userid

- U userid  <n>
- u userid* <n>

### for a dataset 

-D COLIN.ABCD    <n>
-d COLIN.* <n>

### for a general resource group

- class CSQ <n>
- class CSQ* <n>

Where class is STARTED to similar class name

n is an optional debug level defaults to 0.  Other values are 1 and 2

### Output 
The output is stored in //SYSPRINT DD DISP=SHR,DSN=... and can be used 

```
//IBMUSERT JOB 1,MSGCLASS=H 
//S1  EXEC PGM=IKJEFT01,REGION=0M 
//SYSPRINT DD SYSOUT=* 
//SYSTSIN  DD DISP=SHR,DSN=COLIN.MIG.JCL(USTART1) 
//SYSTSPRT DD SYSOUT=* 
```
