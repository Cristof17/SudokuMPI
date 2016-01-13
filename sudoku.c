#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define TRUE 1
#define FALSE 0

void printMatrix(int ** matrix , int size);
int sudoku (int ** matrix , int size , int line , int col);
int isValid (int ** matrix , int size , int line , int col , int value);


int main(){
	
	int squareSize;
	int ** matrix;
	int i;
	int j;
	
	FILE * inFile = fopen("sudoku2.txt" , "r+");
	
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
	
	sudoku(matrix, squareSize * squareSize , 0 , 0 );	
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
	
	printf("\n");
}

int sudoku (int ** matrix , int size , int line , int col){
	int i = 0; 
	int j = 0;
	
	if(line >= size){
		
		printMatrix(matrix , size);
		printf("\n");
		return TRUE;
	}
	
	if(col == size)
		return sudoku(matrix , size , line + 1 , 0);
	
	if(matrix[line][col] != 0){
		sudoku(matrix , size , line , col + 1);
	}
	
	for(i = 1 ; i < size + 1 ; ++i){
		if(isValid (matrix , size , line , col , i )){
			
			matrix[line][col] = i;
			
			int solved = FALSE;
			 
			if(col == size)
				solved = sudoku(matrix, size , line + 1 , 0);
			solved = sudoku(matrix , size , line , col + 1);
			
			if (solved)
				printf("Sudoku solved");
				
			matrix[line][col] = 0 ;
		}
	}
	
	return FALSE ;
}

int isValid (int ** matrix , int size , int line , int col , int value){
	int i;
	int j;
	
	for(i = 0 ; i < size ; ++i){
		if(matrix[line][i] == value){
			return FALSE;
		}
	}
	
	for(j = 0 ; j < size ; ++j){
		if(matrix[j][col] == value){
			return FALSE;
		}
	}
	
	int patratSize = sqrt(size);
	int lineRoot = (line / patratSize) * patratSize;
	int colRoot = (col / patratSize) * patratSize;
	
	for(i = 0 ; i < patratSize ; ++i){
		for(j = 0 ; j < patratSize ; j++){
			if(matrix[lineRoot + i][colRoot + j] == value){
				return FALSE;
			}
		}
	}
		
	return TRUE;
}