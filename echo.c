#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>


#define MATRIX_SIZE 3

int main(){
	
	int ** matrix; 	
	
	int size;
	int rank;
	int i ;
	int read;
	char * line ;
	
	size_t len;
	MPI_Init(NULL , NULL);
	MPI_Comm_size(MPI_COMM_WORLD , & size);
	MPI_Comm_rank(MPI_COMM_WORLD , & rank);
	
	/*Init */
	
	if(rank == 0){
		matrix = (int ** ) malloc (MATRIX_SIZE * sizeof(int*));
	
		for(i = 0 ; i < MATRIX_SIZE ; ++i){
			matrix[i] = (int *) malloc(MATRIX_SIZE * sizeof(int));
		}
		
		line = (char *) malloc (200 * sizeof(char));
	}
	
	
	
	/* Open the file */
	FILE * inFile = fopen("echoInput.txt" , "r+");
	if(inFile == NULL){
		printf("Cannot open input file");
		return -1 ;
	}
	
	/* Read from file */
	while ((read = getline(&line, &len, inFile)) != -1) {
		printf("%s" , line);
	}
	
	
	MPI_Finalize();
	
	return 0;
}