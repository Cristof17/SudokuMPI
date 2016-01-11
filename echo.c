#include <stdlib.h>
#include <mpi.h>
#include <string.h>
#include <stdio.h>


#define MATRIX_SIZE 100
#define LINE_SIZE 200
#define INITIATOR 0

#define SONDA_MESSAGE 10
#define ECHO_MESSAGE 20

#define TRUE 1 
#define FALSE 0
	int ** matrix; 
	int ** topology;		
	int size;
	int rank;
	int i , j;
	int read;
	int topoSize;
	int * routingVector;
	int * parentVector;
	int * emptyMessage;
	int * top_nou;
	char * line ;
	size_t len;

void initTopology(int ** topology , int size);
void parseInput(char * filename , char * mode , int ** outMatrix , int * size);
int * parseInputAsArray(char * topologyName, char * sudokuName , char * mode , int rank);
void createTopologyUsingMessages(int ** topology , int size , int rank , int * adiacenta);
int isEmptyMessage(int * receivedMessage, int size );
int getNumberOfNodes(char * filename , char * mode );
void combine (int * top_nou , int * adiacenta , int size );
void printMatrix(int ** matrix , int * size );
void printArray(int * array , int size);

int main(){
	int value; //DEBUG
	
	MPI_Init(NULL , NULL);
	MPI_Comm_size(MPI_COMM_WORLD , & size);
	MPI_Comm_rank(MPI_COMM_WORLD , & rank);
	
	/*Init */
		
	topoSize = getNumberOfNodes("sudoku.txt", "r+");
	top_nou = parseInputAsArray("echoInput.txt" , "sudoku.txt" , "r+", rank);
	if(top_nou == NULL){
		top_nou = (int *) calloc (topoSize , sizeof(int));
	}
	topology = (int **) malloc (topoSize * sizeof(int *));
	initTopology(topology , topoSize);
	printArray(top_nou , topoSize);
	emptyMessage = (int *) calloc (topoSize , sizeof(int));
	// printMatrix(topology, &topoSize);
		
	if(rank == 0){
		//pass a valid pointer to parseInput()
		// printf("Rank = %d \n" , rank);
		// matrix = (int **)malloc(MATRIX_SIZE * sizeof(int*));
		// parseInput("echoInput.txt" , "r+" , matrix, &topoSize);
		// routingVector = (int *) calloc(topoSize  ,sizeof(int));
		// parentVector = (int *) calloc (topoSize , sizeof(int));
		// printMatrix(matrix , &topoSize);
		// printMatrix(topology , &topoSize);
		
				
	}else{
			
		// printf("Rank = %d \n" , rank);
		// printMatrix(matrix , &topoSize);
	
	}
	
	createTopologyUsingMessages(topology , topoSize , rank , top_nou);
	
	MPI_Finalize();
	
	// if(rank == 0 ){
	// 	printMatrix(topology , & topoSize);
	// }
	

	return 0;
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

void createTopologyUsingMessages(int ** topology , int size , int rank , int * adiacenta){
		
	// printArray(top_nou , size);
	int * top_nou = (int *) calloc (size , sizeof(int));
	
	if(rank == 0){ //INITIATOR
		for(i = 0 ; i < size ; ++i){
			if(adiacenta[i] == 1){
				MPI_Send(emptyMessage , size , MPI_INT , i , SONDA_MESSAGE , MPI_COMM_WORLD);
				printf("%d sending sonda to %d :" , rank , i);
				printArray(emptyMessage , size );
			}
		}
	}
	
	else{
		
		int parent;
		MPI_Status status;
		MPI_Recv(top_nou , size , MPI_INT , MPI_ANY_SOURCE , SONDA_MESSAGE , MPI_COMM_WORLD , &status);
		printf("%d received sonda first from %d :", rank , parent );
		printArray(top_nou , size);
		parent = status.MPI_SOURCE;
		
		for(i = 0 ; i < size ; ++i){
			if(adiacenta[i] == 1 && i != parent){
				printf ("Rank %d has neightbor %d \n" , rank , i);
				MPI_Send(emptyMessage , size , MPI_INT , i , SONDA_MESSAGE , MPI_COMM_WORLD);
				printf("%d sending sonda to %d :" , rank ,i);
				printArray(emptyMessage , size);
			}
		}
		
		//get the number of echos I need to receive
		int numberOfEcho = 0;
		for(i = 0 ; i < size ; ++i){
			if(adiacenta[i] == 1 && i != parent){
				numberOfEcho ++;
			}
		}
		
		while(numberOfEcho > 0){
			MPI_Recv(top_nou , size , MPI_INT , MPI_ANY_SOURCE , MPI_ANY_TAG , MPI_COMM_WORLD , &status);
			int source = status.MPI_SOURCE;
			if(status.MPI_TAG == ECHO_MESSAGE){
				if(top_nou != NULL){
					topology[source] = top_nou;
					numberOfEcho --;
					combine (top_nou , adiacenta, size);
					printf("%d received echo from %d :", rank , source);
					printArray(top_nou, size);
				}
			}
			else if(status.MPI_TAG == SONDA_MESSAGE){
				MPI_Send(emptyMessage , size , MPI_INT , source , ECHO_MESSAGE , MPI_COMM_WORLD);
				printf("%d sending empty echo to %d :" , rank , source);
				printArray(emptyMessage , size);
			}
		}
		
		//send echo to parent
		MPI_Send(adiacenta , size , MPI_INT , parent , ECHO_MESSAGE , MPI_COMM_WORLD);
		printf("%d sending echo message to %d :" , rank , parent);
		printArray(adiacenta, size);
	}
	
	topology[rank] = top_nou;
}

int isEmptyMessage(int * receivedMessage , int size){
	int i;
	for(i = 0 ; i < size ; ++i){
		if(receivedMessage[i] != 0)
			return FALSE;
	}
	return TRUE;
}

void printArray(int * array , int size){
	int i = 0;
	for(i = 0 ; i < size ; ++i){
		printf("%d ", array[i]);
	}
	printf("\n");
}


int * parseInputAsArray(char * topologyName, char * sudokuName , char * mode , int grad){
	int i = 0 ;
	int size;
	int currentPosition =0 ;
	int * outArray; 
	 
	FILE * sudokuFile = fopen(sudokuName , mode);
	fscanf(sudokuFile , "%d" , &size);
	fclose(sudokuFile);
	
	
	outArray = (int *) calloc (size , sizeof(int));
	FILE * topologyFile = fopen(topologyName , mode );

	/* Read from file */
	while ((read = getline(&line, &len, topologyFile)) != -1) {
						
		int parinte;
		int copil;
		char * tok = strtok(line , " -");
		
		sscanf(tok, "%d" , &parinte);
		//get adjancency nodes
		if(parinte == grad){
			// printf("Rank %d reading " , rank);
			// printf("%s\n" , line); //DEBUG
			while(tok != NULL){
				tok = strtok(NULL , " -");
				if(tok != NULL){
					sscanf(tok, "%d" , &copil);
					// printf("New tok is %s \n" , tok);
					outArray[copil] = 1;
				}
			}
		}				
	}	
	fclose(topologyFile);
	return outArray;
}

int getNumberOfNodes(char * filename , char * mode ){
	int size ;
	
	FILE * sudokuFile = fopen(filename, mode);
	fscanf(sudokuFile , "%d" , &size);
	fclose(sudokuFile);
	return size * size ;
	
}

void parseInput(char * filename , char * mode , int ** outMatrix , int * size){
	
	int maxNod;//sa stiu care este cel mai mare ID al unui nod
	/* Init global variables */

	for(i = 0 ; i < MATRIX_SIZE ; ++i){
		outMatrix[i] = (int *) malloc(MATRIX_SIZE * sizeof(int));
	}
	
	
	for(i = 0 ; i < MATRIX_SIZE ; ++i){
		for(j = 0 ; j < MATRIX_SIZE ; ++j){
			outMatrix[i][j] = 0;
		}
	line = (char *) malloc (LINE_SIZE * sizeof(char));
	}

	/* Open the file */
	FILE * inFile = fopen(filename , mode);
	if(inFile == NULL){
		printf("Cannot open input file");
		MPI_Finalize();
		exit(0);
	}
	
	
	/* Read from file */
	while ((read = getline(&line, &len, inFile)) != -1) {
		
		printf("%s\n" , line); //DEBUG
				
		int parinte;
		int copil;
		char * tok = strtok(line , " -");
		
		sscanf(tok, "%d" , &parinte);
		//get adjancency nodes
		if(parinte > maxNod){
			maxNod = parinte;
		}
			
		while(tok != NULL){
			// printf("New tok is %s \n" , tok);
			tok = strtok(NULL , " -");
			if(tok != NULL){
				sscanf(tok, "%d" , &copil);
				 *(outMatrix[parinte] + copil) = 1;
				 *(outMatrix[copil] + parinte) = 1;
				 if(copil > maxNod){
					 maxNod = copil;
				 } 
			}
		}
		
		
		*size = maxNod + 1; //last parent is the last node 
		//which has the biggest ID (give that nodes are represented
		//as integers				
	}
}

void combine (int * top_nou , int * adiacenta , int size ){
	int i;
	for(i = 0 ; i < size ; ++i){
		adiacenta[i] |= top_nou[i];
	}
}