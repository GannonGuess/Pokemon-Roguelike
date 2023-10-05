#include <stdio.h>
#include <stdarg.h>

/* Converts parameters of 4 types: int, float, char, strings
 * Conversion specifiers are d, f, c, s
 */
void printf327(const char *format, ...) {
    va_list ap;
    int i;
    float f;
    char c;
    char *s;

    va_start(ap, format);

    while(*format) {
        switch(*format) {
        case 'd':
            i = va_arg(ap, int);
            printf("%d\n", i);
            break;
        case 'f':
            f = va_arg(ap, double);
            printf("%f\n", f);
            break;
        case 'c':
            c = va_arg(ap, int);
            printf("%c\n", c);
            break;
        case 's':
            s = va_arg(ap, char *);
            printf("%s\n", s);
            break;
        }
        format++;
    }
    va_end(ap);
}

int main(int argc, char *argv[]) {
    printf327("dfcs", 0, 1.414f, 'g', "Hello World!");
    return 0;
}