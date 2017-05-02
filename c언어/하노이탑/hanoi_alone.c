#include <stdio.h>
#include <stdlib.h>

/* run this program using the console pauser or add your own getch, system("pause") or input loop */
void hanoi(int n,char from,char temp, char to){
	if( n == 1 )
	{
		return printf("%d번째 원반이 %c에서 %c로 옮겨젔습니다.! \n",n,from,to);
	}
	hanoi(n-1,from,to,temp);
	printf("%d번째 원반이 %c에서 %c로 옮겨젔습니다.! \n",n,from,to);
	hanoi(n-1,temp,from,to);
}
int main(int argc, char *argv[]) {
	int i=0;
	int n;
		while(1){
		printf("hanoi 원반을 몇개 쌓을까요???\n");
		scanf("%d",&n);
		printf("A에 원반이 쌓여 C로 갑니다 !!!\n");
		hanoi(n,'A','B','C');
		printf("계속하고 싶은면 1을 그만하고 싶으면 2를...\n");
		scanf("%d",&i);
		if(i==2) break;
		system("cls");
		
	}
	return 0;
}
