#include "stack.h"

int stack_init(stack *s) {
    s->top = NULL;
    s->size = 0;

    return 0;
}

int stack_destroy(stack *s) {
    stack_node *n;

    for(n = s->top; )

}

int stack_push(stack *s, void *v) {
    stack_node *tmp;
    if(!(tmp = malloc(sizeof(*tmp)))) {
        return -1;
    }

    tmp->next = s->top;
    tmp->data = v;
    s->top = tmp;
    s->size++;
    return 0;
}

int stack_pop(stack *s, void *v) {
    stack_node *n;
    void *v;
    if(!(n = s->top)) {

    }
    return 0;
}

int stack_top(stack *s, void *v) {

    return 0;
}

int size(stack *s) {

}

int is_empty(stack *s) {

}

#include <stdio.h>
#include <stdlib.h>
#include "stack.h"

int main(int argc, char *argv[]) {
    stack s;
    int a[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int i;
    int *p;

    stack_init(&s);

    for(i = 0; i < 10; i++) {
        if(i == 5) {
            stack_push(&s, malloc(1000));
        }
        stack_push(&s, a + i);
    }
    while(p = stack_pop(&s)) {
        printf("%d\n", p);
    }
    return 0;
}