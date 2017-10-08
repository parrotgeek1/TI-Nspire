// C++11 code compilable with e.g. GCC 4.6+ or Clang.

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <inttypes.h>
#include <unistd.h>

//#define DEBUG	0
#define nullptr 0

using namespace std;

//typedef unsigned char		uint8_t;
//typedef unsigned short int	uint16_t;
//typedef unsigned long int	uint32_t;
#ifndef UINT16_MAX
#define UINT16_MAX 0xFFFF
#endif
#ifndef PRIX16
#define PRIX16        "X"
#define PRIu32        "lu"
#define PRIX32        "lX"
#endif

FILE * image = nullptr;
FILE * output = nullptr;
char * output_filename = nullptr;
uint8_t * osversion = nullptr;
uint8_t * imgtype = nullptr;


// Utility functions

static uint8_t * malloc_or_exit(size_t len) {
    uint8_t * ptr = (uint8_t *)malloc(len);
    if (!ptr) {
        printf("Cannot allocate 0x%lX (%lu) bytes of memory\n", (unsigned long)len, (unsigned long)len);
        exit(1);
    }
    return ptr;
}

static uint8_t get8bits(void) {
    int byte = getc(image);
    if (byte < 0) {
        puts("get8bits: reached EOF");
        // Fall through, to cope with files not }ing with FFF(0).
    }
    return byte;
}

static void put8bits(uint8_t byte) {
    putc(byte, output);
}

static uint16_t get16bits(void) {
    uint8_t hi = get8bits();
    return ((uint16_t)hi << 8) | get8bits();
}

static void put16bits(uint16_t hi) {
    put8bits(hi>>8);
    put8bits(hi&0xFF);
}

static uint32_t get32bits(void) {
    uint16_t hi = get16bits();
    return ((uint32_t)hi << 16) | get16bits();
}

static void put32bits(uint32_t hi) {
    put16bits(hi>>16);
    put16bits(hi&0xFFFF);
}

/*static uint64_t get64bits(void) {
    uint32_t hi = get32bits();
    return ((uint64_t)hi << 32) | get32bits();
}*/

static uint8_t * getstr(uint32_t len) {
    uint32_t i;
    uint8_t * str = (uint8_t *)malloc_or_exit(len+1);

    for (i = 0; i < len; i++) {
        str[i] = get8bits();
    }
    str[len] = 0;
    return str;
}


// Decompression for boot2/diags


static int getbits(uint32_t n) {
    static uint32_t buf = 0, bits = 0;
	if(!n) bits = 0;					// BOOT2403
    while (bits < n) {
        buf = buf << 8 | get8bits();
        bits += 8;
    }
    bits -= n;
    return buf >> bits & ((1 << n) - 1);
}

static uint32_t decompress(void) {
    uint32_t size = get32bits();
	uint32_t dsize = 0;
    uint16_t common[64];
    uint32_t common_counts[64];

    for (uint32_t i = 0; i < sizeof(common)/sizeof(common[0]); i++) {
        common[i] = get16bits();
        common_counts[i] = 0;
    }

    for (; size > 0; size -= 2) {
        uint16_t hw;
        if (getbits(1)) {
            hw = getbits(16);
        }
        else {
            uint8_t val = getbits(6);
            hw = common[val];
            common_counts[val]++;
        }
        putc(hw >> 8, output);
        putc(hw,      output);
		dsize+=2;
    }

#ifdef DEBUG__OFF
    puts("Most used 16-bit chunks:");
    for (uint32_t i = 0; i < sizeof(common)/sizeof(common[0]); i++) {
        printf("%04" PRIX16 "\t%" PRIu32 "\n", common[i], common_counts[i]);
    }
#endif
	return dsize;
}

// Higher-level functionality of the program

enum models {
    CAS = ('0' << 8) + 'C',
    LABCRADLE = ('0' << 8) + 'D',
    NONCAS = ('0' << 8) + 'E',
    CX_CAS = ('0' << 8) + 'F',
    CX_NONCAS = ('1' << 8) + '0',
    CM_CAS = ('1' << 8) + '1',
    CM_NONCAS = ('1' << 8) + '2'
} ;

void doexit(void) {
    if (image != nullptr) {
        fclose(image);
    }
    if (output != nullptr) {
        fclose(output);
    }
    free(output_filename);
    free(osversion);
    free(imgtype);
}

