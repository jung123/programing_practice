#include<stdio.h>
#include<stdlib.h>

void selection_sort(int arr[],int pos,int len);
int sort(int arr[], int pos, int len, int num, int sort_num);

int main(){
	int n;
	printf("정렬하는 수의 갯수가 몇개 입니까??\n");
	printf("갯수 : ");
	scanf("%d",&n);
	//
	int *arr = (int *)malloc(sizeof(int)*n);
	int i;
	printf("\n 정렬할 수를 입력하시오 !!\n");
	for(i=0;i<n;i++){
		scanf("%d",arr+i);
	}
	printf("정렬하기 전의 숫자들입니다 !\n");
	for(i=0;i<n;i++) printf("%d ",arr[i]);
	printf("\n");
	//
	selection_sort(arr,n,n);
	//
	printf("정렬한 후의 숫자들입니다 !\n");
	for(i=0;i<n;i++) printf("%d ",arr[i]);
	printf("\n");
	
	return 0;
}

void selection_sort(int arr[],int pos,int len){
	if(pos < 1) return;
	selection_sort(arr,pos-1,len);
	arr[pos-1] = sort(arr,pos+1,len,arr[pos-1], arr[pos-1]); 
}
int sort(int arr[], int pos, int len, int num, int sort_num){
	if(pos > len) return num;
	int min = sort(arr,pos+1,len,(arr[pos-1] < num ? arr[pos-1] : num), sort_num);
	if((num != min) && (min == arr[pos-1])) arr[pos-1] = sort_num;
	return min;
}



