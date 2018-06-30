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
	int diffNum;
	int numRows;
	int numCols;
	
	sscanf(argv[1], "%d", &diffNum);
	sscanf(argv[2], "%d", &numRows);
	sscanf(argv[3], "%d", &numCols);
	
	enum DifficultyLevel difficulty;
	
	switch(diffNum)
	{
		case(1):
			difficulty = Easy;
			break;
		case(2):
			difficulty = Medium;
			break;
		case(3):
			difficulty = Hard;
			break;
	}
	
	int** threadArr = (int**) malloc(numRows*sizeof(int*));
	for (int i = 0; i < numRows; i++) {
		threadArr[i] = (int*) malloc(numCols*sizeof(int));
	}
	
	srand((unsigned int) time(NULL));
	for (int i = 0; i < numRows; i++) {
		for (int k = 0; k < numCols; k++) {
			threadArr[i][k] = rand()%500;
		}
	}
	
	int** userArr = (int**) malloc(numRows*sizeof(int*));
	for (int i = 0; i < numRows; i++) {
		userArr[i] = (int*) malloc(numCols*sizeof(int));
	}
	
	for (int i = 0; i < numRows; i++) {
		for (int k = 0; k < numCols; k++) {
			userArr[i][k] = threadArr[i][k];
		}
	}
	
	int timedout = 0;
	
	// TODO Set up timeoutThread
	
	pthread_barrier_t raceBarrier;
	pthread_barrier_init(&raceBarrier, NULL, 2);
	
	ThreadInfo params;
	UserThreadInfo userParams;
	
	params.arr = threadArr;
	userParams.arr = userArr;
	params.rows = userParams.rows = numRows;
	params.cols = userParams.cols = numCols;
	params.raceBarrier = userParams.raceBarrier = &raceBarrier;
	params.progress = userParams.progress = 0;
	params.finished = userParams.finished = 0;
	
	params.diff = difficulty;
	
	pthread_t thread;
	pthread_t userThread;
	
	pthread_create(&thread, NULL, threadFunc, &params);
	pthread_create(&userThread, NULL, userThreadFunc, &userParams);
	
	
	int totalElements = numRows*numCols;
	// Display progress bar based on Indexes or on a numProcessed counter
    float percentFinished;
    int fractionFinished;
    float userPercentFinished;
    int userFractionFinished;
    printf("\n");
    
/*    initscr();
    int row,col;				
	refresh();
	printw("THREAD PROGRESS:");
		
	for (int i = 0; i < 10; i++) {
        printw("#");
    }
    for (int i = 0; i < 0; i++) {
        printw("=");
    }
    
	printw("\n");
    printw("USER THREAD PROGRESS:");
    for (int i = 0; i < 10; i++) {
        printw("#");
    }
    for (int i = 0; i < 0; i++) {
        printw("=");
    }
	getch(); 
*/	char* msg = (char*) malloc(256*sizeof(char));
	initscr();
	clear();
	refresh();
	printw("\n _____ _                        _  ______                    ");
	printw("\n|_   _| |                      | | | ___ \\                   ");              
	printw("\n  | | | |__  _ __ ___  __ _  __| | | |_/ /__ _  ___ ___ _ __ ");
	printw("\n  | | | '_ \\| '__/ _ \\/ _` |/ _` | |    // _` |/ __/ _ \\ '__|");
	printw("\n  | | | | | | | |  __/ (_| | (_| | | |\\ \\ (_| | (_|  __/ |   ");
	printw("\n  \\_/ |_| |_|_|  \\___|\\__,_|\\__,_| \\_| \\_\\__,_|\\___\\___|_|   ");
	                                                     
	printw("\n");
	int row,col;				
    getmaxyx(stdscr,row,col);		
	for (int i = 0; i < col; i++) {
		printw("-");
	}
	while ((params.finished != 1 || params.finished != 1) && timedout != 1) {

 		
 		
		percentFinished = (float) (params.progress) / (float) (totalElements);
        fractionFinished = (int) (percentFinished*10);
        
        strcpy(msg,"THREAD PROGRESS:\t[");
		
		for (int i = 0; i < fractionFinished; i++) {
            if (i+1 != fractionFinished) {
            	strcat(msg,"=");
            }
            else {
            	strcat(msg,">");
            }
        }
        for (int i = 0; i < 10-fractionFinished; i++) {
            strcat(msg," ");
        }
		strcat(msg,"]");
        
		userPercentFinished = (float) (userParams.progress) / (float) (totalElements);
        userFractionFinished = (int) (userPercentFinished*10);
        strcat(msg,"\n");
        strcat(msg,"USER THREAD PROGRESS:\t[");
        for (int i = 0; i < userFractionFinished; i++) {
            if (i+1 != userFractionFinished) {
            	strcat(msg,"=");
            }
            else {
            	strcat(msg,">");
            }
        }
        for (int i = 0; i < 10-userFractionFinished; i++) {
            strcat(msg," ");
        }
		strcat(msg,"]");
		
		move(8,0);
        printw(msg);

		refresh();
		usleep(10000);



	}
	clear();
	refresh();
	
	int answerWrong = 0;
	for (int i = 0; i < numRows; i++) {
		for (int k = 0; k < numCols; k++) {
			if (threadArr[i][k] != userArr[i][k])
				answerWrong = 1;
		}
	}
	
	if (answerWrong == 1) {
		printw("Your implementation was not correct! Try again.");
		printw("\n\n\n");
		printw("\nPress any key to exit the game...");
		getch();
		endwin();
	
		exit(1);
	}
	
    if (timedout == 0) {
        if (userParams.execTime < params.execTime) {
            printw("Your implementation was %f seconds faster. Good job!", params.execTime - userParams.execTime);
        }
        else {
            printw("Your implementation was %f seconds worse. Try again!", userParams.execTime - params.execTime);
        }
    }

    else {
        printw("Your code did not finish executing. Exiting program, try again.");
        printw("\n\n\n");
		printw("\nPress any key to exit the game...");
		getch();
		endwin();

    }
    
    // TODO pthread_join(
	
	printw("\n\n\n");
	printw("\nPress any key to exit the game...");
	getch();
	endwin();
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

	for(int i = 0; i < info->cols; i++) {
		for (int k = 0; k < info->rows; k++) {
			
			
			int number,div;    
			
			number = info->arr[k][i];
			div = 2;
			
			int primesList [number];
			int primeCounter = 0;
			
			
			while(number!=0){
				if(number  % div != 0)
				    div = div + 1;
				else {
				    number = number / div;
				    primesList[primeCounter] = number;
				    if(number==1)
				        break;
				}
			}
			
			int maxPrime = -1;
			for (int j = 0; j < primeCounter; j++) {
				if (maxPrime < primesList[j]) {
					maxPrime = primesList[j];
				}
			}
			info->arr[k][i] = maxPrime;
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


	
	


















time_value = get_posix_clock_time ();
time_diff = time_value - prev_time_value;
info->execTime = (float) time_diff / 1000000.0f;
info->finished = 1;
}