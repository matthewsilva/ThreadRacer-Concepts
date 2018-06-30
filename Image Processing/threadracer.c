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
#include "fileIO_TGA.h"
//
// TAKEN FROM tiff.h TIFF LIBRARY
#define TIFFGetR(abgr)   ((abgr) & 0xff)
#define TIFFGetG(abgr)   (((abgr) >> 8) & 0xff)
#define TIFFGetB(abgr)   (((abgr) >> 16) & 0xff)
#define TIFFGetA(abgr)   (((abgr) >> 24) & 0xff)

void* threadFunc(void* arg);

void* userThreadFunc(void* arg);


enum DifficultyLevel {Easy, Medium, Hard};

typedef struct ThreadInfo
{
	// Order in which thread was created
	int threadIndex;
	
	ImageStruct refImage;
	ImageStruct ansImage;
	float* filter;
	int filterDim;
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
	
	ImageStruct refImage;
	ImageStruct ansImage;
	float* filter;
	int filterDim;
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

//---------------------------------------
//	Image utility functions.
//	Should be moved to some Image.c file
//---------------------------------------
ImageStruct newImage(unsigned int width, unsigned int height, ImageType type,
					 unsigned int wordSizeRowPadding)
{
	if (width<3 || height<3)
	{
		printf("Image size should be at least 3x3\n");
		exit(14);
	}
	if (wordSizeRowPadding!=1 && wordSizeRowPadding!=4 &&
		wordSizeRowPadding!=8 && wordSizeRowPadding!=16 &&
		wordSizeRowPadding!=32 && wordSizeRowPadding!=64)
	{
		printf("wordSizeRowPadding must be one of: 1, 4, 8, 16, 32, or 64\n");
		exit(15);
	}

	ImageStruct img;
	img.nbCols = width;
	img.nbRows = height;
	img.type = type;
	switch (type)
	{
		case RGBA32_RASTER:
		img.bytesPerPixel = 4;
		break;
		
		case GRAY_RASTER:
		img.bytesPerPixel = 1;
		break;
		
		case FLOAT_RASTER:
		img.bytesPerPixel = sizeof(float);
		break;
	}

	img.bytesPerRow = ((img.bytesPerPixel * width + wordSizeRowPadding - 1)/wordSizeRowPadding)*wordSizeRowPadding;
	img.raster = (void*) malloc(height*img.bytesPerRow);

	switch (type)
	{
		case RGBA32_RASTER:
		case GRAY_RASTER:
		{
			unsigned char* r1D = (unsigned char*) img.raster;
			unsigned char** r2D = (unsigned char**) calloc(height, sizeof(unsigned char*));
			img.raster2D = (void*) r2D;
			for (unsigned int i=0; i<height; i++)
				r2D[i] = r1D + i*img.bytesPerRow;
		}
		break;
		
		case FLOAT_RASTER:
		{
			float* r1D = (float*) img.raster;
			float** r2D = (float**) calloc(height, sizeof(float*));
			img.raster2D = (void*) r2D;
			for (unsigned int i=0; i<height; i++)
				r2D[i] = r1D + i*img.bytesPerRow;
		}
		break;
	}

	return img;
}



int main(int argc, char** argv)
{
	int diffNum;
	int numRows;
	int numCols;
	char* imageFilePath;
	
	sscanf(argv[1], "%d", &diffNum);
	imageFilePath = argv[2];
	ImageStruct imageIn = readTGA(imageFilePath);
	numRows = imageIn.nbRows;
	numCols = imageIn.nbCols;
	
	ImageStruct threadImage;
	threadImage = newImage(imageIn.nbCols, imageIn.nbRows, RGBA32_RASTER, 1);
	ImageStruct userImage;
	userImage = newImage(imageIn.nbCols, imageIn.nbRows, RGBA32_RASTER, 1);
	
	int filterDim = 7;
	float* filter;
	filter = (float*) malloc(filterDim*filterDim*sizeof(float));
	for (int i = 0; i < filterDim; i++) {
		for (int k = 0; k < filterDim; k++) {
			filter[i*filterDim + k] = ((float) (i%5))*0.01f + ((float) (k%2))*0.02f;
		}
	}
	
	
	
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
	
	int timedout = 0;
	
	// TODO Set up timeoutThread
	
	pthread_barrier_t raceBarrier;
	pthread_barrier_init(&raceBarrier, NULL, 2);
	
	ThreadInfo params;
	UserThreadInfo userParams;
	
	params.refImage = userParams.refImage = imageIn;
	params.ansImage = threadImage;
	userParams.ansImage = userImage;
	params.filter = userParams.filter = filter;
	params.filterDim = userParams.filterDim = filterDim;
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
	
	
	int totalElements = (numRows-filterDim)*(numCols-filterDim);
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
	
	int* threadRaster = (int*) params.ansImage.raster;
	int* userRaster = (int*) userParams.ansImage.raster;
	
	int answerWrong = 0;
	int incorrectness = 0;
	int badPixels = 0;
/*	
	for (int i = filterDim/2; i < numRows-filterDim/2; i++) {
		for (int k = filterDim/2; k < numCols-filterDim/2; k++) {
			if (TIFFGetR(threadRaster[i*numCols + k]) != TIFFGetR(userRaster[i*numCols + k])) {
				answerWrong = 1;
				incorrectness = TIFFGetR(threadRaster[i*numCols + k]) - TIFFGetR(userRaster[i*numCols + k]);
				badPixels++;
			}
		}
	}
*/
	for (int i = 0; i < numRows; i++) {
		for (int k = 0; k < numCols; k++) {
			if (TIFFGetR(threadRaster[i*numCols + k]) != TIFFGetR(userRaster[i*numCols + k])) {
				answerWrong = 1;
				incorrectness = TIFFGetR(threadRaster[i*numCols + k]) - TIFFGetR(userRaster[i*numCols + k]);
				badPixels++;
			}
		}
	}
	
	if (answerWrong == 1) {
		printw("Your implementation was not correct! Try again.");
		printw("\n(%d bad pixels. Most recent off by %d)\n\n", badPixels, incorrectness);
		printw("\nuserParams.ansImage.nbRows = %d", userParams.ansImage.nbRows);
		printw("\nuserParams.ansImage.nbCols = %d", userParams.ansImage.nbCols);
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

	// Computing the dimensions of the filter
	int f_halfdim = info->filterDim/2;

	int* refRaster = (int*) info->refImage.raster;
	int* ansRaster = (int*) info->ansImage.raster;

	for (int i = 0; i < info->rows; i++) {
		for (int k = 0; k < info->cols; k++) {
			ansRaster[i*info->cols + k] = refRaster[i*info->cols + k];
		}
	}

    // Computes a new pixel for every valid pixel in the grid (valid = all but the edges)
    for (int row = f_halfdim; row < info->rows - f_halfdim; row++) {
        for (int col = f_halfdim; col < info->cols - f_halfdim; col++) {
            // Store computed pixels in the outRaster
            // Accumulators for RGB values
			float countR = 0.0f;
			float countG = 0.0f;
			float countB = 0.0f;

			// For every pixel in a 3x3 (or other filter dimensions) centered on the current pixel...
			for (int i = -f_halfdim; i <= f_halfdim; i++) {
				// Code motion / Common subexpressions
				for (int n = -f_halfdim; n <= f_halfdim; n++) {
				    // Accumulate the RGB values multiplied by the corresponding filter value
				    countR += info->filter[(i+f_halfdim)*info->filterDim + (n+f_halfdim)] * (float) TIFFGetR(refRaster[(row+i)*info->cols + (col+n)]);
				    countG += info->filter[(i+f_halfdim)*info->filterDim + (n+f_halfdim)] * (float) TIFFGetG(refRaster[(row+i)*info->cols + (col+n)]);
				    countB += info->filter[(i+f_halfdim)*info->filterDim + (n+f_halfdim)] * (float) TIFFGetB(refRaster[(row+i)*info->cols + (col+n)]);
				}
			}

			// Check to make sure we don't have any invalid RGB values
			if (countR > 255.0f)
				countR = 255.0f;
			else if (countR < 0.0f)
				countR = 0.0f;

			if (countG > 255.0f)
				countG = 255.0f;
			else if (countG < 0.0f)
				countG = 0.0f;

			if (countB > 255.0f)
				countB = 255.0f;
			else if (countB < 0.0f)
				countB = 0.0f;

			// Rebuild the pixel hex ARGB using shift and or operators on the accumulators
			uint32_t out = 0;
			out = 255; // A value is always 255
			out <<= 8;
			out |= (int) countB;
			out <<= 8;
			out |= (int) countG;
			out <<= 8;
			out |= (int) countR;
			ansRaster[row*info->cols + col] = out;
			info->progress++;
        }
    }
	
	/* Final time */
	time_value = get_posix_clock_time ();

	/* Time difference */
	time_diff = time_value - prev_time_value;
	info->execTime = (float) time_diff / 1000000.0f;
	
	info->finished = 1;
	
	writeTGA("./threadOutImg.tga", &(info->ansImage));
	
}

// Threads use this function to compute the part of the difference image they were assigned to compute
void* userThreadFunc(void* arg) {
	// Cast the void* parameter struct for actual use
	UserThreadInfo* info = ((UserThreadInfo*)arg);
	
	uint64_t prev_time_value, time_value;
	uint64_t time_diff;

	pthread_barrier_wait(info->raceBarrier);
	
	/* Initial time */
	prev_time_value = get_posix_clock_time ();

	
	
















