#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>



#define SEM_KEY 2802
#define THINK   2       //sec to sleep
#define EAT     1       //sec to sleep
#define MEALS   1       //number of meals to eat
#define N       5       //number of philosophers

char *phil_name[N] = {"Nietzsche", "Kant", "Plato", "Homer", "Aquinas"};
int phil_id, child_status, semid;

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short int *array;
    struct seminfi *__buf;
};

int sem_wait(int fork){
    struct sembuf op[1];
    op[0].sem_num = fork;
    op[0].sem_op = -1;
    op[0].sem_flg = 0; //SEM_UNDO;

    return semop(semid, op, 1);
}


int sem_post(int fork){
    struct sembuf op[1];
    op[0].sem_num = fork;
    op[0].sem_op = 1;
    op[0].sem_flg = 0; //SEM_UNDO;

    return semop(semid, op, 1);
}

void grab_forks(int left_fork_id){
    int right_fork_id = left_fork_id-1;
    if(right_fork_id < 0)
        right_fork_id = 4;
    printf("%s is tying to grab left fork #%d and right fork #%d.\n", phil_name[phil_id], left_fork_id, right_fork_id);
    sem_wait(left_fork_id);     //this is not atomic operation.
    sem_wait(right_fork_id);    //this is not atomic operation.
}

void put_away_forks(int left_fork_id){
    int right_fork_id = left_fork_id-1;
    if(right_fork_id < 0)
        right_fork_id = 4;
    printf("%s has finished and put left fork #%d and right fork #%d down on the table.\n", phil_name[phil_id], left_fork_id, right_fork_id);
    sem_post(left_fork_id);     //this is not atomic operation.
    sem_post(right_fork_id);    //this is not atomic operation. 
    

}

int sem_deallocate(int semid){
    union semun ignore_arg;
    return semctl(semid, 1, IPC_RMID, ignore_arg);
}

void eat(int meal){
    grab_forks(phil_id);
    printf("%s got forks and is eating now!\n", phil_name[phil_id]);
    sleep(EAT);
    printf("%s has eaten his %d meal!\n", phil_name[phil_id], meal);
    put_away_forks(phil_id);
}
void think(){
    printf("%s is thinking!\n", phil_name[phil_id]);
    sleep(THINK);
}

void philosopher(){
    int meal = 0;
    int isHungry = 0; //no booleans in c?!
    printf("%s said: I'm thinking and I know it!\n", phil_name[phil_id]);
    while(meal < MEALS){
        if(isHungry){
            eat(++meal);
            isHungry = 0;    
        }    
        else{
            think();
            isHungry = 1;
        }
    }   
}

int main(){
    int i;
    pid_t pid;
    key_t key = SEM_KEY;
    semid = semget(key, N, IPC_CREAT | 0666);
    
    if(semid == -1){
        perror("semget: semget failed");    
        exit(1);
    }
    
    for(i = 0; i < N; i++){
        if(semctl(semid, i, SETVAL, 1) == -1){
            perror("semctl: semctl failed");
            exit(1);
        }
    }
    
    for(i = 0; i< N; i++){
        pid = fork();
        if(pid < 0){
            kill(-2, SIGTERM);
            perror("fork: can't create a process\n");
            exit(1);
        }  
        else if(pid == 0){  //child
            phil_id = i;
            philosopher();   
            printf("%s has eaten all his meals!\n", phil_name[phil_id]);
            exit(0);
        }
    }

    //parent
    while(1){
        pid = wait(&child_status);
        if(pid < 0)
            break;    
    }   

    printf("Good bye!\n");
    sem_deallocate(semid);
    return 0;
}
