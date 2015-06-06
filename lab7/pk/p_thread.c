#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define THINKING_TIME   1
#define EATING_TIME     2
#define MEALS_AMOUNT    2
#define N               5

#define LEFT    (philosopher_id + N - 1) % N
#define RIGHT   (philosopher_id + 1) % N

#define THINKING_STATE  1
#define HUNGRY_STATE    2
#define EATING_STATE    3

void grab_forks(int philosopher_id);
void put_away_forks(int philosopher_id);
void test(int philosopher_id);
void initialize_mutex();
void* philosopher_life(void* philosopher_id);
void think(int philosopher_id);
void eat(int philosopher_id, int current_meal);

pthread_mutex_t m;
pthread_mutex_t s[N];
int state[N];
char *p_name[N] = {"Nietzsche", "Kant", "Plato", "Homer", "Aquinas"};


int main(){
    pthread_t philosopher_thread[N];
    int i;
    int philosopher_id_array[N];
    initialize_mutex();
    
    for(i = 0; i < N; i++){
        philosopher_id_array[i] = i;
        if(pthread_create(&(philosopher_thread[i]), NULL, philosopher_life, (void *) &philosopher_id_array[i]) != 0){
            perror("phtread_create");
            exit(1);
        }
    }


    for(i = 0; i < N; i++){
        pthread_join(philosopher_thread[i], NULL);
    }
    pthread_mutex_destroy(&m);

    for(i = 0; i < N; i++){
        pthread_mutex_destroy(&s[i]);
    }
    pthread_exit(NULL);

    return 0;
}

void* philosopher_life(void* philosopher_id){
    int meal_counter;
    int id = *(int*) philosopher_id;
    printf("p_id: [%d] name: [%s] \thas joined the table.\n", id, p_name[id]);
    for(meal_counter = 1; meal_counter < MEALS_AMOUNT + 1; meal_counter++){
        think(id);
        eat(id, meal_counter);
    }
    printf("p_id: [%d] name: [%s] \thas left the table.\n", id, p_name[id]);
}

void eat(int philosopher_id, int current_meal){
    printf("p_id: [%d] name: [%s] \tis going to grab left fork [%d] and right fork [%d].\n", philosopher_id, p_name[philosopher_id], philosopher_id, RIGHT);
    grab_forks(philosopher_id);
    printf("p_id: [%d] name: [%s] \thas grabed forks and is eating meal [%d] with left fork [%d] right fork [%d].\n", philosopher_id, p_name[philosopher_id], 
            current_meal, philosopher_id, RIGHT);
    sleep(EATING_TIME);
    printf("p_id: [%d] name: [%s] \thas finished his meal [%d] with left fork [%d] right fork [%d].\n", philosopher_id, p_name[philosopher_id], 
            current_meal, philosopher_id, RIGHT);
    put_away_forks(philosopher_id);
}

void grab_forks(int philosopher_id){
    pthread_mutex_lock(&m);
    state[philosopher_id] = HUNGRY_STATE;
    test(philosopher_id);
    pthread_mutex_unlock(&m);
    pthread_mutex_lock(&s[philosopher_id]);   
}

void put_away_forks(int philosopher_id){
    pthread_mutex_lock(&m);
    state[philosopher_id] = THINKING_STATE;
    test(LEFT);
    test(RIGHT);
    pthread_mutex_unlock(&m);    
}

void test(int philosopher_id){
    if(state[philosopher_id] == HUNGRY_STATE && state[LEFT] != EATING_STATE && state[RIGHT] != EATING_STATE){
        state[philosopher_id] = EATING_STATE;
        pthread_mutex_unlock(&s[philosopher_id]);
    }
}

void initialize_mutex(){
    int i;
    if(pthread_mutex_init(&m, NULL) != 0){
        perror("mutex m");
        exit(1);
    }
    for(i = 0; i < N; i++){
        if(pthread_mutex_init(&s[i], NULL) != 0){
            perror("mutex s");
            exit(1);
        }
        pthread_mutex_lock(&s[i]);
        state[i] = THINKING_STATE;
    }
}


void think(int philosopher_id){
    printf("p_id: [%d] name: [%s] \tis thinking.\n", philosopher_id, p_name[philosopher_id]);
    sleep(THINKING_TIME);
}


