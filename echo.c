#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <string.h>


#define MATRIX_SIZE 100
#define LINE_SIZE 200

	int ** matrix; 		
	int size;
	int rank;
	int i , j;
	int read;
	int topoSize;
	char * line ;
	size_t len;

void initMatrix(int ** pointer , int length , int sizeOfElement);
void parseInput(char * filename , char * mode , int ** outMatrix , int * size);
void printMatrix(int ** matrix , int * size );

int main(){
	
	
	MPI_Init(NULL , NULL);
	MPI_Comm_size(MPI_COMM_WORLD , & size);
	MPI_Comm_rank(MPI_COMM_WORLD , & rank);
	
	/*Init */
	
	if(rank == 0){
		matrix = (int **)malloc(MATRIX_SIZE * sizeof(int*));
		parseInput("echoInput.txt" , "r+" , matrix, &topoSize);
		printMatrix(matrix , &topoSize);		
	}
	
	MPI_Finalize();
	return 0;
}

void parseInput(char * filename , char * mode , int ** outMatrix , int * size){
	
	/* Init global variables */

	for(i = 0 ; i < MATRIX_SIZE ; ++i){
		outMatrix[i] = (int *) malloc(MATRIX_SIZE * sizeof(int));
	}
	
	
	for(i = 0 ; i < MATRIX_SIZE ; ++i){
		for(j = 0 ; j < MATRIX_SIZE ; ++j){
			outMatrix[i][j] = 0;
		}
	}
	line = (char *) malloc (LINE_SIZE * sizeof(char));

	/* Open the file */
	FILE * inFile = fopen(filename , mode);
	if(inFile == NULL){
		printf("Cannot open input file");
		MPI_Finalize();
		exit(0);
	}
	
	
	/* Read from file */
	while ((read = getline(&line, &len, inFile)) != -1) {
		printf("%s" , line); //DEBUG
				
		int parinte;
		int copil;
		char * tok = strtok(line , " -");
		
		sscanf(tok, "%d" , &parinte);
		//get adjancency nodes
			
		while(tok != NULL){
			// printf("New tok is %s \n" , tok);
			tok = strtok(NULL , " -");
			if(tok != NULL){
				sscanf(tok, "%d" , &copil);
				 *(outMatrix[parinte] + copil) = 1; 
			}
		}
		
		*size = parinte + 1; //last parent is the last node 
		//which has the biggest ID (give that nodes are represented
		//as integers				
	}
}

void printMatrix(int ** matrix , int * size ){
	
	int i = 0 ;
	int j = 0 ;
	for (i = 0 ; i < *size ; ++i){
		for(j = 0 ; j < *size ; ++j){
			printf("%d " , matrix [i][j]);
		}
		printf("\n");
	}
}