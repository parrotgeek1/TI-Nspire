// C++11 code compilable with a C++ compiler supporting lambda functions, nullptr and raw string literals, e.g. GCC 4.6+ or Clang 3.1+.

#include <cinttypes>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>
#ifdef _OPENMP
#include <omp.h>
#endif
#define nullptr 0

using namespace std;

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

static uint16_t get16bits(void) {
    uint8_t hi = get8bits();
    return ((uint16_t)hi << 8) | get8bits();
}

static uint32_t get32bits(void) {
    uint16_t hi = get16bits();
    return ((uint32_t)hi << 16) | get16bits();
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

enum models {
	CAS = ('0' << 8) + 'C',
	LABCRADLE = ('0' << 8) + 'D',
	NONCAS = ('0' << 8) + 'E',
	CX_CAS = ('0' << 8) + 'F',
	CX_NONCAS = ('1' << 8) + '0',
	CM_CAS = ('1' << 8) + '1',
	CM_NONCAS = ('1' << 8) + '2'
} ;

// Decompression for boot2

static int getbits(uint32_t n) {
    static uint32_t buf = 0, bits = 0;
    // Final step.
    if (!n) {
        bits = 0;
    }
    while (bits < n) {
        buf = buf << 8 | get8bits();
        bits += 8;
    }
    bits -= n;
    return buf >> bits & ((1 << n) - 1);
}

static void decompress(void) {
    uint32_t decompressed_size = get32bits();
    uint16_t common[64];
    uint32_t common_counts[64];

    for (uint32_t i = 0; i < sizeof(common)/sizeof(common[0]); i++) {
        common[i] = get16bits();
        common_counts[i] = 0;
    }

    for (; decompressed_size > 0; decompressed_size -= 2) {
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
    }

    /*puts("Most used 16-bit chunks:");
    for (uint32_t i = 0; i < sizeof(common)/sizeof(common[0]); i++) {
        printf("%04" PRIX16 "\t%" PRIu32 "\n", common[i], common_counts[i]);
    }*/
}

void doexit(void) {
    if (image != nullptr) {
        fclose(image);
    }
    if (output != nullptr) {
        fclose(output);
    }
    free(osversion); osversion = nullptr;
    free(imgtype); imgtype = nullptr;
}

enum operation_modes {
    MODE_NONE = 0,
    MODE_LISTFIELDS,
    MODE_DECOMPRESS,
    MODE_DECRYPT,
    MODE_COMPRESS,
    MODE_EXTRACTFS
} ;

int main(int argc, char * argv[]) {
    if (atexit(doexit)) {
        return 1;
    }

        uint32_t flags = 0;
        long field_8070_offset = 0;
        long field_8200_offset = 0;
        uint32_t field_8070_size = 0;
        uint32_t field_8200_size = 0;
        int has_1024_bit_signature = 0;
        //int has_2048_bit_signature = 0;
        uint16_t calcmodel = 0;
        uint8_t * minosversion = nullptr;
        int found_8020 = 0;
        int has_boot15 = 0;
        uint32_t chunks = 1;
        operation_modes mode = MODE_DECOMPRESS;

        if (argc != 3) {
usage:
            puts("usage: nsbar_evt1pvt boot2.img [output_file]\nNote: if input file is -, stdin is used instead; if output file is -, stdout is used instead.");
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

        printf("Processing file \"%s\"\n", argv[1]);

        if (   (mode == MODE_LISTFIELDS)
            || (mode == MODE_DECOMPRESS)
            || (mode == MODE_DECRYPT)
            || (mode == MODE_EXTRACTFS)
           ) {
	int casplus = 0;
            while (1) {
                long offset = ftell(image);
                uint16_t field = get16bits();
                uint32_t size = field & 0x000F;
                long offset2;
                field &= 0xFFF0;
                if (size == 0x0D) {
                    size = get8bits();
                }
                else if (size == 0x0E) {
                    size = get16bits();
                }
                else if (size == 0x0F) {
                    size = get32bits();
                }

                offset2 = ftell(image);

                if (field == 0x8000) {
                    // Don't skip - 8070 is inside this
                }

                else if (field == 0x8080) {
                    uint32_t addr;
                    if (size != 4 && size != 8) {
                        printf("Field 8080 has unexpected size 0x%" PRIX32 " (%" PRIu32 ")\n", size, size);
                        exit(1);
                    }
                    addr = get32bits();
                    if (size == 8) {
                        flags = get32bits();
                    }
                    printf("Base address = 0x%08" PRIX32 "\n", addr);
                }
                else if (field == 0x8070) {
                    // Save offset and size for later decompression / decryption
                    field_8070_offset = ftell(image);
                    field_8070_size = size;
                    fseek(image, size, SEEK_CUR);
                }
                else if ((field == 0x0220) || (field == 0x0240)) {
                    if (size != 0x80) {
                        printf("Field 0240 has unexpected size 0x%" PRIX32 " (%" PRIu32 ")\n", size, size);
                        exit(1);
                    }
                    has_1024_bit_signature = 1;
                    fseek(image, size, SEEK_CUR);
                }
                else if (field == 0x0290) {
                    if (size != 0x100) {
                        printf("Field 0290 has unexpected size 0x%" PRIX32 " (%" PRIu32 ")\n", size, size);
                        exit(1);
                    }
                    //has_2048_bit_signature = 1;
                    fseek(image, size, SEEK_CUR);
                }
                else if (field == 0xFFF0) {
                    if (has_boot15) {
                        found_8020 = 0; // Reset it.
                        if (chunks == 1) {
                            puts("\tSome expected data past the boot1.5's } of file marker. Going ahead.");
                            break; // TODO do not break.
                        }
                        else if (chunks == 2) {
                            break;
                        }
                        else {
                            puts("\tSome unexpected data past the boot1.5's } of file marker.");
                            exit(1);
                        }
                    }
                    else {
                        break;
                    }
                    chunks++;
                }
                else {
                    // Skip this field
do_skip:
                    fseek(image, size, SEEK_CUR);
                }
            }
            
            if (argc > 3) {
                if (strcmp(argv[2], "-")) {
                    output_filename = strdup(argv[2]);
                    if (!output_filename) {
                        perror(argv[2]);
                        exit(1);
                    };

                    output = fopen(output_filename, "wb");
                    if (!output) {
                        perror(output_filename);
                        exit(1);
                    }
                }
                else {
                    output = stdout;
                }
            }
            else {
				output_filename =  argv[2];

                output = fopen(output_filename, "wb");
                if (!output) {
                    perror(output_filename);
                    exit(1);
                }
            }

            printf("Outputting to file \"%s\"\n", output_filename);

            if (   (mode == MODE_DECOMPRESS)
                || (mode == MODE_DECRYPT)
               ) {
                if (flags == 0 || flags == 4) {
                    puts("Outputting field 8070 verbatim");
                    fseek(image, field_8070_offset, SEEK_SET);

                    for (uint32_t i = 0; i < field_8070_size; i++) {
                        putc(getc(image), output);
                    }
                }
                else if (flags == 1 || flags == 3671897) {
                    puts("Decompressing field 8070");
                    fseek(image, field_8070_offset, SEEK_SET);

                    decompress();
                }
                else {
                    puts("Unknown image type");
                    exit(1);
                }
            }
        }

    exit(0);
}
