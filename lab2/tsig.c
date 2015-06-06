#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

#define	NUM_CHILD	5  
#define WITH_SIG

int CREATED_CHILDS = 0;
pid_t CHILD_IDS[NUM_CHILD]; //array of child's ids.
#ifdef WITH_SIG
char interrupt_flag = 0;
void killedChild() { 
	printf("child[%d]: Termination of the process.\n", getpid());
	exit(1);
}
void keyboardInterrupt() {
	printf("parent[%i]: The keyboard interrupt received. (Ctrl+C)\n", getpid());
	interrupt_flag = 1;
}

#endif
void forkChild(int i){
	
	pid_t child_pid = fork();
	if(child_pid == -1){
		fprintf(stderr, "an error occured in fork()\n");
		int j;
		for(j = 0; j < CREATED_CHILDS; ++j){
			kill(CHILD_IDS[j], SIGTERM);
		}
		exit (1);
	}
	if(child_pid > 0){
		CHILD_IDS[i] = child_pid;
		CREATED_CHILDS++;
	}
	if(child_pid == 0){
		#ifdef WITH_SIG
			signal (SIGINT,killedChild);
		#endif
		printf("child[%d]: has been created\n", (int) getpid());
		printf("child[%d]: falling asleep for 10s\n", (int) getpid());
		sleep(10);
		printf("child[%d]: execution completed\n", (int) getpid());
		exit(0);
	}

}

int main(){
	//int tsig_id = (int) getpid();
	int i, j;
	printf("parent[%d]: has been created\n", (int) getpid());
	for(i = 0; i < NUM_CHILD; ++i){
		#ifdef WITH_SIG
			for(j = 0; j < NSIG; ++j){ 
				sigignore(j);
			}
			signal (SIGCHLD, SIG_DFL); //SIG_DFL specifies the default disposition for the signal
			signal (SIGINT, keyboardInterrupt); //SIG_INT specifies that the signal should be ignored
		#endif
		forkChild(i);
		sleep(1);
		#ifdef WITH_SIG
			if (interrupt_flag == 1){
				printf("parent[%i]: Interrupt of the creation process!\n", getpid());
				kill(-2, SIGTERM);
				break;
			}
		#endif
	}
	//#ifdef WITH_SIG
	if(CREATED_CHILDS == NUM_CHILD) 
		printf("parent[%d]: All processes have been created.\n", (int) getpid());

	int child_status;
	pid_t w[CREATED_CHILDS];
	int exit_code[CREATED_CHILDS];
	
	for(i = 0; i < CREATED_CHILDS; i++){
		w[i] = wait(&child_status);
		if(w[i] == -1)
			break;
		else{	
			if(WIFEXITED (child_status))	
				exit_code[i] = WEXITSTATUS(child_status);
		}
	}

	printf("parent[%d]: There are no more child processes.\n", (int) getpid());
	printf("parent[%d]: Terminated processes [%d]\n", (int) getpid(), CREATED_CHILDS);
	for(i = 0; i < CREATED_CHILDS; i++)
		printf("child[%d]: EXIT_CODE [%d]\n", w[i], exit_code[i]);
		
	//int waitid(idtype_t idtype, id_t id, siginfo_t *infop, int options);



	#ifdef WITH_SIG
		for(j=0; j<NSIG; j++)
			signal(j, SIG_DFL);
	#endif

	return 0;
}
