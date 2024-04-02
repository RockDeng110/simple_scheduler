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

#pragma pack(1)
typedef enum {
    TASK_RUNNING,
    TASK_SUSPEND
} TASK_STATUS;

typedef struct task_parameter task_parameter_s;
/* define the function point for task */
typedef void (*FUNC_VOID)(void *para);
// Define a structure to represent a task
typedef struct {
    char name[50];
    int id;
    //int priority;
    //int execution_time;
    FUNC_VOID func;
    void *para;
    //TASK_STATUS status;
} task_cb;


void init_task(task_cb *t, int id, char *name, void *funcp, void *para){
    t->id = id;
    //t->name = name;
    printf("[init task] %c\n", *name);
    printf("[init task] %s\n", name);
    printf("[inti task] size:%d\n", sizeof(task_cb));
    printf("[sizeof int:%d sizeof void p:%d sizeof enum:%d\n", sizeof(int), sizeof(void*), sizeof(TASK_STATUS));
    //memcpy(t->name, name, strlen(name)+1);
    strcpy(t->name, name);
    t->func = (FUNC_VOID)funcp;
    t->para = para;
}


/**
 * @Breif Regist task to scheduler
 */
void register_task(){
}



void start_scheduler(){
    // find an active task and call its function
}


// Define a simple scheduler function
//void simple_scheduler(Task *tasks, int num_tasks) {
//    // Execute tasks in priority order
//    for (int i = 0; i < num_tasks; i++) {
//        Task *task = &tasks[i];
//        printf("Scheduling task %d with priority %d\n", task->id, task->priority);
//        perform_task(task);
//    }
//}

void func_a(void *p){
    task_cb *pa = (task_cb *)p;
    printf("start %s\n", pa->name);
    //printf("start \n");
    sleep(5);
}

int main() {
    // Define some example tasks
    // Run the scheduler with the example tasks
    //simple_scheduler(tasks, num_tasks);
    task_cb task_a;
    init_task(&task_a, 0, "12345678901112131415", (void *)(&func_a), NULL);
    //((FUNC_VOID)(task_a.func))(task_a.para);
    task_a.func(task_a.name);
    //start_scheduer();

    return 0;
}

