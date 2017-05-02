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
	printf("����ã�� �����Դϴ�. !!\n"); 
	printf("1 ~ %u������ �����߿� �ϳ��� ���� ã���� �˴ϴ� !!\n",MAX_NUMBER);
	printf("%u ũ���� �迭�ȿ� ������ ���ڵ��� ���ĵǾ��ֽ��ϴ�.(�ߺ����� X)\n",ARR_SIZE);
	printf("�̶� �־��� ���ڰ� �� ���� �迭�� index�� 10���ȿ� ���Ͻø� �˴ϴ� !!\n");
	success = rand()%ARR_SIZE;
	printf("\n ����� ã�ƾ��ϴ� ���ڴ� %u�Դϴ�. !!",arr[success]);
	//
	for(i=0;i<10;i++){
		printf("\n------------------------ %2d Game ----------------------------\n",i+1);
		printf("Find %u\n",arr[success]);
		do{
			printf("your input index(0 ~ %u) : ",ARR_SIZE-1);
			scanf("%u",&index);	
		}while((index < 0) || (index > ARR_SIZE-1));
		if(arr[success] == arr[index]){
			printf("%d�� �õ����� GAME���� �¸��ϼ̽��ϴ�. !!\n",i+1);
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
