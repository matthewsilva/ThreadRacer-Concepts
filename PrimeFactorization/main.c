/*	Author:	Matthew Silva
	
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <ncurses.h>			/* ncurses.h includes stdio.h */  
#include <string.h> 
#include <sys/time.h>
#include <stdint.h>

//

void* threadFunc(void* arg);

void* userThreadFunc(void* arg);


enum DifficultyLevel {Easy, Medium, Hard};

typedef struct ThreadInfo
{
	// Order in which thread was created
	int threadIndex;
	
	int** arr;
	unsigned int rows;
	unsigned int cols;
	pthread_barrier_t* raceBarrier;
	int progress;
	int finished;
	float execTime;
	enum DifficultyLevel diff;
	
} ThreadInfo;

typedef struct UserThreadInfo
{
	// Order in which thread was created
	int threadIndex;
	
	int** arr;
	unsigned int rows;
	unsigned int cols;
	pthread_barrier_t* raceBarrier;
	int progress;
	int finished;
	float execTime;
	
} UserThreadInfo;


// Array of threads to create the difference image
pthread_t* thread;
// Array of parameter structs for the threadFunc
ThreadInfo* threadInfo;
	
// Barrier to synchronize the computation of the difference image by threads
pthread_barrier_t diffBarrier;

uint64_t get_posix_clock_time ()
{
    struct timespec ts;

    if (clock_gettime (CLOCK_MONOTONIC, &ts) == 0)
        return (uint64_t) (ts.tv_sec * 1000000 + ts.tv_nsec / 1000);
    else
        return 0;
}


int main(int argc, char** argv)
{
	printf("\n████████╗██╗  ██╗██████╗ ███████╗ █████╗ ██████╗    ██████╗  █████╗  ██████╗███████╗██████╗");
    printf("\n╚══██╔══╝██║  ██║██╔══██╗██╔════╝██╔══██╗██╔══██╗   ██╔══██╗██╔══██╗██╔════╝██╔════╝██╔══██╗");
    printf("\n   ██║   ███████║██████╔╝█████╗  ███████║██║  ██║   ██████╔╝███████║██║     █████╗  ██████╔╝");
    printf("\n   ██║   ██╔══██║██╔══██╗██╔══╝  ██╔══██║██║  ██║   ██╔══██╗██╔══██║██║     ██╔══╝  ██╔══██╗");
    printf("\n   ██║   ██║  ██║██║  ██║███████╗██║  ██║██████╔╝   ██║  ██║██║  ██║╚██████╗███████╗██║  ██║");
    printf("\n   ╚═╝   ╚═╝  ╚═╝╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝╚═════╝    ╚═╝  ╚═╝╚═╝  ╚═╝ ╚═════╝╚══════╝╚═╝  ╚═╝");
	
	printf("\n\n----------------------------------------------------------------------------------------------\n");
	
	printf("\nWelcome to Thread Racer!");
	
	printf("\n\n Please select a difficulty level (1=Easy,2=Medium,3=Hard):");
	
	int diffNum;
	enum DifficultyLevel difficulty;
	scanf("%d", &diffNum);
	//diffNum = 1;
	
	
	printf("\nPlease select the game size:");
	printf("\n\tRows: ");
	int numRows;
	scanf("%d", &numRows);
	//numRows = 100;
	printf("\n\tCols: ");
	int numCols;
	scanf("%d", &numCols);
	//numCols = 100;
	
	// TODO Add the stuff to the ThreadRacer file
	
	char inputString[2000];
  
	printf("\nEnter your implementation of the threadFunction algorithm ( press ` backtick and then enter to end input)\n");
	printf("\nPlease kindly add an extra %% symbol when using modulo to escape that character. Thank you!\n");
	printf("\nSAMPLE:");
	printf("\n");

	printf("\nfor(int i = 0; i < info->cols; i++) {");
		printf("\n\tfor (int k = 0; k < info->rows; k++) {");
			
			
			printf("\n\t\tint number,div;");    
			
			printf("\n\t\tnumber = info->arr[k][i];");
			printf("\n\t\tdiv = 2;");
			
			printf("\n\t\tint primesList [number];");
			printf("\n\t\tint primeCounter = 0;");
			
			
			printf("\n\t\twhile(number!=0){");
				printf("\n\t\t\tif(number  %% div != 0)");
				    printf("\n\t\t\t\tdiv = div + 1;");
				printf("\n\t\t\telse {");
				    printf("\n\t\t\t\tnumber = number / div;");
				    printf("\n\t\t\t\tprimesList[primeCounter] = number;");
				    printf("\n\t\t\t\tif(number==1)");
				        printf("\n\t\t\t\t\tbreak;");
				printf("\n\t\t\t}");
			printf("\n\t\t}");
			
			printf("\n\t\tint maxPrime = -1;");
			printf("\n\t\tfor (int j = 0; j < primeCounter; j++) {");
				printf("\n\t\t\tif (maxPrime < primesList[j]) {");
					printf("\n\t\t\t\tmaxPrime = primesList[j];");
				printf("\n\t\t\t}");
			printf("\n\t\t}");
			printf("\n\t\tinfo->arr[k][i] = maxPrime;");
			printf("\n\t\t(info->progress)++;");
		printf("\n\t}");
	printf("\n}");
	
	scanf("%[^`]s", inputString);
	
	char requiredEnding[200];
	strcpy(requiredEnding, "\ntime_value = get_posix_clock_time ();");

	strcat(requiredEnding, "\ntime_diff = time_value - prev_time_value;");
	strcat(requiredEnding, "\ninfo->execTime = (float) time_diff / 1000000.0f;");
	strcat(requiredEnding, "\ninfo->finished = 1;");
	
	strcat(requiredEnding, "\n}");
	
	
	char buffer[2000];
	FILE *racerFile;
	system("cp threadracer.c userThreadRacer.c");
	racerFile = fopen("userThreadRacer.c", "a");
	fprintf(racerFile, inputString);
	
	fprintf(racerFile, requiredEnding);
	
	fclose(racerFile);
	
	system("chmod +x racer.sh");
	
	char* command = (char*) malloc(256*sizeof(char));
	sprintf(command, "sh racer.sh %d %d %d", diffNum, numRows, numCols);
	system(command);
	
	return 0;
}

