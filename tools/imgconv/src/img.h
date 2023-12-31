#ifndef IMG_H
#define IMG_H

typedef struct {
    unsigned int width, height;
    int bit_depth;
    int color_type;
    unsigned char *data;
    unsigned char **rows;
    long row_bytes;
} GFXImage;

enum {
    OUT_SUCCESS,
    OUT_ERROR_NOT_PNG,
    OUT_ERROR_FILE,
    OUT_ERROR_OUT_OF_MEM,
    OUT_ERROR_UNKNOWN
};

int img_load(char *file, GFXImage *image);
void img_free(GFXImage *image);

#endif
