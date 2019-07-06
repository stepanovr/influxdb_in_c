#include <stdlib.h>
#include <stdio.h>

#include <sys/queue.h>

#include "list.h"


TAILQ_HEAD(_tailq_head_t, _tailq_entry_t) tailq_head;


void initq(struct _tailq_head_t* head)
{
        TAILQ_INIT(head);
}

void init_queue()
{
        TAILQ_INIT(&tailq_head);
}


void pushq(struct _tailq_head_t* head, tailq_entry_t* item)
{
        TAILQ_INSERT_TAIL(head, item, entries);
}

tailq_entry_t* popq(struct _tailq_head_t* head)
{
        tailq_entry_t* n1;

        if (TAILQ_EMPTY(head)){
                return NULL;
        }

        n1 = TAILQ_FIRST(head);
        TAILQ_REMOVE(head, n1, entries);
	return n1;
}

bool is_empty_q(struct _tailq_head_t* head)
{
	return TAILQ_EMPTY(head);
}

bool is_empty_queue()
{
        return TAILQ_EMPTY(&tailq_head);
}

void push(tailq_entry_t* item)
{
         pushq(&tailq_head, item);
}


tailq_entry_t* pop()
{
        return popq(&tailq_head);
}

#if 0
int main(int argc, char **argv)
{
        struct _tailq_entry_t *item;
        int i;


        initq();
        for (i = 0; i < 10; i++) {
                item = malloc(sizeof(*item));
                if (item == NULL) {
                        perror("malloc failed");
                        exit(EXIT_FAILURE);
                }

                item->index = i;

                pushq(&tailq_head, item);
        }



        for (i = 0; i < 10; i++) {
                item = popq(&tailq_head);
                printf("item: %d\n", item->index);
              free(item);
        }

        /* The tail queue should now be empty. */
        if (!TAILQ_EMPTY(&tailq_head))
                printf("tail queue is NOT empty!n");

        return 0;
}

#endif
