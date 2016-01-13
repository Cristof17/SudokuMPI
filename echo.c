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

#define SEND 1000
#define RECEIVE 1001

	int ** matrix; 		
	int size;
	int rank;
	int i , j;
	int read;
	int topoSize;
	int * top_nou;
	char * line ;
	size_t len;

void initTopology(int ** topology , int size ,int value);
int * parseInputAsArray(char * topologyName, char * sudokuName , char * mode , int rank);
int ** createTopologyUsingMessages(int size , int rank , int * adiacenta , int topology[size][size] , int emptyMatrix[size][size]);
int isEmptyMessage(int * receivedMessage, int size );
int isEmptyMatrix(int size, int matrix[size][size]);
int getNumberOfNodes(char * filename , char * mode );
void combine (int * top_nou , int * adiacenta , int size ,int rank);
void combineMatrixAdiacenta(int size ,int rank , int matrix[size][size] , int adiacenta[size]);
void logicalORMatrix(int size, int from[size][size], int to[size][size]);
void sendMatrixToAll(int size , int matrix[size][size]);
void createRoutingVector(int size , int rank , int matrix[size][size], int * vector);
void printMatrix(int size , int matrix[size][size]);
void printArray(int size , int array[size]);
void printMessage(int source , int destination , int * array , int size , int messageTYPE , int direction);
void printMessageMatrix(int a , int b , int size , int messageType, int direction, int matrix[size][size]);

int main(){
	int value; //DEBUG
	int initialized = FALSE ;
	
	MPI_Init(NULL , NULL);
	MPI_Comm_size(MPI_COMM_WORLD , & size);
	MPI_Comm_rank(MPI_COMM_WORLD , & rank);
	
	//number of nodes
	topoSize = getNumberOfNodes("sudoku.txt", "r+");
	int emptyMatrix[topoSize][topoSize];
	int topology[topoSize][topoSize];
	int routingVector[topoSize];
	
	for(i = 0 ; i < size ; ++i){
		routingVector[i] = -1;
	}
	
	for(i = 0 ; i < size ; ++i){
		for(j = 0 ; j < size ; ++j){
			emptyMatrix[i][j] = 0;
			topology[i][j] = 0;
		}
	}
	
		
	//arrays
	top_nou = parseInputAsArray("echoInput.txt" , "sudoku.txt" , "r+", rank);
	
	matrix = createTopologyUsingMessages(topoSize , rank , top_nou , topology , emptyMatrix);
	if(rank == 0)
		for(i = 0 ; i < size ; ++i){
			for(j = 0 ; j < size ; ++j){
				printf("%d " , matrix[i][j]);
			}
			printf("\n");
		}
	
	MPI_Barrier(MPI_COMM_WORLD);
	
	//all nodes have the topology matrix after this line
	MPI_Bcast(&topology , topoSize * topoSize , MPI_INT , 0 , MPI_COMM_WORLD);
	
	// if(rank == 1 ) printMatrix(size , topology);
	// createRoutingVector(topoSize , rank , topology, &routingVector[0]);
	// printf("Rank %d has routing vector :", rank);
	// printArray(topoSize , routingVector);
	// printf("\n");
	
	MPI_Finalize();
	return 0;
}

void printMatrix(int size , int matrix[size][size]){
	
	int i = 0 ;
	int j = 0 ;
	for (i = 0 ; i < size ; ++i){
		for(j = 0 ; j < size ; ++j){
			printf("%d " , matrix [i][j]);
		}
		printf("\n");
	}
	printf("\n");
}

void initTopology(int ** topology , int size , int value){
	int i  =0 ;
	int j  =0 ;
	
	for(i = 0 ; i < size ; ++i){
		topology[i] = (int *) calloc (size , sizeof(int));
	}
	
	for(i = 0; i < size ; ++i){
		for(j = 0 ; j < size ; ++j){
			topology[i][j] = value;
		}
	}

}

