Compile romgrab as follows:
```
gcc -o romgrab romgrab.c
```
Then run:
```
./romgrab  DEFENDER.EXE -d -v -o defender.rom
```
This will extract the uncompressed ROM images to `defender.rom`.
