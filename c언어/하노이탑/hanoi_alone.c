#include <stdio.h>
#include <stdlib.h>

/* run this program using the console pauser or add your own getch, system("pause") or input loop */
void hanoi(int n,char from,char temp, char to){
	if( n == 1 )
	{
		return printf("%d��° ������ %c���� %c�� �Űܠ����ϴ�.! \n",n,from,to);
	}
	hanoi(n-1,from,to,temp);
	printf("%d��° ������ %c���� %c�� �Űܠ����ϴ�.! \n",n,from,to);
	hanoi(n-1,temp,from,to);
}
int main(int argc, char *argv[]) {
	int i=0;
	int n;
		while(1){
		printf("hanoi ������ � �������???\n");
		scanf("%d",&n);
		printf("A�� ������ �׿� C�� ���ϴ� !!!\n");
		hanoi(n,'A','B','C');
		printf("����ϰ� ������ 1�� �׸��ϰ� ������ 2��...\n");
		scanf("%d",&i);
		if(i==2) break;
		system("cls");
		
	}
	return 0;
}
