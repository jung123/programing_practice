#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#define MAX_NUMBER 1000
#define ARR_SIZE 100

void fillArr(uint32_t *arr);
void sortArr(uint32_t *arr);
void printArr(uint32_t *arr);

int main(){
	uint32_t arr[ARR_SIZE] = {0,};
	uint32_t tmpArr[ARR_SIZE] = {0,};
	uint32_t i,j, success, index;
	srand(time(NULL));
	//
	fillArr(arr);
	sortArr(arr);
	//
	printf("숫자찾기 게임입니다. !!\n"); 
	printf("1 ~ %u까지의 숫자중에 하나의 값을 찾으면 됩니다 !!\n",MAX_NUMBER);
	printf("%u 크기의 배열안에 랜덤한 숫자들이 정렬되어있습니다.(중복숫자 X)\n",ARR_SIZE);
	printf("이때 주어진 숫자가 들어가 있을 배열의 index를 10번안에 구하시면 됩니다 !!\n");
	success = rand()%ARR_SIZE;
	printf("\n 당신이 찾아야하는 숫자는 %u입니다. !!",arr[success]);
	//
	for(i=0;i<10;i++){
		printf("\n------------------------ %2d Game ----------------------------\n",i+1);
		printf("Find %u\n",arr[success]);
		do{
			printf("your input index(0 ~ %u) : ",ARR_SIZE-1);
			scanf("%u",&index);	
		}while((index < 0) || (index > ARR_SIZE-1));
		if(arr[success] == arr[index]){
			printf("%d의 시도에서 GAME에서 승리하셨습니다. !!\n",i+1);
			printArr(arr);
			break;
		}
		tmpArr[index] = arr[index];
		printArr(tmpArr);
	}
	//
	return 0;
}
void sortArr(uint32_t *arr){
	uint32_t i,j, tmp;
	for(i=ARR_SIZE-1;i>0;i--){
		for(j=0;j<i;j++){
			if(arr[i] < arr[j]){
				tmp = arr[j];
				arr[j] = arr[i];
				arr[i] = tmp;
			}
		}
	}
}
void fillArr(uint32_t *arr){
	uint32_t i,j, tmp;
	uint32_t check=0;
	
	for(i=0;i<ARR_SIZE;i++) {
		check = 0;
		tmp = rand()%MAX_NUMBER+1;
		for(j=0;j<i;j++){
			if(arr[j] == tmp){
				check =1;
				break;
			}
		}
		//
		if(check == 1) i--;
		else arr[i] = tmp;
	}
}
void printArr(uint32_t *arr){
	uint32_t i;
	printf("\n-------------------------------------------------------------");
	for(i=0;i<ARR_SIZE;i++){
		if((i%5) == 0) printf("\n");
		if(arr[i] == 0) printf("%3u :    X ",i);
		else printf("%3u : %4u ",i,arr[i]);
	}
	printf("\n-------------------------------------------------------------\n");
}
