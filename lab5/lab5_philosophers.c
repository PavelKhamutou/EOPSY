/*
	Łukasz Grądzki
	mark: 5/5
	LAB no. 5: Dining philosophers problem implementation
	
	The Dining Philosophers
	The Dining Philosophers problem is a classic multi-process synchronization
	problem. The problem consists of five philosophers sitting at a table who do
	nothing but think and eat. Between each philosopher, there is a single fork
	In order to eat, a philosopher must have both forks. A problem can arise if
	each philosopher grabs the fork on the right, then waits for the fork on the
	left. In this case a deadlock has occurred, and all philosophers will starve.
	Also, the philosophers should be fair. Each philosopher should be able to eat
	as much as the rest.


	Implement in the C language the dining philosophers program. For
	synchronization implement and use the two following functions:

	void grab_forks( int left_fork_id );

	void put_away_forks( int left_fork_id );

	where parameters are integers identifying semaphores associated with forks.
	grab_forks() and put_away_forks() should use IPC semaphores (man semget, man
	semop) to make atomic changes on two semaphores at the same time. Print on the
	standard output verbose messages from philosophers.
*/

// To enable exit() usage:
#include <stdlib.h>
// To enable wait() usage:
#include <sys/wait.h>
// To enable semun, semget(), semctl() and semop() usage:
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
// To enable printf() usage:
#include <stdio.h>

#define SEM_KEY 17122010	// a key to a set of semaphores (any number)
#define FOOD_LIMIT 3		// number of meals for each philosopher
#define EATING_TIME 1		// eating time
#define THINKING_TIME 2		// thinking time

void grab_forks(int left_fork_id);
void put_away_forks(int left_fork_id);
void philosophers_life_cycle();
void eat(int meals_left);
void think();

char* philosophers_list[5] = {"Confucius","Pascal","Aristotle","Epictetus","Kant"};
int philosopher_id,semaphor_id,pid,status;

int main() {
	int i=0; // temp loop counter
	/*
	On the basis of the key semget(sem_key, no_of semaphores, IPC_CREAT | access_rights)
	creates or gives access permissions to a semafors' set. If different processes want
	to access the same set of semafors, the have to use the same key.
	*/
	semaphor_id = semget(SEM_KEY, 5, 0644 | IPC_CREAT); // 644 = rw- r-- r--
	if(semaphor_id == -1) {
		perror("\nERROR: Allocation of semaphor set unsuccessfull.\n");
		exit(1);
	}
	// Just after creation, semaphores have to be initialized to avoid errors in the further execution.
	while(i <= 4)
		/*
			semctl (no_of_semaphor_set, no_of_semaphor, command, command_parameters)
			It manages the semaphores.
		*/
		semctl(semaphor_id, i++, SETVAL, 1);
	i = 0;
	while(i <= 4) {
		pid = fork();
		if(pid == 0) {
			philosopher_id = i;
			philosophers_life_cycle();
			printf ("%s has left the table.\n",philosophers_list[philosopher_id]);
			return 0;
		}
		else if(pid < 0) {
			kill(-2,SIGTERM);
			perror("\nERROR: can't create a processes\n");
			exit(1);
		}
		++i;
	}
	while(1) {
		pid = wait(&status);
		if(pid < 0)
			break;
	}
	i=0;
	// Deallocation of semaphores:
	if (semctl (semaphor_id, 0, IPC_RMID, 1)<0)
		printf("ERROR deallocationg semaphores.\n");

	return 0;
}

void grab_forks(int left_fork_id) {
	// select right fork with id one less than id of the left fork
	int right_fork_id = left_fork_id-1;
	if(right_fork_id<0)
		right_fork_id=4;
	printf("%s is going to use left fork no. %d and right fork no. %d.\n", philosophers_list[philosopher_id],left_fork_id, right_fork_id);
	/*
	struct sembuf {
		ushort semnum; - a number of the semaphor (fork) in a set of semaphors.
		short sem_op; - operation on semaphor:	sem_op > 0 -> increase sem by this val,
							sem_op < 0 -> decrease sem by this val,
							sem_op = 0 -> operation Z
		ushort sem_flg; - values: 0 - blocking operation, IPC_NOWAIT - nonblocking operation
	};
	*/
	struct sembuf semaphor_as_a_fork[2] = {
		{right_fork_id,-1,0},
		{left_fork_id,-1,0}
	};
	/*
	semop(semaphor_id, semaphor_buffer, no_of_semaphors_on_which_op_is_performed)
	Performs operations over semaphors.
	*/
	semop(semaphor_id, semaphor_as_a_fork, 2);
}

void put_away_forks(int left_fork_id) {
	int right_fork_id = left_fork_id-1;
	if(right_fork_id<0)
		right_fork_id=4;
	printf("%s puts away left fork no. %d and right fork no. %d.\n", philosophers_list[philosopher_id], left_fork_id, right_fork_id);
	struct sembuf semaphor_as_a_fork[2] = {
		{right_fork_id,1,0},
		{left_fork_id,1,0}
	};
	semop(semaphor_id, semaphor_as_a_fork, 2);
}

void philosophers_life_cycle() {
	printf("%s came to the table.\n", philosophers_list[philosopher_id]);
	int no_of_meals = FOOD_LIMIT;
	char hungry = 0;
	while(no_of_meals) {
		if(hungry) {
			eat(--no_of_meals);
			hungry = 0;
		}
		else {
			think();
			hungry = 1;
		}
	}
}

void eat(int meals_left) {
	grab_forks(philosopher_id);
	printf("%s is eating\n", philosophers_list[philosopher_id]);
	sleep(EATING_TIME);
	printf("%s ate his %d",philosophers_list[philosopher_id],(FOOD_LIMIT-meals_left));
			switch (FOOD_LIMIT-meals_left){
				case 1:
					printf("st");
					break;
				case 2:
					printf("nd");
					break;
				case 3:
					printf("rd");
					break;
				default:
					printf("th");
			}
			printf(" meal.\n");
	put_away_forks(philosopher_id);
}

void think() {
	printf("%s is thinking\n", philosophers_list[philosopher_id]);
	sleep(THINKING_TIME);
}
