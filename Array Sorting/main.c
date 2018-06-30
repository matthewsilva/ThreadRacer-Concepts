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
	scanf("%d", &diffNum);
	//diffNum = 1;
	
	/*
	printf("\nPlease select the picture for the game:");
	printf("\n\tPicture filepath: ");
	char* imageFilePath;
	imageFilePath = (char*) malloc(256*sizeof(char));
	scanf("%s", imageFilePath);
	*/
	// TODO Add the stuff to the ThreadRacer file
	
	char inputString[2000];
  
	printf("\nEnter your implementation of the threadFunction algorithm ( press ` backtick and then enter to end input)\n");
	printf("\nPlease kindly add an extra %% symbol when using modulo to escape that character. Thank you!\n");
	printf("\nSTARTER:");
	printf("\n");
	printf("\nvoid userSort(int* arr, int arrSize) {");

	
	scanf("%[^`]s", inputString);
	
	
	char buffer[2000];
	FILE *racerFile;
	system("cp threadracer.c userThreadRacer.c");
	racerFile = fopen("userThreadRacer.c", "a");
	fprintf(racerFile, inputString);

	fclose(racerFile);
	
	system("chmod +x racer.sh");
	
	char* command = (char*) malloc(256*sizeof(char));
	sprintf(command, "sh racer.sh %d", diffNum);
	system(command);
	
	return 0;
}













