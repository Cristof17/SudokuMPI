#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <string.h>


#define MATRIX_SIZE 100
#define LINE_SIZE 200

	int ** matrix; 
	int ** topology;		
	int size;
	int rank;
	int i , j;
	int read;
	int topoSize;
	int * routingVector;
	int * parentVector;
	char * line ;
	size_t len;

void initTopology(int ** topology , int size);
void parseInput(char * filename , char * mode , int ** outMatrix , int * size);
void printMatrix(int ** matrix , int * size );

int main(){
	
	
	MPI_Init(NULL , NULL);
	MPI_Comm_size(MPI_COMM_WORLD , & size);
	MPI_Comm_rank(MPI_COMM_WORLD , & rank);
	
	/*Init */
	
	if(rank == 0){
		//pass a valid pointer to parseInput()
		matrix = (int **)malloc(MATRIX_SIZE * sizeof(int*));
		parseInput("echoInput.txt" , "r+" , matrix, &topoSize);
		topology = (int **) malloc (topoSize * sizeof(int *));
		initTopology(topology , topoSize);
		routingVector = (int *) calloc(topoSize  ,sizeof(int));
		parentVector = (int *) calloc (topoSize , sizeof(int));
		printMatrix(matrix , &topoSize);
		printMatrix(topology , &topoSize);		
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
	
	printf("\n");
}

void initTopology(int ** topology , int size){
	int i  =0 ;
	int j  =0;
	for(i = 0 ; i < size ; ++i){
		topology[i] = (int *) malloc (size * sizeof(int));
	}
	
	for(i = 0; i < size ; ++i){
		for(j = 0 ; j < size ; ++j){
			topology[i][j] = -1;
		}
	}

}