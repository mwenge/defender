# Defender (1981) by Eugene Jarvis and Sam Dicker
<img src="https://user-images.githubusercontent.com/58846/125094063-d8c90380-e0ca-11eb-91b1-a3e80ef0e377.png" height=250><img src="https://user-images.githubusercontent.com/58846/125094574-4aa14d00-e0cb-11eb-9cf9-6dec489585f4.gif" height=250>

[<img src="https://img.shields.io/badge/Lastest%20Release-Defender-blue.svg">](https://github.com/mwenge/defender/releases/download/v0.1/defender.rom)

This is the source code for the Williams arcade game Defender.

The source code can be compiled into an executable that you can run it in Windows or Linux.


<!-- vim-markdown-toc GFM -->

* [Build Instructions](#build-instructions)
  * [Build Requirements](#build-requirements)
  * [Build the assembler toolchain](#build-the-assembler-toolchain)
  * [Build Defender](#build-defender)
* [Play Defender](#play-defender)
* [Notes on the Source Code](#notes-on-the-source-code)

<!-- vim-markdown-toc -->
## Build Instructions

### Build Requirements
```sh
sudo apt install build-essentials wine python3
```

### Build the assembler toolchain

We use [`asm6809`](https://www.6809.org.uk/asm6809/) to assembler the source code. 

First you must run the following to set up the git submodules containing the assembler toolchain:

```sh
git submodule init
git submodule update
```

Now you can build the toolchain, as follows:

```sh
cd asm6809
./autogen.sh
./configure
make 
cd ..
```

### Build Defender

To build the rom image `defender.rom`:
```sh
make defender
```

## Play Defender

Coming soon.

## Notes on the Source Code

The source code in [src](src) was originally retrieved from https://github.com/historical-source/defender. It is
the code for the 'Red Label' version of the game. There were four versions of the game released: White Label, Blue Label,
Green Label, and Red Label; in that order. Each release was a circuit board with the compiled code split across a 
number of different ROM chip. This image of the Red Label ROM board from [Scott Tunstall's site](https://www.robotron-2084.co.uk/techwilliamshardwareid.html)
gives you an idea of what such a board looks like:

<img src="orig/D8572.jpg" size=250>

If you compare this image to [the file listing for the Red Label roms](orig/defender-redlabel) you'll notice that the missing chip on the board corresponds
to a missing file `defend.5`:
```sh
[robert@mwenge-desktop defender-redlabel (master)]$ ls -al
total 64
drwxrwxr-x 2 robert robert 4096 Jul 11 10:32 .
drwxrwxr-x 8 robert robert 4096 Jul 11 11:02 ..
-rw-rw-r-- 1 robert robert 2048 Dec 24  1996 defend.1
-rw-rw-r-- 1 robert robert 2048 Dec 24  1996 defend.10
-rw-rw-r-- 1 robert robert 2048 Dec 24  1996 defend.11
-rw-rw-r-- 1 robert robert 2048 Dec 24  1996 defend.12
-rw-rw-r-- 1 robert robert 4096 Dec 24  1996 defend.2
-rw-rw-r-- 1 robert robert 4096 Dec 24  1996 defend.3
-rw-rw-r-- 1 robert robert 2048 Dec 24  1996 defend.4
-rw-rw-r-- 1 robert robert 2048 Dec 24  1996 defend.6
-rw-rw-r-- 1 robert robert 2048 Dec 24  1996 defend.7
-rw-rw-r-- 1 robert robert 2048 Dec 24  1996 defend.8
-rw-rw-r-- 1 robert robert 2048 Dec 24  1996 defend.9
```

So each filed `defend.[x]` corresponds to a matching chip on the board and
because there is a missing chip in slot 5 we have no `defend.5` in the rom dump
listing.

In the [Defender product documentation](https://www.robotron-2084.co.uk/manuals/defender/defender_later_pcb_drawing_set.pdf) a 
chart lists the part numbers for each chip and confirms that IC5 (i.e. `defend.5`) is unused:

<img src="orig/RedLabelRomChart.png" size="300">

When we assemble the Defender source with `make roms` we create a bunch of
object files and then split them across the 11 files to match the 11 in the Red
Label ROM dump listing above.

Eugene Jarvis left a slightly cryptic note to how we assemble the source in [`info.src`](src/info.src):

```
 TO ASSEMBLE THE DEFENDER MESS

RASM PHR2,DEFA2,DEFB2,AMODE0;-X (ELSE CREF SYMBOL OVERFLOW)
RASM PHR2,SAMEXPA7
RASM PHR2,DEFA2,DEFB2
TO GET THE DIAGS, CHAIN ALL.CF
LOAD IT ALL AND THEN PRAY IT WORKS
(NOTE: BEWARE OF ORDER OF LOADING
       LOOK OUT FOR THE SELECTED BLOCK SHIT

DR J. 1/21/81

```

Since the `RASM` assembler is no longer available to us we use [`asm6809`](asm6809) instead. Fortunately this
does a good job of assembling the source faithfully and [only very minor modifications to the source files are
required to produce binaries](src). We recreate the steps in Eugene's notes as follows:

```
	# Build amode1 # The equivalent of: RASM PHR2,DEFA2,DEFB2,AMODE0;-X (ELSE CREF SYMBOL OVERFLOW)
	./asm6809/src/asm6809 -B src/phr6.src src/defa7.src src/defb6.src src/amode1.src\
	 		-l bin/defa7-defb6-amode1.lst  -o bin/defa7-defb6-amode1.o
```
This is the main game code with the attract mode module compiled in. `phr6.src` is a file containing definitions,
while `defa7.src` and `defb6/src` contain the main game code; `amode1.src` contains the attract mode code.

We also have to build a version of this game code without the attract mode module:

```

	# Build defa7 and defb6
	# The equivalent of: RASM PHR2,DEFA2,DEFB2
	./asm6809/src/asm6809 -B src/phr6.src src/defa7.src src/defb6.src\
 			-l bin/defa7-defb6.lst -o bin/defa7-defb6.o
```

The final module the notes mention is `samexpa7` a bunch of explosion routines added by Sam Dicker:

```
	# Build samexamp
	# The equivalent of: RASM PHR2,SAMEXPA7
	./asm6809/src/asm6809 -B src/phr6.src src/samexap7.src\
	    -l bin/samexap7.lst -o bin/samexap7.o
```

Eugene's notes are much less clear on how we go about assembling the reamining source files:

```
mess0.src
blk71.src
romf8.src
romc0.src
romc8.src
```

However the way he lists them in the notes turns out to provide a clue to the order in which they
shoud be assembled:

```
PHR6.SRC	  636 LINES
DEFA7.SRC	3,375 LINES
DEFB6.SRC	2,252 LINES
AMODE1.SRC	1,310 LINES
BLK71.SRC	  723 LINES
SAMEXAP7.SRC	  382 LINES
MESS0.SRC	  955 LINES
ROMF8.SRC	  692 LINES
ROMC0.SRC	  925 LINES
ROMC8.SRC	  839 LINES
```

We assemble the last four together in the order that they appear above:

```
	# Build roms
	./asm6809/src/asm6809 -B src/mess0.src src/romf8.src src/romc0.src src/romc8.src\
	 		-l bin/roms.lst -o bin/roms.o
```

And we assemble `blk71.src` by itself:

```
	# Build blk71
	./asm6809/src/asm6809 -B src/blk71.src -l bin/blk71.lst -o bin/blk71.o
```

There were a few modifications to the source required along the way to get this to work, mainly
to work around the fact that `RASM` seems to have allowed you to assemble code sections into overlapping
memory segments. For example both `amode1.src` and `defa7.src` want to assemble into position `$C000` in
the ROM, meaning that one will overwrite the other. This explains why the main game files get assembled
twice, once with attract mode (`amode1.src`) and once without: they wanted a binary with some segments
overwritten with attract mode features and one without. We achieve this ourselves by modifying the source
to compile and place the attract mode code to position `$2000` in memory, and when we later split the
object files into the `defend[x].bin` files pick the chunk of code we're interested in.


