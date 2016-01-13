#include <stdio.h>

void foo(int size , int * matrix){
	int i ;
	int j ;
	
		
	for(i = 0 ; i < size ; ++i){
		for(j = 0 ; j < size ; ++j){
			printf("%d " , matrix[j + i*size]);
		}
		printf("\n");
	}
	
	for(i = 0 ; i < size ;++i){
		for (j = 0 ; j < size ; ++j){
			matrix[j + i * size] = 0;
		}
	}

}

int main(){
	
	int i ; 
	int j ;
	int size = 3;
	int matrix[size][size];
	foo(size, &matrix[0][0]);
	
	for(i = 0 ; i < size ; ++i){
		for (j = 0 ; j < size ; ++j){
			printf("%d " , matrix[i][j]);
		}
		printf("\n");
	}
	return 0;
}