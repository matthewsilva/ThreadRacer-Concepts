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
	
	
	printf("\nPlease select the picture for the game:");
	printf("\n\tPicture filepath: ");
	char* imageFilePath;
	imageFilePath = (char*) malloc(256*sizeof(char));
	scanf("%s", imageFilePath);
	
	// TODO Add the stuff to the ThreadRacer file
	
	char inputString[2000];
  
	printf("\nEnter your implementation of the threadFunction algorithm ( press ` backtick and then enter to end input)\n");
	printf("\nPlease kindly add an extra %% symbol when using modulo to escape that character. Thank you!\n");
	printf("\nSAMPLE:");
	printf("\n");

	printf("\nint f_halfdim = info->filterDim/2;");

	printf("\nint* refRaster = (int*) info->refImage.raster;");
	printf("\nint* ansRaster = (int*) info->ansImage.raster;");

	printf("\nfor (int i = 0; i < info->rows; i++) {");
		printf("\n\tfor (int k = 0; k < info->cols; k++) {");
			printf("\n\t\tansRaster[i*info->cols + k] = refRaster[i*info->cols + k];");
		printf("\n\t}");
	printf("\n}");

    printf("\nfor (int row = f_halfdim; row < info->rows - f_halfdim; row++) {");
        printf("\n\tfor (int col = f_halfdim; col < info->cols - f_halfdim; col++) {");
            printf("\n\t\tfloat countR = 0.0f;");
			printf("\n\t\tfloat countG = 0.0f;");
			printf("\n\t\tfloat countB = 0.0f;");

			printf("\n\t\tint halfdimdim = f_halfdim*info->filterDim;");
			printf("\n\t\tint roww = row*info->cols;");
			printf("\n\t\tint ihalfdimdim;");
			printf("\n\t\tint rowiw;");
			printf("\n\t\tfor (int i = -f_halfdim; i <= f_halfdim; i++) {");
				printf("\n\t\t\tihalfdimdim = (i)*info->filterDim + halfdimdim;");
				printf("\n\t\t\trowiw = (i)*info->cols + roww;");
				printf("\n\t\t\tfor (int n = -f_halfdim; n <= f_halfdim; n++) {");
				    printf("\n\t\t\t\tcountR += info->filter[ihalfdimdim + (n+f_halfdim)] * (float) TIFFGetR(refRaster[rowiw + (col+n)]);");
				    printf("\n\t\t\t\tcountG += info->filter[ihalfdimdim + (n+f_halfdim)] * (float) TIFFGetG(refRaster[rowiw + (col+n)]);");
				    printf("\n\t\t\t\tcountB += info->filter[ihalfdimdim + (n+f_halfdim)] * (float) TIFFGetB(refRaster[rowiw + (col+n)]);");
				printf("\n\t\t\t}");
			printf("\n\t\t}");

			printf("\n\t\tif (countR > 255.0f)");
				printf("\n\t\t\tcountR = 255.0f;");
			printf("\n\t\telse if (countR < 0.0f)");
				printf("\n\t\t\tcountR = 0.0f;");

			printf("\n\t\tif (countG > 255.0f)");
				printf("\n\t\t\tcountG = 255.0f;");
			printf("\n\t\telse if (countG < 0.0f)");
				printf("\n\t\t\tcountG = 0.0f;");

			printf("\n\t\tif (countB > 255.0f)");
				printf("\n\t\t\tcountB = 255.0f;");
			printf("\n\t\telse if (countB < 0.0f)");
				printf("\n\t\t\tcountB = 0.0f;");

			printf("\n\t\tuint32_t out = 0;");
			printf("\n\t\tout = 255; ");
			printf("\n\t\tout <<= 8;");
			printf("\n\t\tout |= (int) countB;");
			printf("\n\t\tout <<= 8;");
			printf("\n\t\tout |= (int) countG;");
			printf("\n\t\tout <<= 8;");
			printf("\n\t\tout |= (int) countR;");
			printf("\n\t\tansRaster[row*info->cols + col] = out;");
			printf("\n\t\tinfo->progress++;");
        printf("\n\t}");
    printf("\n}");
	
	
	scanf("%[^`]s", inputString);
	
	char requiredEnding[200];
	strcpy(requiredEnding, "\ntime_value = get_posix_clock_time ();");

	strcat(requiredEnding, "\ntime_diff = time_value - prev_time_value;");
	strcat(requiredEnding, "\ninfo->execTime = (float) time_diff / 1000000.0f;");
	strcat(requiredEnding, "\ninfo->finished = 1;");
	strcat(requiredEnding, "\nwriteTGA(\"./userOutImg.tga\", &(info->ansImage));");
	
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
	sprintf(command, "sh racer.sh %d %s", diffNum, imageFilePath);
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
