int ** createTopologyUsingMessages(int size , int rank , int * adiacenta , int topology[size][size] , int emptyMatrix[size][size]){
		
	// printArray(top_nou , size);
	// int * top_nou = (int *) calloc (size , sizeof(int ));
	int top_nou[size][size];
	for(i = 0 ; i < size ; ++i){
		for(j = 0 ; j < size ; ++j){
			top_nou[i][j] = 0;
		}
	}
	
	int ** topology_pointer = (int **) calloc (size , sizeof(int *));
	for(i = 0 ; i < size ; ++i){
		topology_pointer[i] = (int *) calloc (size , sizeof(int ));
	}
	combineMatrixAdiacenta(size ,rank , topology , adiacenta );
	
	// printf("Rank %d has topology \n" ,rank);
	// printMatrix(size , topology);
	
	
	// printArray(adiacenta , size );
	int parent;
	MPI_Status status;	
	
	if(rank == 0){ //INITIATOR
		for(i = 0 ; i < size ; ++i){
			if(adiacenta[i] == 1){
				MPI_Send(emptyMatrix , size * size , MPI_INT , i , SONDA_MESSAGE , MPI_COMM_WORLD);
				printMessageMatrix(rank , i , size , SONDA_MESSAGE , SEND , emptyMatrix);
			}
		}
	}
	
	else{
		
		//receive sonda
		MPI_Recv(top_nou , size * size , MPI_INT , MPI_ANY_SOURCE , SONDA_MESSAGE , MPI_COMM_WORLD , &status);
		parent = status.MPI_SOURCE;
		printMessageMatrix(rank , parent , size , SONDA_MESSAGE , RECEIVE , top_nou );
		
			
		//send sonde to neighbors
		for(i = 0 ; i < size ; ++i){
			if(adiacenta[i] == 1 && i != parent){
				MPI_Send(emptyMatrix , size * size , MPI_INT , i , SONDA_MESSAGE , MPI_COMM_WORLD);
				printMessageMatrix(rank , i  , size , SONDA_MESSAGE , SEND , top_nou);
			}
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
			MPI_Recv(top_nou , size * size , MPI_INT , MPI_ANY_SOURCE , MPI_ANY_TAG , MPI_COMM_WORLD , &status);
			int source = status.MPI_SOURCE;
			
			//received echo
			printMessageMatrix(rank , source  , size , ECHO_MESSAGE , RECEIVE , top_nou);
			
			if(status.MPI_TAG == ECHO_MESSAGE){
				if(top_nou != NULL){
					int empty = isEmptyMatrix(size , top_nou);
						if(!empty){
							logicalORMatrix(size , top_nou ,topology);
								// printf("Creating logical OR from %d to %d \n" , source , rank);
								// printMatrix(size ,topology);
						}
					numberOfEcho --;
					// combine (top_nou , adiacenta, size ,rank);
				}
			}
			//received sonda when listening for echo
			else if(status.MPI_TAG == SONDA_MESSAGE){
				MPI_Send(emptyMatrix , size * size , MPI_INT , source , ECHO_MESSAGE , MPI_COMM_WORLD);
				printMessageMatrix(rank , source  , size , ECHO_MESSAGE , SEND , emptyMatrix);
				//delete connection
				if(topology[rank][source] == 1){
					printf("%d taie legatura cu %d source \n" , rank, source);
					topology[rank][source] =0;
				}
			}
			
		}

		//send echo to parent
		if(rank != 0){
			MPI_Send(topology , size * size , MPI_INT , parent , ECHO_MESSAGE , MPI_COMM_WORLD);
			printMessageMatrix(rank , parent , size , ECHO_MESSAGE , SEND , topology );
	
			printf("Rank %d a terminat de transmis \n" , rank);
			
		}else{
						
			for(i = 0 ; i < size ; ++i){
				for(j = 0 ; j < size ; ++j){
					topology_pointer[i][j] = topology[i][j];
				}
			}
			
			return topology_pointer;
		}
	
}

int isEmptyMessage(int * receivedMessage , int size){
	int i;

	for(i = 0 ; i < size ; ++i){
		if(receivedMessage[i] != 0)
			return FALSE;
			
		return TRUE;
	}
}

void printArray(int size , int array[size]){
	int i = 0;
	for(i = 0 ; i < size ; ++i){
		printf("%d ", array[i]);
	}
}


int * parseInputAsArray(char * topologyName, char * sudokuName , char * mode , int grad){
	int i = 0 ;
	int size;
	int currentPosition =0 ;
	int * outArray; 
	 
	FILE * sudokuFile = fopen(sudokuName , mode);
	fscanf(sudokuFile , "%d" , &size);
	fclose(sudokuFile);
	
	
	outArray = (int *) calloc (size * size , sizeof(int));
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

void combine (int * top_nou , int * adiacenta , int size ,int rank){
	int i;
	for(i = 0 ; i < size ; ++i){
		adiacenta[i] |= top_nou[i];
	}
	
	adiacenta[rank] = 0;
}

void copy(int * from , int * to , int size){
	int i ;
	for (i = 0; i < size ; ++i){
		to[i] = from[i];
	}
}

void logicalOR(int * from , int * to , int size){
	int i ;
	
	for (i = 0 ; i < size ; ++i){
		to[i] |= from[i];
	}
}

void printMessage(int a , int b , int * array , int size , int messageTYPE , int direction){
	int i ;
	if(direction == SEND)
		printf("%d trimite ",a);
	else 
		printf("%d primeste ",a);
	printArray(size, array);
	if(direction == SEND)
		printf(" catre %d ", b);
	else
		printf(" de la %d ", b);
		
	if(messageTYPE == ECHO_MESSAGE)
		printf("echo");
	else
		printf("sonda");
	printf("\n");		
}

int isEmptyMatrix(int size, int matrix[size][size]){
	int i ;
	int j ;
	for (i = 0; i < size ; ++i ){
		for (j = 0 ; j < size ; ++j){
			if(matrix[i][j] != 0)
				return FALSE;
		}
	}
	
	return TRUE;
}

void printMessageMatrix(int a , int b , int size , int messageType, int direction, int matrix[size][size]){
	int i ;
	if(direction == SEND)
		printf("%d trimite ",a);
	else 
		printf("%d primeste ",a);

	if(direction == SEND)
		printf("catre %d ", b);
	else
		printf("de la %d ", b);
		
	
	if (messageType == ECHO_MESSAGE && isEmptyMatrix(size , matrix))
		printf("echo empty");
	else if(messageType == ECHO_MESSAGE)
		printf("echo");
	else
		printf("sonda");
	printf("\n");	
	
	// printMatrix(matrix , size);
			
}

void logicalORMatrix(int size, int from[size][size], int to[size][size]){
	int i ;
	int j ;
		
	int ** aux = (int **) calloc (size , sizeof(int *));
	for(i = 0 ; i < size ; ++i){
		aux[i] = (int *) calloc (size , sizeof(int));
	}
	
	for(i = 0 ; i < size ; ++i){
		for (j = 0 ; j < size ; ++j){
			aux[i][j] = to[i][j];
		}
	}
	
	for (i = 0 ; i < size ; ++i){
		for (j = 0 ; j < size ; ++j){
			if(from[i][j] == 1){
				aux[i][j] = 1;
			}
		}
	}
		
	for(i = 0 ; i < size ; ++i){
		for (j = 0 ; j < size ; ++j){
			to[i][j] = aux[i][j];
		}
	}
}

void combineMatrixAdiacenta(int size ,int rank , int matrix[size][size] , int adiacenta[size]){
	
	int i ;
	
	for (i = 0; i < size ; ++i){
		matrix[rank][i] |= adiacenta[i];
	}
}

void createRoutingVector(int size , int rank , int matrix[size][size], int * vector){
	
	int i;
	int j;
		
	for(i = 0 ; i < size ; ++i){
		if(matrix[rank][i] == 1){
			vector[i] = i;
		}
	}
	
	for(i = 0 ; i < size ; ++i){
		if(vector[i] == -1 && i != rank){
			for (j = 0 ; j < size ; ++j){
				if(matrix[j][i] == 1){
					vector[i] = j;
				}
			}
		}
	}
}