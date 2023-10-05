#include <stdio.h>
#include <ctype.h>

char *words[] = {
    "alfa",
    "bravo",
    "charlie",
    "delta",
    "echo",
    "foxtrot",
    "golf",
    "hotel",
    "india",
    "juliet",
    "kilo",
    "lima",
    "mike",
    "november",
    "oscar",
    "papa",
    "quebec",
    "romeo",
    "sierra",
    "tango",
    "uniform",
    "victor",
    "whiskey",
    "x-ray",
    "yankee",
    "zulu"
};

int main(int argc, char *argv[]) {

    if(argc != 2) {
        fprintf(stderr, "Usage: %s <letter>\n", argv[0]);
        return -1;
    }
    printf("%c is for %s.\n", argv[1][0], words[tolower(argv[1][0]) - 'a']);

    return 0;
}