#include <stdio.h>
#include <string.h>

// typedef extant_type new_type;

typedef enum {
    write_text,   // first enum has a value 0
    read_text,    // Each subsequent enum has value 1 + previous 
    write_binary,
    read_binary
} operation;

typedef struct {
    int i;
    float f;
}data;

/* Program takes one of four switches: -wt, -rt, -wb, -rb*/

int main(int argc, char *argv[]) {
    operation op;
    data d;
    FILE *f;
    char *s;

    d.i = 1;
    d.f = 1.414;

    if(argc != 2) {
        fprintf(stderr, "Usage: %s [-wt|-rt|-wb|rb]\n", argv[0]);
        return -1;
    }

    if(!strcmp(argv[1], "-wt")) { // if argv[1] == "-wt"
        op = write_text;
    } else if(!strcmp(argv[1], "-rt")) {
        op = read_text;
    } else if(!strcmp(argv[1], "-wb")) {
        op = write_binary;
    } else if(!strcmp(argv[1], "-rb")) {
        op = read_binary;
    } else {
        fprintf(stderr, "Usage: %s [-wt|-rt|-wb|rb]\n", argv[0]);
        return -1;
    }

    switch(op) {
        case write_text:
            sprintf(s, "%d %f\n", d.i, d.f);
            if (!(f = fopen("textfile", "w"))) {
                perror("textfile");
                return -1;
            }
            //fprintf(f, "%d %f\n", d.i, d.f);
            fwrite(s, 1, 11, f);
            fclose(f);
            break;
        case read_text:
            d.i = 0;
            d.f = 0.0;
            if (!(f = fopen("textfile", "r"))) {
                perror("textfile");
                return -1;
            }
            fscanf(f, "%d %f\n", &d.i, &d.f);
            fclose(f);
            printf("%d %f\n", d.i, d.f);
            break;
        case write_binary:
            if (!(f = fopen("binaryfile", "w"))) { // use hexdump -C to view binary data in hexabindecimal
                perror("textfile");
                return -1;
            }
            fwrite(&d, sizeof (d), 1, f);
            fclose(f);
            break;
        case read_binary:
            d.i = 0;
            d.f = 0.0;
            if (!(f = fopen("binaryfile", "r"))) {
                perror("textfile");
                return -1;
            }
            fread(&d, sizeof (d), 1, f);
            fclose(f);
            printf("%d %f\n", d.i, d.f);
            break;
    }
}