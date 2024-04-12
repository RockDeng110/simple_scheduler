/*
 * A simeple scheduler
 * All tasks have same priority
 * Time slice for tasks can be set by user
 * User can add new task
 *
 *
 * */
#define _POSIX_C_SOURCE 199309
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>

#define TIMER_SIGNAL SIGRTMIN+1

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

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * @Breif task control block
 */
typedef struct task_cb{
    char name[50];
    fp func;
    task_cb *next;
    task_cb *prev;
    int id;
    int priority;
    int execution_time;
    int wait_time;
    void *para;
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
        printf("[%s] insert first member. id:%d name:%s.\n", __FUNCTION__, (*head_p)->id, (*head_p)->name);
        return 0;
    }

    /* when the list is not empty */
    temp = head;
    while (temp != NULL && temp->next != NULL){
        temp = temp->next;
    }

    temp->next = target;
    target->prev = temp;
    printf("[%s] insert member. id:%d name:%s.\n", __FUNCTION__, temp->next->id, temp->next->name);

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
task_cb* list_delete_m(task_cb **head_p, int id){
    printf("[list_delete_m]\n");
    task_cb *head = *head_p;
    task_cb *delete;
    task_cb *temp;

    /* the list is empty */
    if (head == NULL){
        return -1;
    }

    /* try to find the member via its id */
    temp = head;
    while (temp->id != id){
        if (temp->next != NULL){
            temp = temp->next;
        }
        else {
            return -1; /* didn't find the task in this list */
        }
    }

    //if (temp->next == NULL && temp->id != id){
    //    return -1; /* didn't find the task in this list */
    //}

    /* found the task and deleted it from the list */
    delete = temp;
    /* replace its previous' next */
    if (delete->prev != NULL){
        delete->prev->next = delete->next;
    }
    else {
        /* When this is the first node, the head */
        *head_p = delete->next;
        if (*head_p != NULL){
            (*head_p)->prev = NULL;
        }

    }
    /* replace its next's previous */
    if (delete->next != NULL){
        delete->next->prev = delete->prev;
    }
    else {
        /* when this is the last node */
        if (delete->prev != NULL){
            delete->prev->next = NULL;
        }
    }
    /* reset its prev and next */
    delete->next = NULL;
    delete->prev = NULL;

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
        printf("List current member. id:%d wait:%d name:%s\n", temp->id, temp->wait_time, temp->name);
        temp = temp->next;
    }

}

void switch_taskstate(task_cb **from, task_cb **to, task_cb *who){
    printf("[switch_taskstate]\n");
    list_delete_m(from, who->id);
    list_insert_end(to, who);
    list_test(wait_list);
}

void timer_handler(union sigval sv) {
//void timer_handler(int sig, siginfo_t *si, void *uc) {
    task_cb *tobe_switch;
    task_cb *head = wait_list;

    //pthread_mutex_lock(&mutex);
    //printf("System Timer Expired.\n");
    if (head == NULL){
        /* do nothing */
        //pthread_mutex_unlock(&mutex);
        return;
    }

    printf("[timer_handler]\n");
    task_cb *temp = head;
    while(temp != NULL){
        if (temp->wait_time > 0){
            temp->wait_time--;
            printf("[timer_handler] id:%d wait:%d\n", temp->id, temp->wait_time);
            temp = temp->next;
        }
        else {
            printf("[timer_handler] swith to ready. id:%d\n", temp->id);
            tobe_switch = temp;
            temp = temp->next;
            switch_taskstate(&wait_list, &ready_list, tobe_switch);
        }
    }
    //pthread_mutex_unlock(&mutex);
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
    t->wait_time = 0;
    list_insert_end(&ready_list, t);
}

void delay_s(int sec){
    task_cb *temp;
    printf("[delay] curr id:%d\n", curr->id);
    /* set its wait time */
    //pthread_mutex_lock(&mutex);
    curr->wait_time = sec;
    temp = curr;
    curr = temp->next;
    /* set state of curr to wait */
    switch_taskstate(&ready_list, &wait_list, temp);
    //pthread_mutex_unlock(&mutex);

}


/**
 * @Breif Start scheduler, implement the first tatsk in the ready list
 *
 * @Returns
 */
int start_scheduler(void) {
    printf("%s started.\n", __FUNCTION__);

    struct timespec ts;
    ts.tv_sec = 1; // 1 seconds
    ts.tv_nsec = 0;

    curr = ready_list;
    for (;;){
        printf("[start_scheduler]\n");
        //pthread_mutex_lock(&mutex);
        if (curr == NULL){ /* no ready task */
            //nanosleep(&ts, NULL);
            sleep(1);
            printf("[start_scheduler] no ready task.\n");
            /* update curr */
            curr = ready_list;
        }
        else {
            curr->func(curr);
            if (curr == NULL){
                //pthread_mutex_unlock(&mutex);
                continue;
            }
            if (curr->next == NULL){
                curr = ready_list;
            }
            else {
                curr = curr->next;
            }
        }
        //pthread_mutex_unlock(&mutex);
    }
}

/**
 * @Breif Print task name 
 *
 * @Param p
 */
void print_taskname(void *p){
    //pthread_mutex_lock(&mutex);
    task_cb *pa = (task_cb *)p;
    printf("start %s\n", pa->name);
    //pthread_mutex_unlock(&mutex);
    delay_s(1);
    //sleep(1);
}

void print_taskid(void *p){
    //pthread_mutex_lock(&mutex);
    task_cb *pa = (task_cb *)p;
    printf("start taskid. id:%d\n", pa->id);
    //pthread_mutex_unlock(&mutex);
    delay_s(5);
    //sleep(1);
}

void func_b(void *p){
    printf("start func_b.\n");
    delay_s(10);
    //sleep(3);
}

/**
 * @Breif the Main
 *
 * @Returns
 */
int main() {

    // Create and arm a POSIX timer
    struct sigevent sev;
    timer_t timerid;
    struct itimerspec its;
    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_notify_function = timer_handler;
    sev.sigev_value.sival_ptr = &timerid;
    timer_create(CLOCK_REALTIME, &sev, &timerid);
    its.it_value.tv_sec = 1; // 1 second interval
    its.it_value.tv_nsec = 0;
    its.it_interval.tv_sec = 1;
    its.it_interval.tv_nsec = 0;
    timer_settime(timerid, 0, &its, NULL);


    /* task init */
    task_cb task_1;
    task_cb task_2;
    init_task(&task_1, 0, "The first task", (void *)(&print_taskid), NULL);
    init_task(&task_2, 1, "The second task", (void *)(&func_b), NULL);
    list_test(ready_list);

    start_scheduler();

    return 0;
}

