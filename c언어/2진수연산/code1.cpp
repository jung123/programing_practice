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
	i=(lena > lenb ? tmpa : tmpb );	// 2���� �������� �� ��� ! 
	res[i+1] = 0;					// res ���ڿ��� �������� null�� �־��ش�. 
	for(;i>=0;i--){
		tmp = 0;			// �������� �� ������ ������ ������̴�. 
		ach = (tmpa < 0 ? '0' : a[tmpa--]);	// ���ڷ� ���� �ΰ��� ������ �� �ϳ��� ��������� 
		bch = (tmpb < 0 ? '0' : b[tmpb--]);	// �ε����� ���̳ʽ��̴� �̶��� ��� '0'���� ���ִٰ� �����ϱ� 
		if(ach == '1') tmp++;	// ���ϴ� ���� ���� 0 Ȥ�� 1�̴�. 1�ΰ�쿡�� tmp�� ���� ���Ѵ� 
		if(bch == '1') tmp++;
		if(carry =='1') tmp++;
		//
		if(tmp %2 == 1) res[i+1] = '1';	// tmp�� 3Ȥ�� 1�̴� �� ��쿡�� ������� 1�� �����ؾ� �ϴ� ����̴�. 
		else res[i+1] = '0';	// 4�� ������ ���� ���� carry�� ���� �� ������ ��� '1'�� ����ε�
								// �̶��� ������� '0'�� �־�����Ѵ�. 
		if(tmp >=2 ) carry = '1';	// tmp�� ���� 3�� ������ carry�� �����. 
		else carry='0';			// tmp�� ���� 0 Ȥ�� 1�� ��� carry�� �߻����� �ʴ´�. 
	}
	// ������ ��� �Ŀ� carry�� ���� ��츦 ó���ϴ� �κ��̴�.
	if(carry == '0') res[0] = '0';  
	else res[0] = '1';
}
// a * b
void binarymul(char a[],char b[], char res[]){
	// �� �������� ���̸� �����ϴ� �����̴�. 
	int lena = strlen(a), lenb = strlen(b);
	// 1��° �������� 2��° �������� �ڸ������� ���� �����ؾ��Ѵ�.
	// ��, ������ �� �� 2��° �������� '0'���� Ȥ�� '1'������ ���ؼ�
	// ����Ű�� ���̴�. 
	int tmpb = lenb;
	// ������ �� �ڸ������� �����Ǵ� �ӽ����� �������̴�.
	// �ִ��� 2���� �������� ���̸� ���� �� �������� null�� �߰���Ų �����̴�. 
	char tmp_arr[lena+lenb+1] = {0,};
	int i,j,it;
	// ������ ����� ��� �迭�� �ʱ�ȭ�Ѵ�. 
	// ������ �ϴ� ��� ������ � ���� ��� �ȴ�. 
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
			==> �̷��� �߰��� ����� �ӽ� �������� ��� ���ϸ� ������ ����� �����. !! 
		*/ 
		// �Ʒ��� �ڵ�� ������ ����� �ӽ� �������� tmp_arr�� �����ϴ� �κ��̴�. 
		if(b[--tmpb] == '0') continue;
		for(j=0;j<lena;j++) tmp_arr[j] = a[j];
		for(it = i;it > 0;it--) tmp_arr[j++] = '0';
		
		//�ӽ� �������� ����ϰ���� ��� !! 
		//printf("%d | %s\n",i,tmp_arr);
		
		// �����Ǵ� �ӽ� �������� ���� �� ����ؼ� res[]�� ���ϴ��ڵ��̴�. 
		binaryplus(res,tmp_arr,res);
		
		//���� ������ ������ ����� ����ϴ� �ڵ��̴�. 
		//printf("Test : %s\n",res);
		
		//
	}
	
	return ;
}
