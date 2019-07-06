#ifndef __LIST_H__
#define __LIST_H__

#include <stdbool.h>

typedef struct _payload_t{
        char* string;
}payload_t;

struct _tailq_head_t;


typedef struct _tailq_entry_t{
        TAILQ_ENTRY(_tailq_entry_t) entries;
        int index;
        payload_t* payload;

}tailq_entry_t;



void initq(struct _tailq_head_t* head);
void init_queue();
void pushq(struct _tailq_head_t* head, tailq_entry_t* item);
tailq_entry_t* popq(struct _tailq_head_t* head);
bool is_empty_q(struct _tailq_head_t* head);
bool is_empty_queue();

void push(tailq_entry_t* item);
tailq_entry_t* pop();


#endif
