#include <iostream>
using namespace std;

int bar (int size , int * matrix){
	int i ;
	int j ;
	for (i = 0 ; i < size ; ++i){
		for (j = 0 ; j < size ; ++j){
			cout << matrix[i * size + j]<< " " ;
		}
		cout << endl;
	}
	return 0;
}

int main(){
	
	int i ; 
	int j ;
	int size = 3;
	int matrix[size * size];
	
	bar(size, &matrix[0]);
	return 0;
}