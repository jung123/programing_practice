/*
	�˰��� ���� 2
	insertion sort�� ����Լ��� ����� !! 
*/
#include <stdio.h>
#include <stdlib.h>

void insertion_sort(int arr[],int len);
int sort(int arr[],int len,int num);

int main(){
	printf("Insertion Sort�� �����ϴ� ���α׷��Դϴ�!!\n");
	//
	printf("�� ��� ���ڸ� �Է��Ͻǰǰ���??\n");
	int n;
	scanf("%d",&n);
	int *arr = (int *)malloc(sizeof(int)*n);
	int i;
	printf("n���� ������ �Է��Ͻÿ� !!\n");
	for(i=0;i<n;i++){
		scanf("%d",arr+i);
	}
	//
	printf("�����ϱ� �� ���ڵ��Դϴ� !\n");
	for(i=0;i<n;i++) printf("%d ",*(arr+i));
	printf("\n");
	insertion_sort(arr,n);
	printf("\n������ �� ���ڵ��Դϴ� !\n");
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
	if(arr[len-1] < num) return num;	// ũ�ٸ� ���� �׳� return �ȴ� ! 
	else{
		int tmp = arr[len-1];
		arr[len-1] = sort(arr,len-1,num); //  ������ ������ ȣ���ѹ��Ѵ�
		return tmp;
	}
}



