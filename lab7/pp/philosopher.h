#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define __THINK_TIME   		    1
#define __EATING_TIME     	    2
#define __NR_OF_MEALS   	    2
#define __NR_OF_PHILOSOPHERS        5

#define LEFT		( *philosopherID + (__NR_OF_PHILOSOPHERS) - 1 ) % (__NR_OF_PHILOSOPHERS)
#define RIGHT		( *philosopherID + 1 ) % (__NR_OF_PHILOSOPHERS)

enum PhilosophersStateEnum
{
    THINKING = 1,
    HUNGRY = 2,
    EATING  = 3
} typedef PhilosophersStateEnum;

void grabForks(const int* philosopherID);
void putAwayForks( const int* philosopherID );
void test(const int* philosopherID);
void think(const int* philosopherID);
void eat(const int* philosopherID,const int* mealNr);
void *philosopher(void *philosopherID);
void initializeMutex();
void preparePhilosopers(int *philosopherIds, pthread_t *philosophersThreads);
void finalize(const pthread_t *philosophersThreads);

pthread_mutex_t m; 		
PhilosophersStateEnum state[__NR_OF_PHILOSOPHERS];
pthread_mutex_t s[__NR_OF_PHILOSOPHERS];
