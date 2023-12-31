#include <stdlib.h>
#include <stdio.h>

#include <conv.h>

int main(int argc, char **argv) {
    if(argc < 4){
        fputs("Need more args!\n", stderr);
        return EXIT_FAILURE;
    }

    return conv(argv[1], argv[2], argv[3]);
}
