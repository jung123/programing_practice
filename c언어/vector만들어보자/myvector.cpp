/*
	c++의 vector같은 기능을 하는 것을 만들어보자 !! 
*/
#include <stdio.h>
#include <stdlib.h>
//
typedef struct vector_ {
	int *ptr;
	int *tmp;
	int number;
	int current_size;
} vector;
//
int get_(vector *myVector, int index);
int push_back(vector *myVector, int num);
void init_vec(vector **myVector);
int resize(vector *myVector, int size);
int get_back(vector *myVector);
int size(vector *myVector);
int empty(vector *myVector);
int delete_vec(vector **myVector);
//
int main(){
	vector *myVector = NULL;
	init_vec(&myVector);
	int num;
	printf("insert number : ");
	scanf("%d",&num);
	push_back(myVector,num);
	printf("insert number : ");
	scanf("%d",&num);
	push_back(myVector,num);
	printf("insert number : ");
	scanf("%d",&num);
	push_back(myVector,num);
	//
	int i;
	for(i=0;i<3;i++){
		printf("%d번째 : %d\n",i+1,get_(myVector,i));
	}
	//
	printf("마지막 : %d\n",get_back(myVector));
	//
	printf("Current size : %d\n",size(myVector));
	resize(myVector,0);
	printf("Current size : %d\n",size(myVector));
	//
	delete_vec(&myVector);
	return 0;
} 
int get_(vector *myVector, int index){
	if((index >= myVector->number) || (index < 0)){
		printf("[get_] : indexing Error !!\n");
		return 0;
	}
	return *(myVector->ptr + index);
}
int push_back(vector *myVector, int num){
	if(myVector->current_size == 0){
		myVector->ptr = (int *)malloc(sizeof(int));
		if(myVector->ptr == NULL){
			printf("[push_back] : malloc Error !!\n");
			return -1;
		}
		*(myVector->ptr) = num;
		myVector->number++;
		myVector->current_size = 4;
		myVector->tmp = myVector->ptr;
	}
	else
	{
		int size = (myVector->number) * sizeof(int);
		if(size < myVector->current_size){
			*(myVector->ptr + myVector->number) = num;
			myVector->number++;
		}
		else if(size == myVector->current_size)
		{
			myVector->tmp = (int *)realloc(myVector->ptr, size + (sizeof(int) * 20));
			if(myVector->tmp == NULL){
				printf("[push_back] : realloc Error !!\n");
				return -1;
			}
			myVector->ptr = myVector->tmp;
			myVector->tmp = NULL;
			//
			myVector->current_size += sizeof(int) * 20;
			*(myVector->ptr + myVector->number) = num;
			myVector->number++;
		}
		else if(size > myVector->current_size)
		{
			printf("[push_back] : Vector Error ! vector를 초기화하시오 !!\n");
			return -1;
		}
		return 1;
	}
}
void init_vec(vector **myVector){
	if(*myVector != NULL){
		printf("[init_vec] : vector가 초기화가 되어있지 않습니다.\n");
		return;
	}
	//
	*myVector = (vector *)malloc(sizeof(vector));
	if(*myVector == NULL){
		printf("[init_vec] : vector 할당에러 !!\n");
		return;
	}
	//
	(*myVector)->number = 0;
	(*myVector)->current_size = 0;
	(*myVector)->ptr = NULL;
	(*myVector)->tmp = NULL;
	return;
}
int resize(vector *myVector, int size){
	int current_size = myVector->current_size;
	int arg_size = (size == 0 ? 0 : size * sizeof(int));
	//
	if(arg_size == 0){
		empty(myVector);
	}
	//
	if(current_size < arg_size)
	{
		myVector->tmp = (int *)realloc(myVector->ptr, current_size + arg_size);
		myVector->ptr = myVector->tmp;
		myVector->tmp = NULL;
		myVector->current_size += arg_size;
	}
	else if(current_size == arg_size)
	{
	}
	else if(current_size > arg_size)
	{
		myVector->tmp = (int *)malloc(arg_size);
		if(myVector->tmp == NULL){
			printf("[resize] : malloc Error !!\n");
			return -1;
		}
		//
		int i;
		int *lp = myVector->ptr;
		for(i=0;i<size;i++){
			*(myVector->tmp + i) = lp[i];
		}
		//
		free(myVector->ptr);
		myVector->ptr = myVector->tmp;
		myVector->tmp = NULL;
		myVector->number = size;
	}
	return 1;
}
int get_back(vector *myVector){
	if(myVector->number == 0){
		printf("[get_back] : vector is empty !!\n");
		return 0;
	}
	return *((myVector->ptr)+(myVector->number-1));
}
int size(vector *myVector){
	return ((myVector->number) * (sizeof(int)));
}
int empty(vector *myVector){
	free(myVector->ptr);
	myVector->ptr = NULL;
	myVector->number = 0;
	myVector->tmp = NULL;
	myVector->current_size = 0;
	return 1;
}

int delete_vec(vector **myVector){
	vector *tmp = *myVector;
	if(tmp == NULL){
		return 1;
	}
	free(tmp->ptr);
	tmp->ptr = tmp->tmp = NULL;
	tmp->number = 0;
	free(tmp);
	return 1;
}




