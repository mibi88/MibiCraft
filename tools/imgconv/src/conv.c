#include <conv.h>

#include <img.h>

#include <stdlib.h>
#include <stdio.h>

#include <string.h>

#include <ctype.h>

char *get_name(char *file) {
    int size;
    char *new;
    char *end = file+strlen(file);
    char *start;
    int end_found = 0;

    for(start=end;start>=file;start--){
        if(*start == '.' && !end_found){
            end = start;
            end_found = 1;
        }
        if(*start == '/' || *start == '\\'){
            start++;
            break;
        }
    }

    size = end-start;
    new = malloc((size+1)*sizeof(char));
    if(new == NULL){
        return NULL;
    }
    memcpy(new, start, size*sizeof(char));
    new[size] = '\0';

    return new;
}

void uppercase_fputs(char *str, FILE *fp) {
    int i;

    for(i=0;i<(int)strlen(str);i++){
        fputc(toupper(str[i]), fp);
    }
}

int conv(char *input, char *c_header, char *c_source) {
    long i;
    int c, len;
    char buffer[10];
    char *name;

    GFXImage image;

    FILE *fp_c_header;
    FILE *fp_c_source;

    name = get_name(input);
    if(name == NULL){
        puts("Out of memory!");
        return EXIT_FAILURE;
    }

    if(img_load(input, &image)){
        puts("Error!");
        free(name);
        return EXIT_FAILURE;
    }else{
        puts("Success!");
    }

    fp_c_header = fopen(c_header, "w");
    if(!fp_c_header){
        puts("Could not open file!");
        free(name);
        img_free(&image);
        return EXIT_FAILURE;
    }
    fp_c_source = fopen(c_source, "w");
    if(!fp_c_source){
        puts("Could not open file!");
        fclose(fp_c_header);
        free(name);
        img_free(&image);
        return EXIT_FAILURE;
    }

    /* Generate C source file */
    fprintf(fp_c_source, "#include \"%s\"\n\n"
            "/* Image %s converted with tools/imgconv */\n\n"
            "const unsigned char %s_data[%ld] = {\n    ", c_header, name, name,
            image.row_bytes*image.height);
    c = 4;

    for(i=0;i<image.row_bytes*image.height;i++){
        if(i != image.row_bytes*image.height-1){
            len = sprintf(buffer, "0x%02x, ", image.data[i]);
        }else{
            len = sprintf(buffer, "0x%02x", image.data[i]);
        }
        if(len > 0){
            if(c+len > 80){
                fputs("\n    ", fp_c_source);
                c = 4;
            }
            fputs(buffer, fp_c_source);
            c += len;
        }
    }

    fprintf(fp_c_source, "\n};\n\n"
            "const int %s_width = %d;\n"
            "const int %s_height = %d;\n\n", name, image.width, name,
            image.height);

    /* Generate the C header */
    fputs("#ifndef ", fp_c_header);
    uppercase_fputs(name, fp_c_header);
    fputs("_H\n", fp_c_header);

    fputs("#define ", fp_c_header);
    uppercase_fputs(name, fp_c_header);
    fputs("_H\n\n", fp_c_header);

    fprintf(fp_c_header, "extern const unsigned char %s_data[%ld];\n\n"
            "extern const int %s_width;\n"
            "extern const int %s_height;\n\n"
            "#endif\n", name, image.row_bytes*image.height, name, name);

    fclose(fp_c_source);
    fclose(fp_c_header);

    free(name);
    img_free(&image);

    return EXIT_SUCCESS;
}
