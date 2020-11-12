#include <stdlib.h>
#include <stdio.h>
#include "converter.h"
#include "qdbmp.h"

void BMP_free(FILE_BMP* bmp){
    if (bmp == NULL){
        return;
    }
    if (bmp->palette != NULL){
        free(bmp->palette);
    }
    if (bmp->data != NULL){
        free(bmp->data);
    }
    free(bmp);
}

FILE_BMP* Reading_BMP (const char* filename){
    FILE_BMP* bmp;
    FILE* bmpFile;

    if (filename == NULL) {
        error("Incorrect arguments.\n");
        return NULL;
    }

    bmp = (FILE_BMP*)calloc(1, sizeof(FILE_BMP));
    if(bmp == NULL){
        error("Cannot allocate memory.\n");
        return  NULL;
    }

    bmpFile = fopen(filename, "rb");
    if (bmpFile == NULL){
        error("Cannot find file\n.");
        free(bmp);
        return  NULL;
    }

    fread(&bmp->header.identifier, 1, 2, bmpFile);
    if (&bmp->header.identifier != 0x4d42) {
        error("Type of input file is not supported.\n");
        return NULL;
    }

    fread(&bmp->header.size_bmp_bytes,sizeof(bmp->header.size_bmp_bytes),1, bmpFile);

    fread(&bmp->header.reserved_1, sizeof(bmp->header.reserved_1),1,bmpFile);
    if (bmp->header.reserved_1 != 0){
        error("Broken file. Error in reserve of file.\n");
        return NULL;
    }

    fread(&bmp->header.reserved_2, sizeof(bmp->header.reserved_2),1,bmpFile);
    if (bmp->header.reserved_2 != 0){
        error("Broken file. Error in reserve of file.\n");
        return NULL;
    }

    fread(&bmp->header.offset, sizeof(bmp->header.offset),1,bmpFile);

    fread(&bmp->header.size_header,sizeof(bmp->header.size_header),1,bmpFile);
    if (bmp->header.size_header != 40) {
        error("Broken file. Error in a size of the header of file.\n");
        return NULL;
    }

    fread(&bmp->header.width,sizeof(bmp->header.width),1,bmpFile);
    fread(&bmp->header.height,sizeof(bmp->header.height),1,bmpFile);

    fread(&bmp->header.number_planes,sizeof(bmp->header.number_planes),1,bmpFile);
    if (bmp->header.number_planes != 1) {
        error("Broken file. Error in the number of color planes (it must be 1).\n");
        return NULL;
    }

    fread(&bmp->header.bits_per_pixel,sizeof(bmp->header.bits_per_pixel),1,bmpFile);
    if (bmp->header.bits_per_pixel != 8 && bmp->header.bits_per_pixel != 24) {
        error("The program support only for 8bpp and 24bpp BMP files.\n");
        return NULL;
    }

    fread(&bmp->header.compression,sizeof(bmp->header.compression),1,bmpFile);
    if (bmp->header.compression != 0){
        error("The program support only BMP files without any compression.\n");
        return NULL;
    }

    fread(&bmp->header.image_size,sizeof(bmp->header.image_size),1,bmpFile);
    if (bmp->header.width * bmp->header.height * bmp->header.bits_per_pixel / 8 != bmp->header.image_size){
        error("Broken file. Incorrect size of file. Real size not match with written.\n");
        return NULL;
    }

    fread(&bmp->header.horizontal_resolution,sizeof(bmp->header.horizontal_resolution),1,bmpFile);
    fread(&bmp->header.vertical_resolution,sizeof(bmp->header.vertical_resolution),1,bmpFile);
    fread(&bmp->header.number_colors,sizeof(bmp->header.number_colors),1, bmpFile );
    fread(&bmp->header.number_important_colors,sizeof(bmp->header.number_important_colors),1, bmpFile );

    if(bmp->header.bits_per_pixel == 8){
        bmp->palette = (unsigned  char*)malloc(BMP_PALETTE_SIZE_8bpp * sizeof(unsigned  char));
        if (bmp->palette == NULL) {
            error("Cannot allocate memory.\n");
            fclose(bmpFile);
            free(bmp);
            return NULL;
        }
        if (fread(bmp->palette, sizeof(unsigned char ), BMP_PALETTE_SIZE_8bpp, bmpFile) != BMP_PALETTE_SIZE_8bpp){
            error("Incorrect file.\n");
            fclose(bmpFile);
            free(bmp->palette);
            free(bmp);
            return NULL;
        }
    }
    else {
        bmp->palette = NULL;
    }

    bmp->data = (unsigned char*)malloc(bmp->header.image_size);
    if (bmp->data == NULL){
        error("Cannot allocate memory.\n");
        fclose(bmpFile);
        free(bmp->palette);
        free(bmp);
        return  NULL;
    }

    if (fread(bmp->data,sizeof(unsigned char),bmp->header.image_size,bmpFile) != bmp->header.image_size){
        error("Incorrect file.\n");
        fclose(bmpFile);
        free(bmp->data);
        free(bmp->palette);
        free(bmp);
        return  NULL;
    }
    fclose(bmpFile);
    return bmp;
}

int Writing_BMP(FILE_BMP* bmp, const char* filename){
    FILE* bmpFile;

    if ( filename == NULL )
    {
        error("Incorrect file.\n");
        return -2;
    }

    bmpFile = fopen(filename, "wb");
    
    if (fwrite( &bmp->header, sizeof(unsigned short), 27, bmpFile ) != 27 ) {
        error("Fail in creating new image.\n");
        fclose(bmpFile);
        return -2;
    }

    if (bmp->header.bits_per_pixel == 8){
        if ( fwrite( bmp->palette, sizeof(unsigned char), BMP_PALETTE_SIZE_8bpp,bmpFile) != BMP_PALETTE_SIZE_8bpp){
            error("Fail in creating new image.\n");
            fclose(bmpFile);
            return -2;
        }
    }

    if (fwrite( bmp->data, sizeof(unsigned char), bmp->header.image_size,bmpFile) != bmp->header.image_size){
        error("Fail in creating new image.\n");
        fclose(bmpFile);
        return -2;
    }
    fclose(bmpFile);
    return 0;
}

int checking_errors(const char* filename){
    if (Reading_BMP(filename) == NULL){
        return -2;
    }
    return 0;
}

void mine_converter_negative(const char* inputFile, const char* outputFile) {
    FILE_BMP *bmp;
    bmp = Reading_BMP(inputFile);
    if (bmp->header.bits_per_pixel == 24) {
        for (int i = 0; i < bmp->header.image_size; i++) {
            bmp->data[i] = ~bmp->data[i];
        }
    } else if (bmp->header.bits_per_pixel == 8) {
        for (int i = 0; i < BMP_PALETTE_SIZE_8bpp; i++) {
            if ((i + 1) % 4 != 0) {
                bmp->palette[i] = ~bmp->palette[i];
            }
        }
    }

    Writing_BMP(bmp, outputFile);
    BMP_free(bmp);
}