// Threads use this function to compute the part of the difference image they were assigned to compute
void* threadFunc(void* arg) {
	// Cast the void* parameter struct for actual use
	ThreadInfo* info = ((ThreadInfo*)arg);
	
	uint64_t prev_time_value, time_value;
	uint64_t time_diff;

	pthread_barrier_wait(info->raceBarrier);
	
	/* Initial time */
	prev_time_value = get_posix_clock_time ();

	for(int i = 0; i < info->rows; i++) {
		for (int k = 0; k < info->cols; k++) {
			int test = info->arr[i][k];
			usleep(2000);
			(info->progress)++;
		}
	}
	
	/* Final time */
	time_value = get_posix_clock_time ();

	/* Time difference */
	time_diff = time_value - prev_time_value;
	info->execTime = (float) time_diff / 1000000.0f;
	
	info->finished = 1;
	
}

// Threads use this function to compute the part of the difference image they were assigned to compute
void* userThreadFunc(void* arg) {
	// Cast the void* parameter struct for actual use
	ThreadInfo* info = ((ThreadInfo*)arg);
	
	uint64_t prev_time_value, time_value;
	uint64_t time_diff;
	
	//after the user defined function does its work
	pthread_barrier_wait(info->raceBarrier);
	
	/* Initial time */
	prev_time_value = get_posix_clock_time ();

	
	for(int i = 0; i < info->rows; i++) {
		for (int k = 0; k < info->cols; k++) {
			int test = info->arr[i][k];
			usleep(1000);
			(info->progress)++;
		}
	}
	
	/* Final time */
	time_value = get_posix_clock_time ();

	/* Time difference */
	time_diff = time_value - prev_time_value;
	info->execTime = (float) time_diff / 1000000.0f;
	info->finished = 1;
	
}
















