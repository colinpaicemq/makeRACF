# compile the source and create a file extract, and a
# load module in the specified load library
name=extract
loadmodule="//'COLIN.LOAD(EXTRACT)'"
# rm CEE*
# rm $name
# rm $name.o

export _C89_CCMODE=1
p1="-Wc,arch(8),target(zOSV2R3),list,source,ilp32,gonum,asm,float(ieee)"
p5="                           -I.                            "
p7="-Wc,ASM,ASMLIB(//'SYS1.MACLIB')                   "
p8="-Wc,LIST(c.lst),SOURCE,NOWARN64,XREF,SHOWINC -Wa,LIST(133),RENT"
# compile it
xlc  $p1 $p5 $p7 $p8   -c $name.c -o $name.o
# bind it to local program and load module
l1="-Wl,LIST,MAP,XREF,AC=1 "
/bin/xlc $name.o  -o $name   -V   $l1    1>extract.list
extattr +a $name
/bin/xlc $name.o  -o $loadmodule  -V $l1    1>lm.list
# ./$name  G   SYS1        1
# ./$name  D  "ADCD*"      1
# ./$name  RDATALIB "START*"  0
# ./$name U COLIN          0
