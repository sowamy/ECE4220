/*	FILE: 4220_Lab2Week1_AngelinoLefevers.c
 *	DESCRIPTION:
 *		- This program requires 2 command line arguments:
 *			-- File Name
 *			-- Value to be searched for
 *		- This program explores the changes in efficiencies using multithreading by comparing
 *			performance differences in different methedologies of implementing a searching algorithm.
 *		- Searches for <value> given <filename>
 *		- For this program to operate effectively, the data must be stored in the folder ./Data, 
 *			which should be stored where the executable is being ran.
 *		- This program was ran on a raspberry pi3 development module; however, the software should
 *			be ran effectively on all linux machines.
 *		- Uses a variety of number of processes to complete the task and compares the efficiency in 
 *			the number of threads used
 *	COMPILE: gcc ECE4220_Lab2Week1_AngelinoLefevers.c -o L2W1 -lpthread
 *
 *	AUTHOR: Angelino Lefevers
 *	CLASS: ECE 4220 - Real-Time Operating Systems
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define NUM_TEST_CASES 10

/*	NAME: data
 *	DESCRIPTION: Stores the characteristics and the data of the desired file
 *	VARIABLES:
 *		- num_rows		:= Number of rows in the data
 *		- num_columns 	:= Number of columns in the data
 *		- rawData 		:= Pointer to start of array storing all of the data, organized via rows
 *		- columnsData 	:= Pointer to start of array storing all of the data, is organized via columns instead of rows
 *		- rowIndices	:= Array of pointers describing the address of each row in <rawData>
 *		- columnIndices	:= Array of pointers describing the address of each column in <columnsData>
 */
struct data{
	int num_columns;
	int num_rows;
	int* rawData;
	int* columnsData;
	int** rowIndices;
	int** columnIndices;
}; // END STRUCTURE: data

/*	NAME: searchStruct
 *	DESCRIPTION: Structure which stores all of the searching criteria and data needed in the search operation
 *	VARIABLES:
 *		- data 		:= The raw data array
 *		- size 		:= Number of elements in the data array
 *		- key		:= Value to search for
 *		- freq 		:= Number of times the value was found in the array
 */
struct searchStruct {
	int* data;
	int size;
	int key;
	int freq;
}; // END STRUCTURE: searchStruct

typedef struct data rawData;
typedef rawData* rawDataPtr;

typedef struct searchStruct ss;
typedef ss* ssPtr;

rawDataPtr loadData(rawDataPtr coreData, char filename[256]);
void printData(rawData coreData);
void printDataInverse(rawData coreData);
int search(int* data, int size, int key);	// For use in the single process
void* search_t(void * x);					// For use in multithreaded operations

