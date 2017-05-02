#include <iostream>

using namespace std;

int getMaxSumArrange(int *arr,int len, int& up, int& down, int start);
int getSum(int arr[], int len, int index, int& end);
int getminusSum(int arr[], int len, int index, int& minusend);

int main(){
	freopen("p2input.txt", "r", stdin);
	int *arr = NULL;
	int len,i;
	printf("��� ���ڸ� �Է��Ͻʴϱ�?\n ���� : ");
	scanf("%d",&len);
	//
	arr = new int [len];
	for(i=0;i<len;i++){
		scanf("%d",arr+i);
	}
	//
	int up,down;
	int sum = getMaxSumArrange(arr,len,up,down,0);
	cout << "---------- ��� ------------" << endl;
	cout << "����ū �� : " << sum << endl;
	cout << " arrage : " << down << " ~ " << up << endl;
	delete[] arr;
	return 0;
}

int getMaxSumArrange(int *arr,int len, int& up, int& down, int start){
	// ������ ���� 
	if(start == len) return 0;
	// ������ ��� 
	if(arr[start] < 0) return getMaxSumArrange(arr,len,up,down,start+1);
	// ����� ���� �� �ջ갪 ã��
	int end = start; 
	int mySum = getSum(arr,len,start,end);
	// �ٷε��� ������ ���� ���ϱ�
	int minusend = end+1;
	int minusSum = getminusSum(arr,len,end+1,minusend);	
	// ���� ������ ��������� ���� ū ��ã�� 
	int returnSum = getMaxSumArrange(arr,len,up,down,end+1);
	// ������ ������ ���� ������ ������ ���
	if(end == minusend){
		up = end;
		down = start;
		return mySum;
	}
	// ���� ������ ������ ���� �Ʒ��� if������ ó���Ѵ�.
	 
	//cout << "------------------------------------" << endl;
	//cout << "return sum : " << returnSum << endl;
	//cout << "return range : " << down << " ~ " << up << endl;
	//cout << "mysum : " << mySum << endl;
	//cout << "my racge : " << start << " ~ " << end << endl;
	//cout << "------------------------------------" << endl;
	
	// �� ��찡 �� ���� ������ �����������͸� ������ �����ϴ� ���̴�. 
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
		// ���� ����� ������ ���� ����� ���� �� �� ū�� ��
		if(mySum > returnSum){
			down = start;
			up = end;
			return mySum;
		}else{
			return returnSum;
		}
	}
	cout << "�̰� ������ ������!!" << endl;
}
// �̰Ŵ� getMaxSumArrange()���� for�����ε� ���������� �׳� ��������� ���� 
int getSum(int arr[], int len, int index, int& end){
	// �迭�� �������� ������ ���� 
	if(index == len) {
		end = index-1;
		return 0;
	}
	// ������ ������ ���� 
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
