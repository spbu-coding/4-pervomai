#include <stdio.h>
#include <stdlib.h>
#include "converter.h"

void Getting_Pix_RGB(FILE_BMP* bmp, unsigned long x, unsigned long y, unsigned char* r,unsigned char* g,unsigned char* b){
    unsigned char* pixel;
    unsigned long bytes_per_row;
    unsigned char bytes_per_pixel;
    if (bmp == NULL || x < 0 || x >= bmp->header.width || y < 0 || y >= bmp->header.height ){
        error("Incorrect arguments");
    }
    else{
        bytes_per_pixel = bmp->header.bits_per_pixel >> 3;
        bytes_per_row = bmp->header.image_size / bmp->header.height;
        pixel = bmp->data + (( bmp->header.height - y - 1 ) * bytes_per_row + x * bytes_per_pixel );

        if ( bmp->palette != NULL ){
            pixel = bmp->palette + *pixel * 4;
        }
        if (r) *r = *(pixel + 2);
        if (g) *g = *(pixel + 1);
        if (b) *b = *(pixel + 0);
    }
}

int compare_bmp(const char* File_1, const char* File_2){
    FILE_BMP* Bmp_1 = Reading_BMP(File_1);
    FILE_BMP* Bmp_2 = Reading_BMP(File_2);
    unsigned int counting_not_same = 0;
    long x,y;
    unsigned char r1, r2, g1,g2,b1,b2;

    if (Bmp_1->header.width != Bmp_2->header.width || abs(Bmp_1->header.height) != abs(Bmp_2->header.height)) {
        BMP_free(Bmp_1);
        BMP_free(Bmp_2);
        error("These two images could not be compared as have different width or height.\n");
        return -1;
    }
    if (Bmp_1->header.bits_per_pixel != Bmp_2->header.bits_per_pixel) {
        error("These two images could not be compared as have different BPP.\n");
        return -1;
    }


    if(Bmp_1->header.height > 0 && Bmp_2->header.height > 0){
        for (y = 0; y < Bmp_1->header.height; y++){
            for (x = 0; x < Bmp_2->header.width; x++){
                Getting_Pix_RGB(Bmp_1,x,y,&r1,&g1,&b1);
                Getting_Pix_RGB(Bmp_2,x,y,&r2,&g2,&b2);
                if(r1 != r2 || g1 != g2 || b1 != b2 && counting_not_same < 100) {
                    counting_not_same++;
                    error("%ld %ld\n", x, y);
                }
            }
        }
    }
    else if(Bmp_1->header.height < 0 && Bmp_2->header.height < 0){
        for (y = 0; y < abs(Bmp_1->header.height); y++){
            for (x = 0; x < Bmp_2->header.width; x++){
                Getting_Pix_RGB(Bmp_1,x,abs(Bmp_1->header.height) - y - 1,&r1,&g1,&b1);
                Getting_Pix_RGB(Bmp_2,x,abs(Bmp_2->header.height) - y - 1,&r2,&g2,&b2);
                if(r1 != r2 || g1 != g2 || b1 != b2 && counting_not_same < 100) {
                    counting_not_same++;
                    error("%ld %ld\n", x, y);
                }
            }
        }
    }
    else if(Bmp_1->header.height > 0 && Bmp_2->header.height < 0){
        for (y = 0; y < abs(Bmp_1->header.height); y++){
            for (x = 0; x < Bmp_2->header.width; x++){
                Getting_Pix_RGB(Bmp_1,x,y,&r1,&g1,&b1);
                Getting_Pix_RGB(Bmp_2,x,abs(Bmp_2->header.height) - y - 1,&r2,&g2,&b2);
                if(r1 != r2 || g1 != g2 || b1 != b2 && counting_not_same < 100) {
                    counting_not_same++;
                    error("%ld %ld\n", x, y);
                }
            }
        }
    }
    else if(Bmp_1->header.height < 0 && Bmp_2->header.height > 0){
        for (y = 0; y < abs(Bmp_1->header.height); y++){
            for (x = 0; x < Bmp_2->header.width; x++){
                Getting_Pix_RGB(Bmp_1,x,abs(Bmp_1->header.height) - y - 1,&r1,&g1,&b1);
                Getting_Pix_RGB(Bmp_2,x,y,&r2,&g2,&b2);
                if(r1 != r2 || g1 != g2 || b1 != b2 && counting_not_same < 100) {
                    counting_not_same++;
                    error("%ld %ld\n", x, y);
                }
            }
        }
    }
    BMP_free(Bmp_1);
    BMP_free(Bmp_2);
    if (counting_not_same){
        return 1;
    }
    else {
        return 0;
    }
}

int main(int argc, char* argv[]){
    if (argc != 3){
        error("Wrong amount of arguments! You must write names of "
              "input and output files (<input_name>.bmp, <output_name>.bmp)\n");
        return -1;
    }
    int comparing = compare_bmp(argv[1],argv[2]);
    return comparing;
}