int main( int argc, char *argv[] ) {
	char filename[256] = "./Data/";
	
	// Counters
	int i = 0;
	int j = 0;
	int k = 0;

	// Value searching for
	int value;

	int freq = 0;
	float totalTime;
	float avgTime;
	struct timeval timeBefore;
	struct timeval timeAfter;
	rawData base;
	rawDataPtr basePtr = &base;
	ss args;
	ssPtr argsPtr = malloc(sizeof(args));
	pthread_t* core;

	// Invalid number of command line arguments
	if( argc != 3 ) { 
		printf( "There must be 2 arguments along with the executable for this program to work\n" );
		printf( "There must first be the data file name to be accessed.\n" );
		printf( "The second is the value to be searched for.\n" );
		printf( "EXAMPLE: ./Lab2 2x100.txt 5\n" );
		return 0;
	} 	
	
	// Prepares the filename and location
	strcat( filename, argv[1] );

	// Value to search for
	value = atoi( argv[2] );

	// TEST: Prints filename and value
	printf( "FILENAME: %s\n", filename );
	printf( "DESIRED VALUE: %d\n", value );

	loadData(basePtr, filename);

	printf( "ROWS: %d\n", base.num_rows );
	printf( "COLUMNS: %d\n", base.num_columns );

	// printData(base);
	// printf("\n");
	// printDataInverse(base);

	// --------------SINGLE THREAD--------------
	// Uses one thread/process to search every element in the file value by value
	// Runs process <NUM_TEST_CASES> times and finds the average time it takes for the process to complete the task

	printf("--------------SINGLE THREAD--------------\n");

	for(i = 0; i < NUM_TEST_CASES; i++) {
		// Gets initial time stamp
		gettimeofday (&timeBefore, NULL);

		freq = search(base.rawData, (base.num_rows*base.num_columns), value);

		// Gets final time stamp
		gettimeofday (&timeAfter, NULL);

		// Calculates execution time
		totalTime += (float)(timeAfter.tv_usec - timeBefore.tv_usec);

		// TEST: Print each time case
		// printf("Single Process Run #%d: %f\n", i, (float)(timeAfter.tv_usec - timeBefore.tv_usec));
	} // END FOR : Runs process <NUM_TEST_CASES> times

	avgTime = totalTime / 1000;

	printf("AVERAGE TIME: %.2f MICROSECONDS\n",(float)avgTime);
	printf("FREQUENCY: %d\n", freq);

	// --------------THREAD FOR EVERY ROW--------------
	
	printf("----------THREAD FOR EVERY ROW-----------\n");

	// Base of multithreading operations
	core = (pthread_t*)malloc(base.num_rows*sizeof(pthread_t*));
	pthread_t* c2 = core;

	// Initialize argument structure to pass to threading operations
	args.size = base.num_columns;
	args.key = value;

	for(i=0; i < NUM_TEST_CASES; i++) {
		// Pointers to data to be searched
		int** dataToBeSearched = base.rowIndices;

		int totalFreq = 0;

		// Gets initial time stamp
		gettimeofday (&timeBefore, NULL);

		for(j=0; j < base.num_rows; j++) {
			args.data = *dataToBeSearched++;
			pthread_create(&c2[j], NULL, search_t, (void*) argsPtr);
			totalFreq += args.freq;
		} // END FOR: Create a thread for each row, adds up all the frequencies of key from each row

		for(k=0; k < base.num_rows; k++) {
			pthread_join(c2[k], NULL);
		} // END FOR: Joins each thread

		// Gets final time stamp
		gettimeofday (&timeAfter, NULL);

		// Calculates execution time
		totalTime += (float)(timeAfter.tv_usec - timeBefore.tv_usec);

	} // END FOR: Runs process 1000 times	

	avgTime = totalTime / 1000;

	printf("AVERAGE TIME: %.2f MICROSECONDS\n",(float)avgTime);
	printf("FREQUENCY: %d\n", freq);

	// --------------THREAD FOR EVERY VALUE------------
	
	printf("----------THREAD FOR EVERY VALUE---------\n");

	// Base of multithreading operations
	core = (pthread_t*)malloc(base.num_rows*base.num_columns*sizeof(pthread_t*));
	pthread_t* c = core;

	// Initialize argument structure to pass to threading operations
	args.size = 1;
	args.key = value;

	for(i=0; i < NUM_TEST_CASES; i++) {
		// Pointer to data to be searched
		int* dataToBeSearched = base.rawData;

		int totalFreq = 0;

		// Gets initial time stamp
		gettimeofday (&timeBefore, NULL);

		for(j=0; j < base.num_rows*base.num_columns; j++) {
			args.data = dataToBeSearched++;
			pthread_create(&c[j], NULL, search_t, (void*) argsPtr);
			totalFreq += args.freq;
		} // END FOR: Create a thread for each row, adds up all the frequencies of key from each row

		for(k=0; k < base.num_rows*base.num_columns; k++) {
			pthread_join(c[k], NULL);
		} // END FOR: Joins each thread

		// Gets final time stamp
		gettimeofday (&timeAfter, NULL);

		// Calculates execution time
		totalTime += (float)(timeAfter.tv_usec - timeBefore.tv_usec);

	} // END FOR: Runs process 1000 times	

	avgTime = totalTime / 1000;

	printf("AVERAGE TIME: %.2f MICROSECONDS\n",(float)avgTime);
	printf("FREQUENCY: %d\n", freq);

	return 0;
} // END FUNCTION main

/*	NAME: loadData
 *	DESCRIPTION: This function loads the data from the file element by element and stores it 
 *					in the data function.
 */
