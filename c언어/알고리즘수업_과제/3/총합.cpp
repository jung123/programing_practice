/*
	알고리즘 과제 1
	n개의 정수의 총합을 구하는 재귀함수 작성하기 ! 
*/

#include<stdio.h>
#include<stdlib.h> 

float sum_num(float arr[], int n);

int main(){
	freopen("input_1.txt","r",stdin);
	
	float *arr = NULL;
	printf("N개의 수를 입력받아 총합을 구하는 프로그램입니다 !\n");
	printf("총 몇개의 숫자를 입력하실 건가요??\n N : ");
	//
	int n;
	scanf("%d",&n);
	arr = (float *)malloc(sizeof(float) * n);
	//
	int i;
	for(i=0;i<n;i++){
		scanf("%f",arr+i);
	}
	//
	float res = sum_num(arr,n);
	printf("총합 : %f",res);
	return 0;
}

float sum_num(float arr[], int n){
	if(n == 1) return arr[n-1];
	else return (arr[n-1] + sum_num(arr,n-1));
}


