/*
	�˰��� ���� 1
	n���� ������ ������ ���ϴ� ����Լ� �ۼ��ϱ� ! 
*/

#include<stdio.h>
#include<stdlib.h> 

float sum_num(float arr[], int n);

int main(){
	freopen("input_1.txt","r",stdin);
	
	float *arr = NULL;
	printf("N���� ���� �Է¹޾� ������ ���ϴ� ���α׷��Դϴ� !\n");
	printf("�� ��� ���ڸ� �Է��Ͻ� �ǰ���??\n N : ");
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
	printf("���� : %f",res);
	return 0;
}

float sum_num(float arr[], int n){
	if(n == 1) return arr[n-1];
	else return (arr[n-1] + sum_num(arr,n-1));
}


