#include <stdio.h>
#include <stdlib.h>

void printMatrix(int ** matrix , int size);

int main(){
	
	int squareSize;
	int ** matrix;
	int i;
	int j;
	
	FILE * inFile = fopen("sudoku.txt" , "r+");
	
	fscanf(inFile, "%d\n" , &squareSize);
	matrix = (int ** ) calloc (squareSize * squareSize , sizeof(int *));
	for(i = 0 ; i < squareSize * squareSize ; ++i){
		matrix[i] = (int *) calloc (squareSize * squareSize , sizeof(int));
	}
	
	for(i = 0 ; i < squareSize * squareSize ; ++i){
		for(j = 0 ; j < squareSize * squareSize ; ++j){
			fscanf(inFile ,"%d " , &matrix[i][j]);
		}
	}
	
	
	printMatrix(matrix , squareSize * squareSize);
	return 0;
	
}

void printMatrix(int ** matrix , int size){
	int i;
	int j;
	
	for(i = 0 ; i < size ; ++i){
		for(j = 0 ; j < size ; ++j){
			printf("%d " , matrix[i][j]);
		}
		printf("\n");
	}
}