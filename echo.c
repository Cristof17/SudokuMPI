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

void initTopology(int ** topology , int size ,int value);
void parseInput(char * filename , char * mode , int ** outMatrix , int * size);
int * parseInputAsArray(char * topologyName, char * sudokuName , char * mode , int rank);
void createTopologyUsingMessages(int ** topology , int size , int rank , int * adiacenta);
int isEmptyMessage(int * receivedMessage, int size );
int getNumberOfNodes(char * filename , char * mode );
void combine (int * top_nou , int * adiacenta , int size ,int rank);
void logicalOR(int * from , int * to , int size);
void printMatrix(int ** matrix , int * size );
void printArray(int * array , int size);
void printMessage(int source , int destination , int * array , int size , int messageTYPE , int direction);

int main(){
	int value; //DEBUG
	int initialized = FALSE ;
	
	MPI_Init(NULL , NULL);
	MPI_Comm_size(MPI_COMM_WORLD , & size);
	MPI_Comm_rank(MPI_COMM_WORLD , & rank);
	
	/*Init */
	if(!initialized){
		topoSize = getNumberOfNodes("sudoku.txt", "r+");
		topology = (int **) malloc (topoSize * sizeof(int *));
		initTopology(topology , topoSize, 0);
		emptyMessage = (int * ) calloc (topoSize , sizeof(int ));
		initialized = TRUE;
	}
	top_nou = parseInputAsArray("echoInput.txt" , "sudoku.txt" , "r+", rank);
	if(top_nou == NULL){
		top_nou = (int *) calloc (topoSize , sizeof(int));
	}
	
	createTopologyUsingMessages(topology , topoSize , rank , top_nou);
			
	MPI_Finalize();
	//INITIATORUL PRINTEAZA MATRICEA INITIALA
	// if(rank == 0 )
	// 	printMatrix(topology , & topoSize);
	

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

void initTopology(int ** topology , int size , int value){
	int i  =0 ;
	int j  =0;
	for(i = 0 ; i < size ; ++i){
		topology[i] = (int *) malloc (size * sizeof(int));
	}
	
	for(i = 0; i < size ; ++i){
		for(j = 0 ; j < size ; ++j){
			topology[i][j] = value;
		}
	}

}

void createTopologyUsingMessages(int ** topology , int size , int rank , int * adiacenta){
		
	// printArray(top_nou , size);
	int * top_nou = (int *) calloc (size , sizeof(int ));
	// printArray(adiacenta , size );
	int parent;
	MPI_Status status;
	
	if(rank == 0){ //INITIATOR
		for(i = 0 ; i < size ; ++i){
			if(adiacenta[i] == 1){
				MPI_Send(emptyMessage , size , MPI_INT , i , SONDA_MESSAGE , MPI_COMM_WORLD);
				printMessage(rank , i , emptyMessage , size , SONDA_MESSAGE , SEND);
				// printf("\n- %d sending sonda to %d :\n" , rank , i);
				// printArray(emptyMessage , size );
				// printf("\n");
			}
		}
	}
	
	else{
		
		//receive sonda
		MPI_Recv(top_nou , size , MPI_INT , MPI_ANY_SOURCE , SONDA_MESSAGE , MPI_COMM_WORLD , &status);
		parent = status.MPI_SOURCE;
		adiacenta[parent] = 1 ;
		printMessage(rank , parent , top_nou , size , SONDA_MESSAGE , RECEIVE);
		// printf("\n- %d received sonda first from %d :\n", rank , parent );
		// printArray(top_nou , size );
		// printf("\n");
		
			
			
		//send sonde to neighbors
		for(i = 0 ; i < size ; ++i){
			if(adiacenta[i] == 1 && i != parent){
				// printf ("Rank %d has neightbor %d \n" , rank , i);
				MPI_Send(emptyMessage , size , MPI_INT , i , SONDA_MESSAGE , MPI_COMM_WORLD);
				printMessage(rank , i , top_nou , size , SONDA_MESSAGE , SEND);
				// printf("\n- %d sending sonda to %d :\n" , rank ,i);
				// printArray(emptyMessage , size );
				// printf("\n");
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
			MPI_Recv(top_nou , size , MPI_INT , MPI_ANY_SOURCE , MPI_ANY_TAG , MPI_COMM_WORLD , &status);
			int source = status.MPI_SOURCE;
			
			//received echo
			printMessage(rank , source , top_nou , size , ECHO_MESSAGE , RECEIVE);
			// printf("\n- %d received echo from %d :\n", rank , source);
			// printArray(top_nou , size );
			// printf("\n");
			
			if(status.MPI_TAG == ECHO_MESSAGE){
				if(top_nou != NULL){
					int empty = isEmptyMessage(top_nou , size);
						if(!empty){
							logicalOR(top_nou , topology[rank], size);
								printf("\n- %d copiaza " , rank);
								printf("pe linia %d",source);
								printf(" pentru ca isEmptyMessage = %d \n", isEmptyMessage(top_nou,size));
								printMatrix(topology, &size);
						}
					numberOfEcho --;
					// combine (top_nou , adiacenta, size ,rank);
				}
			}
			//received sonda when listening for echo
			else if(status.MPI_TAG == SONDA_MESSAGE){
				MPI_Send(emptyMessage , size , MPI_INT , source , ECHO_MESSAGE , MPI_COMM_WORLD);
				printMessage(rank , source , emptyMessage , size , ECHO_MESSAGE , SEND);
				// printf("\n- %d sending empty echo to %d :\n" , rank , source);
				// printArray(emptyMessage , size );
				// printf("\n");
				//delete connection
				if(adiacenta[source] == 1)
					adiacenta[source] = 0;
			}
		}
		
		//send echo to parent
		MPI_Send(adiacenta , size , MPI_INT , parent , ECHO_MESSAGE , MPI_COMM_WORLD);
		printMessage(rank , parent , adiacenta , size , ECHO_MESSAGE , SEND);
		// printf("\n- %d sending echo message to %d : \n" , rank , parent);
		// printArray(adiacenta , size );
		// printf("\n");
	
		// topology[ran	k] = top_nou;
}

int isEmptyMessage(int * receivedMessage , int size){
	int i;

	for(i = 0 ; i < size ; ++i){
		if(receivedMessage[i] != 0)
			return FALSE;
			
		return TRUE;
	}
}

void printArray(int * array , int size){
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
		
		// printf("%s\n" , line); //DEBUG
				
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
	printArray(array , size);
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