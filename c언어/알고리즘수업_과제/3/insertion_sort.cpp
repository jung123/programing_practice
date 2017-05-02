/*
	알고리즘 과제 2
	insertion sort를 재귀함수로 만들기 !! 
*/
#include <stdio.h>
#include <stdlib.h>

void insertion_sort(int arr[],int len);
int sort(int arr[],int len,int num);

int main(){
	printf("Insertion Sort를 실행하는 프로그램입니다!!\n");
	//
	printf("총 몇개의 숫자를 입력하실건가요??\n");
	int n;
	scanf("%d",&n);
	int *arr = (int *)malloc(sizeof(int)*n);
	int i;
	printf("n개의 정수를 입력하시오 !!\n");
	for(i=0;i<n;i++){
		scanf("%d",arr+i);
	}
	//
	printf("정렬하기 전 숫자들입니다 !\n");
	for(i=0;i<n;i++) printf("%d ",*(arr+i));
	printf("\n");
	insertion_sort(arr,n);
	printf("\n정렬한 후 숫자들입니다 !\n");
	for(i=0;i<n;i++) printf("%d ",*(arr+i));
	//
	return 0;
}
void insertion_sort(int arr[],int len){
	if(len < 1) return;
	
	insertion_sort(arr,len-1);
	arr[len-1] = sort(arr,len-1,arr[len-1]);
	
	int i;
	for(i=0;i<10;i++){
		printf("%d ",arr[i]);
	}
	printf("\n");
	return;
}

int sort(int arr[],int len,int num){
	if(len < 1) return num;
	//
	if(arr[len-1] < num) return num;	// 크다면 경우는 그냥 return 된다 ! 
	else{
		int tmp = arr[len-1];
		arr[len-1] = sort(arr,len-1,num); //  작으면 앞으로 호출한번한다
		return tmp;
	}
}



