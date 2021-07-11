
/*
 * PROJECT: romgrab - Utility to grab the ROM files from the Williams
Arcade executables
 * FILE: romgrab.c
 * AUTHOR: Jonathan Wolff
 * LAST UPDATE: 29/1/96 12:21:45 pm
   SKR - 1/2/96 removed & from memmap assignments
 */

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#ifdef applec
#include <CursorCtl.h>
#endif

/*
 * packet types for RLE
 */
enum {
  RLE_REPEAT = 0xb0,
  RLE_REPEAT_END = 0xb1,
  RLE_COPY = 0xb2,
  RLE_COPY_END = 0xb3
};

/*
 * Entries for memory map
 */
typedef struct MemMapEntry {

  long int size;        // 0 is last entry
  long int position;      // -1 is discarded section

} MemMapEntry;

MemMapEntry default_memmap[] = {
  0x9000, 0x0,
  0x3e20,  0xc1e0,
  0
};

MemMapEntry defender_memmap[] = {
  0x3800, 0x8000,
  0x3000, 0xD000,
  0x4800, -1,
  0xe20,  0xc1e0,
  0
};

MemMapEntry sinistar_memmap[] = {
  0x9000, 0x0,
  0xe20,  0xc1e0,
  0x2000,  0xe000,
  0
};


/*
 * Parameters
 */
char *inputname = NULL;
char *outputname = NULL;
MemMapEntry *memmap = default_memmap;
int verbose = 0;

/*
 * Global variables
 */
char *appname = NULL;
int errlevel = 0;
FILE *inputfile = NULL;
FILE *outputfile = NULL;

unsigned char *inputdata = NULL;
unsigned char *outputdata = NULL;
  
long int inputsize, inputpos;
long int outputsize, outputpos;

int memmappos;

MemMapEntry *custom_memmap;

void error(int, char *, ...);
void usage(void);
void init(void);
void process(void);
void cleanup(void);

int main(int argc, char *argv[])
{
  char *arg, *temp, *entry, *pos;
  int i, entries;

  /*
   * Parse arguments
   */
  appname = argv[0];

  while(argv++,--argc)

    if (**argv == '-') {
      /*
       * Got one or more flags
       */
      for(arg = *argv+1; *arg; arg++) {

        if (strchr("iom",*arg) && ((--argc == 0) || **(++argv) == '-')) {
          error(5, "the %c option requires an argument\n", *arg);
          usage();
        }
        switch (*arg) {
        case '?':
          usage();

        case 'i':
          if (inputname != NULL) {
            error(5, "too many input files specified\n");
            usage();
          }
          inputname = *argv;
          break;
          
        case 'o':
          if (outputname != NULL) {
            error(5, "too many output files specified\n");
            usage();
          }
          outputname = *argv;
          break;
          
        case 'v':
          verbose = 1;
          break;
          
        case 'd':
          memmap = defender_memmap;
          break;
          
        case 's':
          memmap = sinistar_memmap;
          break;
          
        case 'm':
          
          for (entries = 1, pos = *argv; *pos != '\0'; pos++)
            if (*pos == ',')
              entries++;
          
          if (custom_memmap != NULL)
            free(custom_memmap);
            
          custom_memmap = calloc(entries + 1, sizeof(MemMapEntry));
          if (custom_memmap == NULL)
            error(10, "couldn't allocate buffer for memory map\n");

          temp = strcpy(malloc(strlen(*argv) + 1), *argv);

          for (i = 0, entry = strtok(temp, ","); i < entries; i++, entry = strtok(NULL, ",")) {
            switch (sscanf(entry, "%lx@%lx", &custom_memmap[i].size, &custom_memmap[i].position)) {
            case 1:
              custom_memmap[i].position = -1;
              break;

            case 2:
              break;

            default:
              error(5, "malformed memory map definition\n");
            }              
          }
          
          free(temp);

          memmap = custom_memmap;
          break;
          
        default:
          error(5, "unknown option '%c'\n", *arg);
          usage();
        }
      }
    } else
      if (inputname == NULL)
        inputname = *argv;
      else if (outputname == NULL)
        outputname = *argv;
      else {
        error(5, "too many parameters\n");
        usage();
      }

  init();
  process();
  cleanup();

  error(0, "done!\n");
  return errlevel;
}

void usage(void)
{
  fprintf(stderr,
"Usage: %s [-i] inputfile [[-o] outputfile] [-v] [-d|-s|-m size[@pos][,size[@pos]]...]\n"
"\t-v\tenables verbose output\n"
"\t-d\tuse Defender memory map (3800@8000,3000@d000,4800,9e20@c1e0)\n"
"\t-s\tuse Sinistar memory map (9000@0,e20@c1e0,1000@e000)\n"
"\t-m\tuse custom memory map\n"
"\n"
"\tdefault output filename is williams.rom\n"
"\tdefault memory map is 9000@0,3e20@c1e0\n"
"\n", appname
  );

  cleanup();
  exit(errlevel);
}

void error(int level, char *format, ...)
{
  va_list arg;

  fprintf(stderr, "# %s: ", appname);

  va_start(arg, format);
  vfprintf(stderr, format, arg);
  va_end(arg);
  
  if (level > errlevel) {
    errlevel = level;
    if (level >= 10) {
      cleanup();
      exit(level);
    }
  }
}

