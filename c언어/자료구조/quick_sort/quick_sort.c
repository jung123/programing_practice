/*
	quick sort
	: ��������, ������������ �����ϰ� ���α׷��� ����.
	�ߺ��� ���� ó���� �� �ִ�.
	�ߺ��� left_ptr, right_ptr ���� <=, >= ó���� ��, �̵� ������ start ~ end�� �ϸ� �ȴ�. 
	
	*** ����鼭 ���� ��. ***
	- ������ ������ �� �� �ִٸ� ����� unsigned�� ó������ ���ƶ�. !
	unsigned �� signed �� �� ����Ǵ� ���, �׻� unsigned�� ����ȯ�Ǿ� ó���Ǳ� ������
	�������� ���� ������ �� �� �ִ�.
	�̰� ������ ������ ����. !! 
*/

#include <stdio.h>

#define DATA int
#define DATA_ARR_SIZE 10

//#define ASCENDING_ORDER
#define DESCENDING_ORDER
#define DEBUG_MODE

typedef unsigned int uint32_t;
typedef signed int int32_t;

#ifdef DEBUG_MODE
uint32_t callNum;
#endif

void quick_sort(DATA array[], int32_t start, int32_t end);
uint32_t sort_func(DATA array[], int32_t start, int32_t end);
void swap ( DATA array[], uint32_t idx_a, uint32_t idx_b );

int main()
{
	#ifdef DEBUG_MODE
	callNum = 0;
	#endif
	uint32_t i;
	DATA inputArray[DATA_ARR_SIZE] = {10,9,8,10,3,5,8,3,2,1};
	
	printf ("[����] : ");
	for (i=0;i<DATA_ARR_SIZE; i++) printf ("%d ", inputArray[i]);
	printf ("\n");
	//
	quick_sort( inputArray, 0, 9 ); 
	//	
	printf ("[���] : ");
	for (i=0;i<DATA_ARR_SIZE; i++) printf ("%d ", inputArray[i]);
	printf ("\n");
	#ifdef DEBUG_MODE
	printf ("ȣ���� �Լ��� �� : %d\n", callNum);
	#endif
	
	return 0;
}
// ������ 
void quick_sort(DATA array[], int32_t start, int32_t end)
{
	#ifdef DEBUG_MODE
	uint32_t callNumber = callNum++;
	printf ("----------- %d -------------\n", callNumber);
	printf ("start : %d, end : %d\n", start, end);
	printf ("[%d] ���� : ", callNumber);
	uint32_t i; 
	for (i = start; i <= end; i++) printf ("%d ", array[i]);
	printf ("\n");
	#endif 
	// range validation
	if (start >= end) return;
	// 
	uint32_t mid = sort_func(array, start, end);
	#ifdef DEBUG_MODE
	printf ("[%d] ���� : ", callNumber);
	for (i = start; i <= end; i++) printf ("%d ", array[i]);
	printf ("\n");
	#endif
	// start < mid < end 
	if ( ( start < mid ) && ( mid < end ) )
	{
		quick_sort(array, start, mid);
		quick_sort(array, mid + 1, end);
	}
	// start == mid => start �κ��� ������ �����ٴ� ���� �ǹ� 
	// ������ �� �κ��� �˻�� �ʿ���� �� �ִ�. 24line���� �˻簡 �����ϱ� �����̴�.
	// �Լ��� ȣ������ �ʴ� ���� �� ���ٴ� �������� �̷��� ��. 
	if ( start == mid )
	{
		if ( mid+1 < end ) quick_sort(array, mid+1, end);	// 2������ ���� 1���� �ɰ����� ��쵵 �ִ�. �� ��� ȣ������ �ʱ� ���� if 
	}
	// end == mid => end �κ��� ������ �������� �ǹ� 
	if ( end == mid )
	{
		if ( start < mid ) quick_sort(array, start ,mid-1);
	}
	#ifdef DEBUG_MODE
	printf ("[%d] ���� : ", callNumber);
	for (i = start; i <= end; i++) printf ("%d ", array[i]);
	printf ("\n");
	#endif 
}
// ���� ���� �Լ� 
uint32_t sort_func(DATA array[], int32_t start, int32_t end)
{
	int32_t pivot = start;
	int32_t left_ptr = start + 1;
	int32_t right_ptr = end; 
	//
	while (1)
	{
		#ifdef ASCENDING_ORDER
		// right pivot���� �׻� ū ���� �;��Ѵ�. => �����ʿ� �� ū�� : �������� 
		while ((right_ptr >= start) && (array[right_ptr] >= array[pivot]))
		{	// �ߺ��� ����ϸ� pivot index���� �������� right_ptr�� ������ �� �ִ�. 
			right_ptr--;
		}
		// left pivot���� �׻� ���� ���� �;��Ѵ�. => ���ʿ� �� ���� �� : �������� 
		while ( (left_ptr <= end) && (array[left_ptr] <= array[pivot]) )
		{	// �ߺ��� ����ϸ� end���� left_ptr�� ���������� ������ �� �ִ�. 
			left_ptr++;
		}
		#endif
		#ifdef DESCENDING_ORDER
		// right pivot���� �׻� ���� ���� �;��Ѵ�. => �����ʿ� �� ���� �� : �������� 
		while ((right_ptr >= start) && (array[right_ptr] <= array[pivot]))
		{	// �ߺ��� ����ϸ� pivot index���� �������� right_ptr�� ������ �� �ִ�. 
			right_ptr--;
		}
		// left pivot���� �׻� ū ���� �;��Ѵ�. => �����ʿ� �� ���� �� : �������� 
		while ( (left_ptr <= end) && (array[left_ptr] >= array[pivot]) )
		{	// �ߺ��� ����ϸ� end���� left_ptr�� ���������� ������ �� �ִ�. 
			left_ptr++;
		}
		#endif
		// �˻� swap
		if ( left_ptr < right_ptr )
		{
			swap (array, left_ptr, right_ptr);
			#ifdef DEBUG_MODE
			int32_t i = 0;
			for (i=start; i<=end; i++) printf ("%d ", array[i]);
			printf ("\n");
			#endif 
		} 
		else	// ���� ���� ����. pivot�� ���� ���� ���� �����̰� �ٸ����� ũ�ų� ���� ��. 
		{
			// �ش� array�� pivot�� ���� ū ���� ���
			if ( left_ptr > end )
			{
				swap (array, right_ptr, pivot);
				break;
			} 
			else if ( right_ptr < start )
			{	// �ش� array�� pivot�� ���� ���� ���� ���
				right_ptr = pivot;
				break;
			}
			else
			{	// �Ϲ����� ��� 
				swap (array, right_ptr, pivot);
				break;
			}
		}
	}
	return right_ptr;
}
// swap �Լ�
void swap ( DATA array[], uint32_t idx_a, uint32_t idx_b )
{
	DATA tmp = array[idx_a];
	array[idx_a] = array[idx_b];
	array[idx_b] = tmp;
}







