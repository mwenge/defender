# Defender (1981) by Eugene Jarvis and Sam Dicker
<img src="https://user-images.githubusercontent.com/58846/125094063-d8c90380-e0ca-11eb-91b1-a3e80ef0e377.png" height=250><img src="https://user-images.githubusercontent.com/58846/125094574-4aa14d00-e0cb-11eb-9cf9-6dec489585f4.gif" height=250>

[<img src="https://img.shields.io/badge/Lastest%20Release-Jaguar-blue.svg">](https://github.com/mwenge/vlm/releases/download/v0.1/VirtualLightMachine.jag)

This is the source code for the Williams arcade game Defender.

The source code can be compiled into an executable that you can run in Windows or Linux.


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

We use `asm6809` to assembler the source code. 

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

You can run the VLM as follows using `t2k.exe`:
```sh
wine ./utils/t2k.exe
```


## Notes on the Source Code

The source code for Defender was originally retrieved from https://github.com/historical-source/defender


