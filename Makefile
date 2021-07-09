.PHONY: all clean

DIRS=bin

all: clean 

defender: 
	# Build notes from info.src ('DR J.' is presumably Eugene Jarvis!):
	#
	# "TO ASSEMBLE THE DEFENDER MESS
	#
	# RASM PHR2,DEFA2,DEFB2,AMODE0;-X (ELSE CREF SYMBOL OVERFLOW)
	# RASM PHR2,SAMEXPA7
	# RASM PHR2,DEFA2,DEFB2
	# TO GET THE DIAGS, CHAIN ALL.CF
	# LOAD IT ALL AND THEN PRAY IT WORKS
	# (NOTE: BEWARE OF ORDER OF LOADING
	#        LOOK OUT FOR THE SELECTED BLOCK SHIT
	#
	# DR J. 1/21/81"

	$(shell mkdir -p $(DIRS))
	#
	# Build amode1
	# The equivalent of: RASM PHR2,DEFA2,DEFB2,AMODE0;-X (ELSE CREF SYMBOL OVERFLOW)
	./asm6809/src/asm6809 -B src/phr6.src src/defa7.src src/defb6.src src/amode1.src\
	 		-l bin/defa7-defb6-amode1.lst  -o bin/defa7-defb6-amode1.o
	#
	# Build samexamp
	# The equivalent of: RASM PHR2,SAMEXPA7
	./asm6809/src/asm6809 -B src/phr6.src src/samexap7.src\
	    -l bin/samexap7.lst -o bin/samexap7.o
	#
	# Build defa7 and defb6
	# The equivalent of: RASM PHR2,DEFA2,DEFB2
	./asm6809/src/asm6809 -B src/phr6.src src/defa7.src src/defb6.src\
 			-l bin/defa7-defb6.lst -o bin/defa7-defb6.o
	#
	# Build blk71
	./asm6809/src/asm6809 -B src/blk71.src -l bin/blk71.lst -o bin/blk71.o
	#
	# Build roms
	./asm6809/src/asm6809 -B src/mess0.src src/romf8.src src/romc0.src src/romc8.src\
	 		-l bin/roms.lst -o bin/roms.o

	./ChainFilesToRom.py defender.rom\
		bin/defa7-defb6-amode1.o,0x0001,0x8000,0x1000,"amode"\
		bin/defa7-defb6-amode1.o,0xaeea,0x8ee9,0x0117,"amode tail"\
		bin/roms.o,0x0001,0x9000,0x0c50,"mess0"\
		orig/defender.rom,0x9c51,0x9c50,0x03b0,"rom without source"\
		bin/roms.o,0xa000,0x9fff,0x1000,"romf8 romc0 romc8"\
		orig/defender.rom,0xaff1,0xaff0,0x0010,"16 stray bytes"\
		bin/blk71.o,0x0001,0xb000,0x1000,"blk71"\
		orig/defender.rom,0xb773,0xb772,0x0006,"6 stray bytes"\
		bin/roms.o,0xa779,0xb778,0x0088,"romc0"\
		orig/defender.rom,0xb7f1,0xb7f0,0x0001,"1 stray byte"\
		orig/defender.rom,0xc1e1,0xc1e0,0x0e20,"rom without source"\
		bin/defa7-defb6.o,0x0371,0xd000,0x2c60,"defa7 defb6"\
		bin/samexap7.o,0x0001,0xfc60,0x02f8,"samexap7"\
		bin/defa7-defb6.o,0x32c9,0xff58,0x0230,"defa7 defb6 2"

	echo "e15b3930a1e827bc967a3e84b3be259a  defender.rom" | md5sum -c

clean:
	-rm bin/*.o
	-rm bin/*.lst
	-rm bin/*.asm