int main(int argc, char * argv[]) {
	int32_t dsize=0;
	uint32_t tsize;
    if (atexit(doexit)) {
        return 1;
    }

    if (true) {
        int version=1; // 0=unsigned CAS+ 1=signed
        uint32_t flags = 0;
        uint16_t calcmodel = 0;
        uint8_t * minosversion = nullptr;
        int found_8020 = 0;
        if (argc < 3) {
            puts("TI-Nspire boot2/diags decompressor for EVT conversion\n"
                 "Usage: nsbar_evt2pvt input_file output_file\n"
                 "Note: if input file is \"-\", stdin is used instead; if output file is \"-\", stdout is used instead");
            exit(1);
        }

        if (strcmp(argv[1], "-")) {
            image = fopen(argv[1], "rb");
            if (!image) {
                perror(argv[1]);
                exit(1);
            }
        }
        else {
            image = stdin;
        }

        if (strcmp(argv[2], "-")) {
            size_t len = strlen(argv[2]) + 1;
            output_filename = (char *)malloc_or_exit(len);
            strcpy(output_filename, argv[2]);

            output = fopen(output_filename, "wb");
            if (!output) {
                perror(output_filename);
                exit(1);
            }
        }
        else {
            output = stdout;
        }

        printf("Processing file \"%s\"\n", argv[1]);
        printf("Outputting to file \"%s\"\n", output_filename);

		int reachedeod=0;						// BOOT2403
            while (1) {
                long offset = ftell(image);
                uint16_t field = get16bits();
				if(reachedeod)					// BOOT2403	(go on if EOD is directly followed by a new 0x8000)
				{								// BOOT2403
					if((field&0xFFF0)!=0x8000)	// BOOT2403
						break;					// BOOT2403
					else						// BOOT2403
					{							// BOOT2403
						reachedeod=0;			// BOOT2403
						found_8020=0;			// BOOT2403
						getbits(0);				// BOOT2403	(reset)
					}							// BOOT2403
				}								// BOOT2403
                if(field==0x8084) {
                    put16bits(0x8088);
                // evt: need 8000 size to be 0
                } else if((field&0xFFF0)==0x8000) {
                    put16bits(0x8000);
                    get32bits();
                } else {
                    put16bits(field);
                }
                uint32_t size = field & 0x000F;
                long offset2;
                uint32_t addr;
                field &= 0xFFF0;
                if (size == 0x0D) {
                    size = get8bits();
                    put8bits(size);
                }
                else if (size == 0x0E) {
                    size = get16bits();
                    put16bits(size);
                }
                else if (size == 0x0F) {
                    size = get32bits();
                    if( field == 0x8070 && (flags & 1)) {
                            put32bits(get32bits());
                            fseek(image, -4, SEEK_CUR);
                    }
                    else {
                        put32bits(size);
                    }
                }

                offset2 = ftell(image);

#ifdef DEBUG
                printf("Processing field %04" PRIX32 ": size 0x%08" PRIX32 " (%" PRIu32 "), offset 0x%lX", field, size, size, offset);
                if (size > 0) {
                    printf(", contents 0x%lX-0x%lX", offset2, offset2 + size - 1);
                }
                puts("");

                if (field != 0x8000) {
                    for (uint32_t i = 0; i < 16; i++) {
                        if (i < size) {
                            printf("%02X ", get8bits());
                        }
                        else {
                            printf("   ");
                        }
                    }
                    if (size > 16) {
                        printf("...");
                    }
                    else {
                        printf("   ");
                    }
                    printf("\t\"");

                    fseek(image, offset2, SEEK_SET);
                    for (uint32_t i = 0; i < ((size < 16) ? size : 16); i++) {
                        uint8_t c = get8bits();
                        if (!isprint(c)) {
                            c = ' ';
                        }
                        printf("%c", c);
                    }
                    puts("\"");

                    fseek(image, offset2, SEEK_SET);
                }
#endif
				
                if (field == 0x8000) {
                    // Don't skip - 8070 is inside this
					// EVT - the old boot2 needs the size of field 8000 to be 0, even though that's "wrong"

					tsize=0;
#ifdef DEBUG
                    printf("\tRoot field\n");
#endif
                }
                else if (field == 0x8040) {
                    if(size==0x8) {
                        version = 0;
                    }
                    imgtype = getstr(size);
                    printf("\tImage type: \"%s\"\n", imgtype);
                }
                else if (field == 0x8010) {
					// Skip leading byte from the 8010 field
					if(size > 2)
						get8bits();
                    if (size != 3) {
						puts("ERROR: won't compress it, compressed boot2 don't work on EVT");
						fclose(output);
						unlink(output_filename);
						exit(1);
                    }
                    calcmodel = get16bits();
                    printf("\tCalculator model = %c%c ", calcmodel >> 8, calcmodel);
                    if (calcmodel == CAS) {
                        puts("(Nspire CAS+/CAS)");
                    }
                    else if (calcmodel == NONCAS) {
                        puts("(Nspire)");
                    }
                    else if (calcmodel == CX_CAS) {
                        puts("(Nspire CX CAS)");
                    }
                    else if (calcmodel == CX_NONCAS) {
                        puts("(Nspire CX)");
                    }
                    else if (calcmodel == CM_CAS) {
                        puts("(Nspire CM CAS)");
                    }
                    else if (calcmodel == CM_NONCAS) {
                        puts("(Nspire CM)");
                    }
                    else if (calcmodel == LABCRADLE) {
                        puts("(Nspire Lab Cradle)");
                    }
                    else if (calcmodel != ('F' << 8) + 'F') {
                        puts("(unknown)");
                    }
					
                }
                else if (field == 0x8020) {
                    if (!found_8020) {
                        // Boot2 version
                        osversion = getstr(size);
                        printf("\tBoot2/Diags version: %s\n", osversion);
                        found_8020++;
                    }
                    else if (found_8020 == 1) {
                        // Minimum OS version
                        minosversion = getstr(size);
                        printf("\tMinimum Boot2/Diags version: %s\n", minosversion);
                        free(minosversion);
                        found_8020++;
                    }
                    else {
                        puts("Expected only one 8020 fields in file, aborting");
                        exit(1);
                    }
                }
                else if (field == 0x8080) {
                    if (size != 4 && size != 8) {
                        printf("Field 8080 has unexpected size 0x%" PRIX32 " (%" PRIu32 ")\n", size, size);
                        exit(1);
                    }
                    addr = get32bits();
                    if (size == 8) {
                        flags = get32bits();
                    }
                    if( (flags&1) && version==0) {
                        fseek(output,-2,SEEK_CUR);
                        put16bits(0x8084);
                    }
                    put32bits(addr);
                    if(version==1) {
                        if(flags&1) {
                            put32bits(0);
                        } else {
                            put32bits(1);
                        }
                    } else {
                        if(!(flags&1)) {
                            put32bits(0x380759);
                        }
                    }
                    printf("\tBase address = 0x%08" PRIX32 ", flags = %" PRIu32 "\n", addr, flags);
                }
                else if (field == 0x80E0) {
                    uint8_t type;
                    const char * message;
                    if (size != 1) {
                        if (size != 2) {
                            printf("Field 80E0 has unexpected size (%" PRIu32 ")\n", size);
                        exit(1);
                        }
                        type = get8bits();
                    }
                    type = get8bits();
                    if (type == 0x00) {
                        message = "Clickpad/Touchpad/CX non-CAS, or Lab Cradle,"; 
                    }
                    else if (type == 0x01) {
                        message = "Clickpad/Touchpad/CX CAS";
                    }
                    else if (type == 0x02) {
                        message = "CM non-CAS";
                    }
                    else if (type == 0x03) {
                        message = "CM CAS";
                    }
                    else {
                        message = "(unknown, WARNING)";
                    }
                    printf("\tOS upgrade aimed at %s flavor\n", message);
                }
                else if (field == 0x8070) {
                    if (flags&1) {
                        puts("\tDecompressing field 8070");
                        dsize+=decompress()-size;
                        if(ftell(image)==offset2+size+1) {
                            fseek(image, offset2+size, SEEK_SET);
                        }
						// evt end after 8070
						puts("\tStopping here and adding 2 empty end fields for EVT");
						put16bits(0x02F0);put16bits(0xFFF0);
						break;
                    } else {
                        puts("ERROR: won't compress it, compressed boot2 don't work on EVT");
						fclose(output);
						unlink(output_filename);
						exit(1);
                    }
                }
                else if (field == 0xFFF0) {
                    puts("\tEnd of Data");
                    reachedeod=1;				// BOOT2403
                }
                else {
                    // Skip this field
do_skip:
#ifdef DEBUG
                    puts("\tSkipping");
#endif
                    fseek(image, size, SEEK_CUR);
                }
                if (field != 0x8000 && field != 0x8070 && field!= 0x8000 && field != 0x8080) {
                    fseek(image, offset2, SEEK_SET);
                    for (uint32_t i = 0; i < size; i++) {
                            putc(getc(image), output);
                    }
                }
            }
    }

    puts("\tDone");
    exit(0);
}
