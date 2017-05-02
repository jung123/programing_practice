#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void binaryplus(char a[],char b[],char res[]);
void binarymul(char a[],char b[], char res[]);

int main(){
	char a[100] = {0};
	char b[100] = {0};
	char res[100] = {0};
	printf("input A : ");
	scanf("%s",a);
	printf("input B : ");
	scanf("%s",b);
	//
	binaryplus(a,b,res);
	printf("A + B : %s\n",res);
	//
	binarymul(a,b,res);
	printf("A * B : %s\n",res);
	return 0;
}

void binaryplus(char a[],char b[], char res[]){
	int lena = strlen(a), lenb = strlen(b);
	int tmpa = lena-1, tmpb = lenb-1;
	int i,tmp;
	char ach,bch,carry = '0';
	i=(lena > lenb ? tmpa : tmpb );	// 2개의 이진수중 더 긴것 ! 
	res[i+1] = 0;					// res 문자열의 마지막에 null을 넣어준다. 
	for(;i>=0;i--){
		tmp = 0;			// 이진수의 각 진수별 덧셈시 결과값이다. 
		ach = (tmpa < 0 ? '0' : a[tmpa--]);	// 인자로 받은 두개의 이진수 중 하나가 더작은경우 
		bch = (tmpb < 0 ? '0' : b[tmpb--]);	// 인덱스가 마이너스이다 이때는 모두 '0'으로 되있다고 생각하기 
		if(ach == '1') tmp++;	// 더하는 곳의 값이 0 혹은 1이다. 1인경우에는 tmp의 값을 더한다 
		if(bch == '1') tmp++;
		if(carry =='1') tmp++;
		//
		if(tmp %2 == 1) res[i+1] = '1';	// tmp가 3혹은 1이다 이 경우에는 결과값에 1을 저장해야 하는 경우이다. 
		else res[i+1] = '0';	// 4가 나오는 경우는 없고 carry가 없고 두 이진수 모두 '1'인 경우인데
								// 이때는 결과값에 '0'을 넣어줘야한다. 
		if(tmp >=2 ) carry = '1';	// tmp의 값이 3이 나오면 carry가 생긴다. 
		else carry='0';			// tmp의 값이 0 혹은 1인 경우 carry가 발생하지 않는다. 
	}
	// 마지막 계산 후에 carry가 생긴 경우를 처리하는 부분이다.
	if(carry == '0') res[0] = '0';  
	else res[0] = '1';
}
// a * b
void binarymul(char a[],char b[], char res[]){
	// 두 이진수의 길이를 저장하는 변수이다. 
	int lena = strlen(a), lenb = strlen(b);
	// 1번째 이진수는 2번째 이진수의 자리수별로 값을 생성해야한다.
	// 즉, 곱셈을 할 때 2번째 이진수가 '0'인지 혹은 '1'인지에 대해서
	// 가리키는 것이다. 
	int tmpb = lenb;
	// 곱셈시 각 자리수별로 생성되는 임시적인 이진수이다.
	// 최댓값은 2개의 이진수의 길이를 더한 후 마지막에 null을 추가시킨 길이이다. 
	char tmp_arr[lena+lenb+1] = {0,};
	int i,j,it;
	// 덧셈시 결과를 담는 배열을 초기화한다. 
	// 덧셈을 하는 경우 어차피 어떤 값도 없어도 된다. 
	memset(res,0,lena+lenb+1); 
	//
	for(i=0;i<lenb; i++){
		/*
			a : 101010
			b : 100111
			+1 : 101010
			+2 : 1010100
			+3 : 10101000
			+4 : 00000000
			+5 : 00000000
			+6 : 10101000000
			==> 이렇게 중간에 생기는 임시 이진수를 모두 더하면 곱셈의 결과가 생긴다. !! 
		*/ 
		// 아래의 코드는 위에서 언급한 임시 이진수를 tmp_arr에 생성하는 부분이다. 
		if(b[--tmpb] == '0') continue;
		for(j=0;j<lena;j++) tmp_arr[j] = a[j];
		for(it = i;it > 0;it--) tmp_arr[j++] = '0';
		
		//임시 이진수를 출력하고싶은 경우 !! 
		//printf("%d | %s\n",i,tmp_arr);
		
		// 생성되는 임시 이진수를 생성 후 계속해서 res[]에 더하는코드이다. 
		binaryplus(res,tmp_arr,res);
		
		//각각 더해질 때마다 결과를 출력하는 코드이다. 
		//printf("Test : %s\n",res);
		
		//
	}
	
	return ;
}
