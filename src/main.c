#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <getopt.h>
#include <string.h>
#include <jpeglib.h>
#include <jerror.h>
#include <sys/stat.h>
#include <sys/types.h>

#define SW_V "sv0a"

#define STR_BUF_SIZE    64
#define RGB565_FROM_RGB(r, g, b)    (((uint16_t)r & 0xF8) << 8) | (((uint16_t)g & 0xFC) << 3) | ((uint16_t)b >> 3)

void str_upper(char *str);
static void print_help(const char *progname);

int main(int argc, char **argv)
{
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

    char pbInputFile[STR_BUF_SIZE];
    memset(pbInputFile, 0x00, STR_BUF_SIZE);

    char pbOutputDirectory[STR_BUF_SIZE];
    memset(pbOutputDirectory, 0x00, STR_BUF_SIZE);

    char pbOutputName[STR_BUF_SIZE];
    memset(pbOutputName, 0x00, STR_BUF_SIZE);

    char pbOutVarName[STR_BUF_SIZE];
    memset(pbOutVarName, 0x00, STR_BUF_SIZE);

    char pbOutNameMacro[STR_BUF_SIZE];
    memset(pbOutNameMacro, 0x00, STR_BUF_SIZE);

    char pbOutHeaderName[STR_BUF_SIZE];
    memset(pbOutHeaderName, 0x00, STR_BUF_SIZE);

    char pbOutSourceName[STR_BUF_SIZE];
    memset(pbOutSourceName, 0x00, STR_BUF_SIZE);

    char pbOutHeaderNameDirectory[STR_BUF_SIZE];
    memset(pbOutHeaderNameDirectory, 0x00, STR_BUF_SIZE);

    char pbOutSourceNameDirectory[STR_BUF_SIZE];
    memset(pbOutSourceNameDirectory, 0x00, STR_BUF_SIZE);

    int c;
    int digit_optind = 0;

    while(1)
    {
        int this_option_optind = optind ? optind : 1;
        int option_index = 0;
        static struct option long_options[] = {
            {"help",        no_argument,        0, 'h'},
            {"output",      required_argument,  0, 'o'},
            {"directory",   required_argument,  0, 'd'},
            {"version",     no_argument,        0, 'v'},
            {0,             0,                  0,  0 }
        };

        c = getopt_long(argc, argv, "hvo:d:", long_options, &option_index);
        if (c == -1)
            break;

        switch(c)
        {
        case 'h':
                print_help(argv[0]);
                return 0;

        case 'v':
                fprintf(stderr, "Version: %s\n", SW_V);
                return 0;

        case 'o':
                strncpy(pbOutputName, optarg, STR_BUF_SIZE);
                break;

        case 'd':
                strncpy(pbOutputDirectory, optarg, STR_BUF_SIZE);
                break;

        case '?':
                fprintf(stderr, "invalid argument!\n");
                fprintf(stderr, "Try `%s --help' for more information.\n", argv[0]);
                return -1;
                break;

        default:
                fprintf(stderr, "option parsing error!\n");
                return -1;
        }
    }

        fprintf(stderr, "pbOutputDirectory: %s\n", pbOutputDirectory);

    if((argc - optind) == 1)
    {
        strncpy(pbInputFile, argv[optind++], STR_BUF_SIZE);
    }
    else
    {
        fprintf(stderr, "invalid arguments.\n");
        fprintf(stderr, "Try `%s --help' for more information.\n", argv[0]);
        return -1;
    }

    if(!(pbOutputName[0]))
    {
        fprintf(stderr, "invalid arguments.\n");
        fprintf(stderr, "Try `%s --help' for more information.\n", argv[0]);
        return -1;
    }

    snprintf(pbOutVarName, STR_BUF_SIZE, "us%s", pbOutputName);
    *(pbOutVarName + 2) = toupper(*(pbOutVarName + 2));

    strncpy(pbOutNameMacro, pbOutputName, STR_BUF_SIZE);
    str_upper(pbOutNameMacro);

    snprintf(pbOutHeaderName, STR_BUF_SIZE, "%s.h", pbOutputName);

    snprintf(pbOutSourceName, STR_BUF_SIZE, "%s.c", pbOutputName);

    if(pbOutputDirectory[0])
    {
        mkdir(pbOutputDirectory, 0777);

        snprintf(pbOutHeaderNameDirectory, STR_BUF_SIZE, "%s/%s", pbOutputDirectory, pbOutHeaderName);
        snprintf(pbOutSourceNameDirectory, STR_BUF_SIZE, "%s/%s", pbOutputDirectory, pbOutSourceName);
    }
    else
    {
        strncpy(pbOutHeaderNameDirectory, pbOutHeaderName, STR_BUF_SIZE);
        strncpy(pbOutSourceNameDirectory, pbOutSourceName, STR_BUF_SIZE);
    }

    fprintf(stderr, "pbInputFile: %s\n", pbInputFile);
    fprintf(stderr, "pbOutputDirectory: %s\n", pbOutputDirectory);
    fprintf(stderr, "pbOutputName: %s\n", pbOutputName);
    fprintf(stderr, "pbOutVarName: %s\n", pbOutVarName);
    fprintf(stderr, "pbOutNameMacro: %s\n", pbOutNameMacro);
    fprintf(stderr, "pbOutHeaderName: %s\n", pbOutHeaderName);
    fprintf(stderr, "pbOutSourceName: %s\n", pbOutSourceName);
    fprintf(stderr, "pbOutHeaderNameDirectory: %s\n", pbOutHeaderNameDirectory);
    fprintf(stderr, "pbOutSourceNameDirectory: %s\n", pbOutSourceNameDirectory);

    FILE *fIn = fopen(pbInputFile, "rb");
    if(fIn == NULL)
    {
        fprintf(stderr, "Could not open input file.\n");
        return -1;
    }

    struct jpeg_decompress_struct info; //for our jpeg info
    struct jpeg_error_mgr err; //the error handler

    info.err = jpeg_std_error(&err);
    jpeg_create_decompress(&info); //fills info structure

    jpeg_stdio_src(&info, fIn);
    jpeg_read_header(&info, true);

    jpeg_start_decompress(&info);

    int w = info.output_width;
    int h = info.output_height;
    int numChannels = info.num_components; // 3 = RGB, 4 = RGBA
    unsigned long dataSize = w * h * numChannels;

    // read RGB(A) scanlines one at a time into jdata[]
    unsigned char *data = (unsigned char *)malloc(dataSize);
    unsigned char* rowptr;
    while(info.output_scanline < h)
    {
        rowptr = data + info.output_scanline * w * numChannels;
        jpeg_read_scanlines(&info, &rowptr, 1);
    }

    jpeg_finish_decompress(&info);

    fclose(fIn);

    FILE *fOutH = fopen(pbOutHeaderNameDirectory, "w+");

    fprintf(fOutH, "/*\n");
    fprintf(fOutH, " * auto-generated file by jpeg-to-rgb565 %s\n", SW_V);
    fprintf(fOutH, " * for more information check:\n");
    fprintf(fOutH, " * https://github.com/gimbas/jpeg-to-rgb565\n");
    fprintf(fOutH, " *\n");
    fprintf(fOutH, " * format:\n");
    fprintf(fOutH, " * %s[0] = image width\n", pbOutVarName);
    fprintf(fOutH, " * %s[1] = image heigth\n", pbOutVarName);
    fprintf(fOutH, " * %s[2:last] = 16bit rgb565 pixel color from top left to bottom right\n", pbOutVarName);
    fprintf(fOutH, " *\n");
    fprintf(fOutH, " * resolution: %dw x %dh\n", w, h);
    fprintf(fOutH, " * n symbols(16bit): %d\n", w * h);
    fprintf(fOutH, " * size(bits): %dKbit", (w * h * 16) / 1000);
    fprintf(fOutH, " * size(bytes): %dKiB\n", (w * h * 2) / 1024);
    fprintf(fOutH, " */\n\n");

    fprintf(fOutH, "#ifndef\t__%s_H_\n", pbOutNameMacro);
    fprintf(fOutH, "#define\t__%s_H_\n\n", pbOutNameMacro);
    fprintf(fOutH, "#include \"rgb565.h\"\n\n");

    fprintf(fOutH, "extern const rgb565_t %s[];\n\n", pbOutVarName);

    fprintf(fOutH, "#endif\t// __%s_H_", pbOutNameMacro);

    fclose(fOutH);

    FILE *fOutC = fopen(pbOutSourceNameDirectory , "w+");

    fprintf(fOutC, "#include \"%s\"\n\n", pbOutHeaderName);
    fprintf(fOutC, "const rgb565_t %s[] = {\n0x%04X, 0x%04X,\n", pbOutVarName, w, h);
    for(uint32_t i = 0; i < w * h; i++)
    {
        fprintf(fOutC, "0x%04X", RGB565_FROM_RGB(data[i * numChannels], data[(i * 3) + 1], data[(i * 3) + 2]));
        if(i != ((w * h) - 1))
        {
            if(i != 0)
            {
                if(!((i + 1) % 8))
                    fprintf(fOutC, ",\n");
                else
                    fprintf(fOutC, ", ");
            }
            else
                fprintf(fOutC, ", ");
        }
    }
    fprintf(fOutC, "\n};\n\n");

    fclose(fOutC);

    free(data);
}

void str_upper(char *str)
{
    while(*str)
    {
        *str = toupper((unsigned char) *str);
        str++;
    }
}

static void print_help(const char *progname)
{
	fprintf(stderr, "Simple jpeg to \"array\" converter tool for use in mcu projects.\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Version: %s\n", SW_V);
    fprintf(stderr, "\n");
	fprintf(stderr, "Usage: %s <input file> -o <output name>\n", progname);
	fprintf(stderr, "\n");
	fprintf(stderr, "General options:\n");
	fprintf(stderr, "  --output -o <output name>    required option, use this string for output array and file names\n");
    fprintf(stderr, "                               do not include directory or file extension\n");
    fprintf(stderr, "  --directory -d <directory>   optional output directory\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "Mode of operation:\n");
	fprintf(stderr, "  [default]             write jpeg contents (16bit rgb565) to array in file\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "Miscellaneous options:\n");
	fprintf(stderr, "      --help            display this help and exit\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "Exit status:\n");
	fprintf(stderr, "  0 on success,\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "If you have a bug report, please file an issue on github:\n");
	fprintf(stderr, "  https://github.com/gimbas/\n");
}