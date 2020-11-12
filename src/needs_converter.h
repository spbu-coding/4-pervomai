
#ifndef UNTITLED2_CONVERTER_H
#define UNTITLED2_CONVERTER_H

#define BMP_PALETTE_SIZE_8bpp ( 256 * 4 )
#define error(...) (fprintf(stderr, __VA_ARGS__))

struct BMP_File_Header
{
    unsigned short		identifier;
    unsigned long		size_bmp_bytes;
    unsigned short		reserved_1;
    unsigned short		reserved_2;
    unsigned long		offset;
    unsigned long		size_header;
    long	        	width;
    long        		height;
    unsigned short		number_planes;
    unsigned short		bits_per_pixel;
    unsigned long		compression;
    unsigned long		image_size;
    long	        	horizontal_resolution;
    long	        	vertical_resolution;
    unsigned long		number_colors;
    unsigned long		number_important_colors;
};

struct file_bmp
{
    struct BMP_File_Header	header;
    unsigned char*		palette;
    unsigned char*		data;
};

typedef struct file_bmp FILE_BMP;

void BMP_free(FILE_BMP* bmp);
FILE_BMP* Reading_BMP (const char* filename);
int Writing_BMP(FILE_BMP* bmp, const char* filename);
void Getting_Pix_RGB(FILE_BMP* bmp, unsigned long x, unsigned long y, unsigned char* r,unsigned char* g,unsigned char* b);
void mine_converter_negative(const char* inputFile, const char* outputFile);
int checking_errors(const char* filename);

#endif //UNTITLED2_CONVERTER_H
