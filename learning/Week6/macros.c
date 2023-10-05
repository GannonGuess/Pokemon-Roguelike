/* Two kinds of macros: value-type, function-type. */
#define PI 3.1415926
//math.h: M_PI
//const double PI = 3.14
// const sits in memory somewhere, macro is encoded into instructions and somewhat more efficient

#define add(x, y) (x + y)

#define min(x, y) (x < y ? x : y)

#define max(x, y) ({ \
    typeof (x) _x = x; \
    typeof (y) _y = y; \
    _x > _y ? _x : _y; \
})

#define stringify(x) #x

struct lutable_entry {
    char *name;
    void (*func)();
};

#define build_entry(x) {#x, x}

struct lutable_entry lookup_table[] = {
    // {"a",    a    },
    // {"c",    c    },
    // {"foo",  foo  },
    // {"go",   go   },
    // {"stop", stop },
    build_entry(a),
    build_entry(c),
    build_entry(foo),
    build_entry(go),
    build_entry(stop),
};

bsearch("c", lookup_table, num elements in table, size of each element, comparitor **)->func();

PI;

#deine concatenate(x) ## foo

add(PI, 10) * 2;

min(4, 7);
min(really_expensive_function(), function_with_side_effects());

stringify(Hello World!);

concatenate(Hello);

enum trainer_type {
    trainer_hiker,
    triner_rival,
    triner_swimmer,
};

#define move(rival) trainer_move_func[trainer_ ## x]()

move(rival);

#define FOO

#ifndef FOO_H
#define FOO_H

#endif
#ifdef
#if !defined(x) 