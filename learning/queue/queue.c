int queue_remove(struct queue *q, int *i) {
    struct queue_node *q;

    if(!q->front) {
        return -1;
    }

    n = q->front;
    *i = n->data;
    if(!(q->front = n->next)) {
        q->back = NULL;
    }
    q->size--;
    q->front = n->next;

    free(n);
    return 0;
}