#ifndef STACK_H
#define STACK_H

/* Forward declaration */
typedef struct stack_node stack_node;

struct stack_node{
    stack_node *next;
    void *data;
};

typedef struct {
    stack_node *top;
    int size;

} stack;

int stack_init(stack *s);
int stack_destroy(stack *s);
int stack_push(stack *s, void *v);
void *stack_pop(stack *s);
void *stack_top(stack *s);
int size(stack *s);
int is_empty(stack *s);



#endif