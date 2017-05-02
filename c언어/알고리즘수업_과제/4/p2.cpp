#include <iostream>

using namespace std;

int getMaxSumArrange(int *arr,int len, int& up, int& down, int start);
int getSum(int arr[], int len, int index, int& end);
int getminusSum(int arr[], int len, int index, int& minusend);

int main(){
	freopen("p2input.txt", "r", stdin);
	int *arr = NULL;
	int len,i;
	printf("몇개의 숫자를 입력하십니까?\n 숫자 : ");
	scanf("%d",&len);
	//
	arr = new int [len];
	for(i=0;i<len;i++){
		scanf("%d",arr+i);
	}
	//
	int up,down;
	int sum = getMaxSumArrange(arr,len,up,down,0);
	cout << "---------- 결과 ------------" << endl;
	cout << "가장큰 값 : " << sum << endl;
	cout << " arrage : " << down << " ~ " << up << endl;
	delete[] arr;
	return 0;
}

int getMaxSumArrange(int *arr,int len, int& up, int& down, int start){
	// 끝나는 시점 
	if(start == len) return 0;
	// 음수인 경우 
	if(arr[start] < 0) return getMaxSumArrange(arr,len,up,down,start+1);
	// 양수의 범위 및 합산값 찾기
	int end = start; 
	int mySum = getSum(arr,len,start,end);
	// 바로뒤의 음수의 합을 구하기
	int minusend = end+1;
	int minusSum = getminusSum(arr,len,end+1,minusend);	
	// 다음 수들의 범위들부터 가장 큰 것찾기 
	int returnSum = getMaxSumArrange(arr,len,up,down,end+1);
	// 숫자의 나열이 양의 정수로 끝나는 경우
	if(end == minusend){
		up = end;
		down = start;
		return mySum;
	}
	// 음의 정수로 끝나는 경우는 아래에 if문에서 처리한다.
	 
	//cout << "------------------------------------" << endl;
	//cout << "return sum : " << returnSum << endl;
	//cout << "return range : " << down << " ~ " << up << endl;
	//cout << "mysum : " << mySum << endl;
	//cout << "my racge : " << start << " ~ " << end << endl;
	//cout << "------------------------------------" << endl;
	
	// 이 경우가 그 다음 범위의 양의정수부터를 범위로 포함하는 것이다. 
	if(minusend+1 == down){
		int total = mySum + minusSum + returnSum;
		if((total > mySum) && (total > returnSum)){
			down = start;
			return total;
		}else if((returnSum > total) && (returnSum > mySum)){
			return returnSum;
		}else if((mySum > total) && (mySum > returnSum)){
			down = start;
			up = end;
			return mySum;
		}
	}else{
		// 나의 양수의 범위와 다음 양수의 범위 중 더 큰것 비교
		if(mySum > returnSum){
			down = start;
			up = end;
			return mySum;
		}else{
			return returnSum;
		}
	}
	cout << "이거 나오면 에러임!!" << endl;
}
// 이거는 getMaxSumArrange()에서 for문으로도 가능하지만 그냥 재귀적으로 만듬 
int getSum(int arr[], int len, int index, int& end){
	// 배열의 마지막을 만나면 종료 
	if(index == len) {
		end = index-1;
		return 0;
	}
	// 음수를 만나면 종료 
	if(arr[index] < 0){
		end = index-1;
		return 0;
	}
	//
	return (arr[index] + getSum(arr,len,index+1,end)); 
}

int getminusSum(int arr[], int len, int index, int& minusend){
	if(index == len){
		minusend = index-1;
		return 0;
	}
	if(arr[index] > 0){
		minusend = index-1;
		return 0;
	}
	return (arr[index] + getminusSum(arr,len,index+1, minusend));
}
