#include "converter.h"
#include "qdbmp.h"

int reading_entering_args(int argc, char* argv[]) {
    if (argc != 4){
        error("Wrong amount of arguments! You must write the type of converting(--mine or --theirs)"
              "names of input and output files (<input_name>.bmp, <output_name>.bmp)");
        return -1;
    }
    if (strcmp(argv[1], "--mine") != 0 && strcmp(argv[1], "--theirs") != 0){
        error("Please, write the methode of converting (--mine or --theirs)");
        return -1;
    }
    if (strstr(argv[2], ".bmp") != 0){
        error("Incorrect input file. File must be BPM type");
        return -3;
    }
    if (strstr(argv[3], ".bmp") != 0){
        error("Incorrect output file. File must be BPM type");
        return -3;
    }
    return 0;
}

int others_converter_negative(const char* inputFile, const char* outputFile) {
    BMP *bmp;
    unsigned char r, g, b;
    unsigned long width, height;
    unsigned long x, y;
    bmp = BMP_ReadFile(inputFile);
    BMP_CHECK_ERROR(stderr, -3);
    width = BMP_GetWidth(bmp);
    height = BMP_GetHeight(bmp);

    if (bmp->Header.BitsPerPixel == 24) {
        for (x = 0; x < width; ++x) {
            for (y = 0; y < height; ++y) {
                BMP_GetPixelRGB(bmp, x, y, &r, &g, &b);
                BMP_SetPixelRGB(bmp, x, y, ~r, ~g, ~b);
            }
        }
    } else if (bmp->Header.BitsPerPixel == 8) {
        for (unsigned i = 0; i < BMP_PALETTE_SIZE_8bpp; i++) {
            BMP_GetPaletteColor(bmp, i, &r, &g, &b);
            BMP_SetPaletteColor(bmp, i, ~r, ~g, ~b);
        }
    } else {
        BMP_Free(bmp);
        return -3;
    }

    BMP_WriteFile( bmp, outputFile);
    BMP_CHECK_ERROR(stderr, -3);
    BMP_Free(bmp);
    return 0;
}

int main(int argc, char* argv[]) {

    int review_args = reading_entering_args(argc, argv);
    if(review_args < 0){
        return review_args;
    }

    if (strcmp(argv[1],"--mine") == 0){
        if (checking_errors(argv[2]) == -2) {
            mine_converter_negative(argv[2], argv[3]);
        }
    }
    else if (strcmp(argv[1],"--theirs") == 0){
        others_converter_negative(argv[2],argv[3]);
    }

    return 0;
}
