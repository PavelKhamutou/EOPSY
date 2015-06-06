#include "philosopher.h"

int main()
{
    int philosopherIds[__NR_OF_PHILOSOPHERS];
    pthread_t philosophersThreads[__NR_OF_PHILOSOPHERS];

    initializeMutex();
    preparePhilosopers(philosopherIds,philosophersThreads);
    finalize(philosophersThreads);

    return 0;
}

void initializeMutex()
{
    int i,result;
    if ((result = pthread_mutex_init(&m, NULL)) != 0)
    {
        perror("mutex");
        exit(1);
    }
    for(i = 0; i < __NR_OF_PHILOSOPHERS; i++)
    {
        if ((result = pthread_mutex_init(&s[i], NULL)) != 0)
        {
            perror("mutex");
            exit(2);
        }
        pthread_mutex_lock(&s[i]);
        state[i]=THINKING;
    }
}

void preparePhilosopers(int *philosopherIds, pthread_t *philosophersThreads)
{
    int i;
    for(i=0; i<__NR_OF_PHILOSOPHERS; ++i)
    {
        philosopherIds[i] = i;
        if(pthread_create(&philosophersThreads[i], NULL, philosopher, (void *)&philosopherIds[i])!=0)
        {
            fprintf(stderr,"\nERROR: pthread_create() failed. Cannot create philosopher!\n");
            exit(3);
        }
    }
}

void *philosopher(void *philosopherID)
{
    int mealCnt = 0;
    printf("Philospoher NR. %d \tjoined the table\n", *(int*)philosopherID);
    for(; mealCnt < __NR_OF_MEALS; mealCnt++)
    {
        think((int*)philosopherID);
        eat((int*)philosopherID,&mealCnt);
    }
    printf("Philospoher NR. %d \tFinished all of meals\n", *(int*)philosopherID);

}

void think(const int* philosopherID)
{
    printf("Philospoher NR. %d \tThink\n", *philosopherID);
    sleep(__THINK_TIME);
}

void eat(const int* philosopherID,const int* mealNr)
{
    printf("Philospoher NR. %d \tReaches For\t      fork #%d and fork #%d.\n", *philosopherID, *philosopherID , RIGHT);
    grabForks(philosopherID);
    printf("Philospoher NR. %d \tStarted meal #%d\tUsing fork #%d and fork #%d.\n", *philosopherID, *mealNr,*philosopherID,RIGHT);
    sleep(__EATING_TIME);
    putAwayForks(philosopherID);
    printf("Philospoher NR. %d \tFinished\tUsing fork #%d and fork #%d.\n",*philosopherID,*philosopherID,RIGHT);
}

void grabForks(const int* philosopherID)
{
    pthread_mutex_lock(&m);
    state[*philosopherID]=HUNGRY;
    test(philosopherID);
    pthread_mutex_unlock(&m);
    pthread_mutex_lock(&s[*philosopherID]);
}

void putAwayForks( const int* philosopherID )
{
    int left = LEFT;
    int right = RIGHT;
    pthread_mutex_lock(&m);
    state[*philosopherID]=THINKING;
    test(&left);
    test(&right);
    pthread_mutex_unlock(&m);
}

void test(const int* philosopherID)
{
    if( state[*philosopherID] == HUNGRY
            && state[LEFT] != EATING
            && state[RIGHT] != EATING )
    {
        state[*philosopherID] = EATING;
        pthread_mutex_unlock(&s[*philosopherID]);
    }
}

void finalize(const pthread_t *philosophersThreads)
{
    int i;
    for(i=0; i<__NR_OF_PHILOSOPHERS; ++i)
    {
        pthread_join(philosophersThreads[i],NULL);
    }
    pthread_mutex_destroy(&m);
    for(i=0; i<__NR_OF_PHILOSOPHERS; ++i)
    {
        pthread_mutex_destroy(&s[i]);
    }
    pthread_exit(NULL);
}




