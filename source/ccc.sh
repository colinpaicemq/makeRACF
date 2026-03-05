name=extract
rm CEE*
rm $name
rm $name.o
# lp64 ilp32

export _C89_CCMODE=1
p1="-Wc,arch(8),target(zOSV2R3),list,source,lp64,gonum,asm,float(ieee)"
p1="-Wc,arch(8),target(zOSV2R3),list,source,ilp32,gonum,asm,float(ieee)"
p5="                           -I.                            "
p7="-Wc,ASM,ASMLIB(//'SYS1.MACLIB'),INFO(ALL)         "
p7=" "
p7="-Wc,ASM,ASMLIB(//'SYS1.MACLIB')                   "
p8="-Wc,LIST(c.lst),SOURCE,NOWARN64,XREF,SHOWINC -Wa,LIST(133),RENT"

xlc  $p1 $p5 $p7  $p8   -c $name.c -o $name.o
rc=$?
echo "Return code "$?
echo "RC="rc
l1="-Wl,LIST,MAP,XREF,AC=1 "
/bin/xlc $name.o  -o $name   -V   $l1    1>a
/bin/xlc $name.o  -o "//'COLIN.LOAD(EXTRACT)'"  -V $l1    1>a
extattr +a $name
#/$name  G   SYS1        1
#/$name  D  "ADCD*"      1
./$name  RDATALIB "START*"  0
#/$name U COLIN          0
