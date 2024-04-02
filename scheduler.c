/*
 * A simeple scheduler
 * All tasks have same priority
 * Time slice for tasks can be set by user
 * User can add new task
 *
 *
 * */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


/**
 * @Breif task status
 */
typedef enum {
    TASK_RUNNING,
    TASK_READY,
    TASK_SUSPEND
} TASK_STATUS;

typedef struct task_parameter task_parameter_s;
typedef struct task_cb task_cb;
/* define the function point for task */
typedef void (*fp)(void *para);

/**
 * @Breif task control block
 */
typedef struct task_cb{
    char name[50];
    int id;
    //int priority;
    //int execution_time;
    void *para;
    fp func;
    task_cb *next;
    task_cb *prev;
    TASK_STATUS status;
} task_cb;

/* 3 list for task management */
task_cb *ready_list = NULL;
task_cb *wait_list = NULL;
task_cb *susp_list = NULL;

task_cb *curr = NULL;    /* current task */


/**
 * @Breif insert a task member into a list
 *
 * @Param head_p: head of the list
 * @Param target: to be inserted
 *
 * @Returns
 */
int  list_insert_end(task_cb **head_p, task_cb *target){
    task_cb *temp;
    task_cb *head = *head_p;
    /* when the list is empty */
    if (head == NULL){
        *head_p = target;
        printf("[%s] insert first member: %s.\n", __FUNCTION__, (*head_p)->name);
        return 0;
    }

    /* when the list is not empty */
    temp = head;
    while (temp != NULL && temp->next != NULL){
        temp = temp->next;
    }

    temp->next = target;
    target->prev = temp;
    printf("[%s] insert member: %s.\n", __FUNCTION__, temp->next->name);

    return 0;
}

/**
 * @Breif Delete a member from the list
 *
 * @Param head: head of the list
 * @Param id: id of the task to be removed from the list
 *
 * @Returns
 */
task_cb* list_delete_m(task_cb *head, int id){
    task_cb *temp = head;
    task_cb *delete;

    /* the list is empty */
    if (head == NULL){
        return -1;
    }

    /* try to find the member via its id */
    while (temp->id != id){
        temp = temp->next;
    }

    if (temp->next == NULL && temp->id != id){
        return -1; /* didn't find the task in this list */
    }

    /* found the task and deleted it from the list */
    delete = temp;
    if (temp->next == NULL){
       temp->prev->next = NULL;
       temp->prev = NULL;
    }

    return delete;
}

/**
 * @Breif Check the members in the list
 *
 * @Param head
 *
 * @Returns
 */
int list_test(task_cb *head){
    if (head == NULL){
        printf(" Empty list\n");
        return 0;
    }

    task_cb *temp = head;
    while(temp != NULL){
        printf("List current member:%s\n", temp->name);
        temp = temp->next;
    }

}


/**
 * @Breif Init a task
 *
 * @Param t
 * @Param id
 * @Param name
 * @Param funcp
 * @Param para
 */
void init_task(task_cb *t, int id, char *name, void *funcp, void *para){
    t->id = id;
    strcpy(t->name, name);
    t->func = (fp)funcp;
    t->para = para;
    t->prev = NULL;
    t->next = NULL;
    list_insert_end(&ready_list, t);
}



/**
 * @Breif Start scheduler, implement the first tatsk in the ready list
 *
 * @Returns
 */
int start_scheduler(void) {
    printf("%s started.\n", __FUNCTION__);
    curr = ready_list;
    for (;;){
        if (curr == NULL){ /* no ready task */
            sleep(5);
        }
        else {
            curr->func(curr);
            if (curr->next == NULL){
                curr = ready_list;
            }
            else {
                curr = curr->next;
            }
        }
    }
}

/**
 * @Breif Print task name 
 *
 * @Param p
 */
void print_taskname(void *p){
    task_cb *pa = (task_cb *)p;
    printf("start %s\n", pa->name);
    //printf("start \n");
    sleep(1);
}

void func_b(void *p){
    return 0;
}

/**
 * @Breif the Main
 *
 * @Returns
 */
int main() {
    task_cb task_1;
    task_cb task_2;
    init_task(&task_1, 0, "12345678901112131415", (void *)(&print_taskname), NULL);
    init_task(&task_2, 1, "abcdddddddddd", (void *)(&print_taskname), NULL);
    list_test(ready_list);
    start_scheduler();

    return 0;
}

