#include <stdio.h>
#include <stdlib.h>



/* Normal Linked List */

struct Node{
    char name[256];
    struct Node *next;
};




struct Node *add(char data[] , struct Node *head){
    if(head == NULL){
        struct Node *new_head = (struct Node *)malloc(sizeof(struct Node));
        strcpy(new_head->name , data);
        new_head->next = NULL;
        return new_head;
    }
    else{
        struct Node *temp = head;
        while(temp->next != NULL)
            temp = temp->next;
        struct Node *new_node = (struct Node *)malloc(sizeof(struct Node));
        temp->next = new_node;
        strcpy(new_node->name , data);
        new_node->next = NULL;
        return head;
    }
}

/* PRINT */
void printList(struct Node *head){
    while (head != NULL){
        printf("%s", head->name);
        head = head->next;
    }
}