void init(void)
{
  long int i;

  if (inputname)
    inputfile = fopen(inputname, "rb");

  if (inputfile == NULL)
    error(10, "couldn't open input file\n");

  fseek(inputfile, 0, SEEK_END);
  inputsize = ftell(inputfile);
  fseek(inputfile, 0, SEEK_SET);
  
  inputdata = malloc(inputsize);
  if (inputdata == NULL)
    error(10, "couldn't allocate buffer for input file\n");
  
  if (fread(inputdata, inputsize, 1, inputfile) != 1)
    error(10, "couldn't read input file\n");
  
  inputpos = inputsize;

  if (!outputname)
    outputname = "williams.rom";

  outputfile = fopen(outputname, "wb");
  if (outputfile == NULL)
    error(10, "couldn't open output file\n");

  memmappos = 0;
  outputsize = 0;
  while (memmap[memmappos].size > 0) {
    outputpos = memmap[memmappos].position + memmap[memmappos].size;
    if (memmap[memmappos].position >= 0 && outputpos > outputsize)
      outputsize = outputpos;
    memmappos++;
  }
  memmappos--;

  outputdata = malloc(outputsize);
  if (outputdata == NULL)
    error(10, "couldn't allocate buffer for output file\n");

  for (i = 0; i < outputsize; i++)
    outputdata[i] = 0xff;
}

unsigned char cur_byte(void)
{
  if (inputpos < 1)
    error(10, "unexpected start of file\n");

  return inputdata[inputpos - 1];
}

unsigned char get_byte(void)
{
  if (inputpos < 1)
    error(10, "unexpected start of file\n");

  return inputdata[--inputpos];
}

unsigned int get_word(void)
{
  if (inputpos < 2)
    error(10, "unexpected start of file\n");

  inputpos -= 2;
  return ((unsigned)(inputdata[inputpos + 1] << 8) + inputdata[inputpos]);
}

void put_byte(unsigned char b)
{
  if (memmappos < 0) {
    if (verbose && outputpos-- == 0)
      printf("EXE...");
    return;
  }
  
  if (verbose && memmap[memmappos].position >= 0 && outputpos ==
memmap[memmappos].position + memmap[memmappos].size)
    printf("%04X... ", outputpos - 1);

  outputpos--;
  if (memmap[memmappos].position >= 0)
    outputdata[outputpos] = b;
  
  if (outputpos == memmap[memmappos].position) {

    if (verbose && memmap[memmappos].position >= 0)
      printf("...%04X ", outputpos);

    memmappos--;
    if (memmappos < 0)
      outputpos = 0;
    else
      outputpos = memmap[memmappos].position + memmap[memmappos].size;
  }
}

void process(void)
{
  unsigned int count;
  unsigned char byte;
  int finished = 0;
  
  /*
   * Locate end of compressed image - this seems to be a bit bodgy
   */
  while (1) {
    while (cur_byte() != 0x00)
      get_byte();
    get_byte();
    if (cur_byte() == 0x40) {
      get_byte();
      break;
    }
  }

  if (verbose) {
    printf("%05x-%05x: ..            crap\n", inputpos + 2, inputsize - 1);
    printf("%05x-%05x: 40 00         marks end of compressed image\n",
inputpos, inputpos + 1);
  }

  /*
   * Skip the checksum/size/whatever it is & the padding
   */
  get_word();
  if (verbose)
    printf("%05x-%05x: %02x %02x         ?\n", inputpos, inputpos + 1,
inputdata[inputpos], inputdata[inputpos + 1]);

  if (cur_byte() == 0xff) {

    count = 0;
    while (cur_byte() == 0xff) {
      count++;
      get_byte();
    }
  
    if (verbose)
      printf("%05x-%05x: ff ..         padding\n", inputpos, inputpos + count
- 1);
  }

  while (!finished) {

    switch(get_byte()) {
    
      case RLE_REPEAT_END:
      
         finished=1;
         
      case RLE_COPY_END:
      
         finished=1;
      
         
    case RLE_REPEAT:
      count = get_word();
      byte = get_byte();
      
      if (verbose)
        printf("%05x-%05x: %02x %02x %02x %02x   repeat   ", inputpos,
inputpos + 4, inputdata[inputpos], inputdata[inputpos + 1],
inputdata[inputpos + 2], inputdata[inputpos + 3]);

      while (count-- > 0)
        put_byte(byte);

      if (verbose)
        printf("\n");

      break;

    case RLE_COPY:

      count = get_word();
      
      if (verbose)
        printf("%05x-%05x: .. %02x %02x %02x   copy     ", inputpos - count,
inputpos + 2, inputdata[inputpos], inputdata[inputpos + 1],
inputdata[inputpos + 2]);

      while (count-- > 0)
        put_byte(get_byte());

      if (verbose)
        printf("\n");

      break;

    default:
      error(10, "unexpected RLE packet 0x%02x\n", inputdata[inputpos]);
    }

    /*
     * Spin the beachball from time to time...
     */
#ifdef applec
    SpinCursor(32);
#endif
  }
  
  if (verbose)
    printf("00000-%05x: ..            crap\n", inputpos);

  if (fwrite(outputdata, outputsize, 1, outputfile) != 1)
    error(10, "couldn't write output file\n");
  
  error(0, "written ROM image to '%s'\n", outputname);
}

void cleanup(void)
{
  if (inputfile != NULL)
    fclose(inputfile);

  if (outputfile != NULL)
    fclose(outputfile);

  if (inputdata != NULL)
    free(inputdata);

  if (outputdata != NULL)
    free(outputdata);

  if (custom_memmap != NULL)
    free(custom_memmap);
}


