#include <stdio.h>

int main()
{
	char str[20];
	
	fgets(str, 20 ,stdin);
	
	int *lpint = (int *)str;
	int res;
	for(int i =0; i<5;i++)
	{
		res+=lpint[i];
		printf("lpint[%d]:%d/HEX:%x res:%d res:%x\n",i, lpint[i],lpint[i],res,res);
	}
	
	
}