rawDataPtr loadData(rawDataPtr coreData, char filename[256])
{
	// Iterators
	int i = 0;
	int j = 0;
	int k = 0;
	FILE *fp;

	fp = fopen( filename, "r" );
		
	if( fp == NULL ) {
		printf( "FILE ERROR\n" );
		return 0;
	}

	fscanf( fp, "%d %d\n", &coreData->num_rows, &coreData->num_columns );
	
	coreData->rawData = malloc((coreData->num_rows*coreData->num_columns)*sizeof(int));
	int* dataPtr = coreData->rawData;	// Used to increment the storage addresses as data is being stored to the array
	coreData->columnsData = malloc((coreData->num_rows*coreData->num_columns)*sizeof(int));
	int* cdPtr = coreData->columnsData;	// Column Data Poiner - Used to store data in columns instead of rows
	coreData->rowIndices = malloc(coreData->num_rows*sizeof(int*));
	int** rowPtr = coreData->rowIndices;	// Used to increment the storage addresses for the addresses of rows
	coreData->columnIndices = malloc(coreData->num_columns*sizeof(int*));
	int** columnPtr = coreData->columnIndices;

	// Gets data and arrange into rows
	for(j=0; j < coreData->num_rows; j++) {
		for(i = 0; i < coreData->num_columns; i++) {
			fscanf(fp, "%d", dataPtr);
			if(i == 0) {
				*rowPtr = dataPtr;
				rowPtr++;
			} // END IF: First column in each row
			dataPtr++;
		} // END FOR : Iterate through each row column by column
		fscanf(fp, "\n");
	} // END FOR : Iterate through file row by row

	dataPtr = coreData->rawData;

	// Arranges data into columns (Using Offsets)
	for(i=0; i < coreData->num_columns; i++) {
		cdPtr = coreData->columnsData + i*sizeof(int);	// Initial Offset
		for(j=0; j < coreData->num_rows; j++) {
			*cdPtr = *dataPtr;	
			cdPtr += coreData->num_columns*sizeof(int);	// Sequential Offset
			dataPtr++;
		} // END FOR: Iterate through data row by row
	} // END FOR: Iterate through data column by column

	fclose( fp );

} // END FUNCTION loadData

/*	NAME: printData
 *	DESCRIPTION: This function prints every element of the data
 *	NOTE: Used primarily for testing purposes, will not be used in final version of the program
 */
void printData(rawData coreData)
{
	int i = 0;
	int j = 0;

	int *dataInc = coreData.rawData;

	for(i; i < coreData.num_rows; i++) {
		for(j = 0; j < coreData.num_columns; j++) {
			printf("%d\t", *dataInc++);
		} // END FOR
		printf("\n");
	} // END FOR
} // END FUNCTION printData

void printDataInverse(rawData coreData)
{
	int i = 0;
	int j = 0;

	int* cd = coreData.columnsData;

	for(i; i < coreData.num_columns; i++) {
		for(j=0; j < coreData.num_rows; j++) {
			printf("%d\t", *cd++);
		} // END FOR : Iterate row by row
		printf("\n");
	} // END FOR : Iterate column by column
} // END FUNCTION printData

/* 	NAME: search
 *	DESCRIPTION: Goes through <data> of size <size> element by element, incrementing <frequency> each time <key> is found
 *					within the data
 */
int search(int* data, int size, int key)
{
	int i = 0;
	int frequency = 0;
	int *dataInc = data;

	for(i = 0; i < size; i++) {
		if(key == *dataInc) { frequency++; }
		dataInc++;
	} // END FOR

	return frequency;
} // END FUNCTION search

/* 	NAME: search
 *	DESCRIPTION: Goes through <data> of size <size> element by element, incrementing <frequency> each time <key> is found
 *					within the data. Used for multithreaded operations.
 */
void* search_t(void* x)
{
	int i = 0;
	int frequency = 0;
	ss* data = (ss*)x;
	int *dataInc = data->data;

	for(i = 0; i < data->size; i++) {
		if(data->key == *dataInc) { frequency++; }
		dataInc++;
	} // END FOR

	data->freq = frequency;
	return NULL;
}
