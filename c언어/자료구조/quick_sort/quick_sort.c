/*
	quick sort
	: 오름차순, 내림차순으로 정렬하게 프로그램을 만듬.
	중복된 수도 처리할 수 있다.
	중복은 left_ptr, right_ptr 값을 <=, >= 처리한 후, 이동 범위를 start ~ end로 하면 된다. 
	
	*** 만들면서 느낀 것. ***
	- 범위가 음숙가 될 수 있다면 절대로 unsigned로 처리하지 말아라. !
	unsigned 와 signed 수 가 연산되는 경우, 항상 unsigned로 형변환되어 처리되기 때문에
	예측하지 못한 동작을 할 수 있다.
	이것 때문에 오류가 났음. !! 
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
	
	printf ("[이전] : ");
	for (i=0;i<DATA_ARR_SIZE; i++) printf ("%d ", inputArray[i]);
	printf ("\n");
	//
	quick_sort( inputArray, 0, 9 ); 
	//	
	printf ("[결과] : ");
	for (i=0;i<DATA_ARR_SIZE; i++) printf ("%d ", inputArray[i]);
	printf ("\n");
	#ifdef DEBUG_MODE
	printf ("호출한 함수의 수 : %d\n", callNum);
	#endif
	
	return 0;
}
// 퀵정렬 
void quick_sort(DATA array[], int32_t start, int32_t end)
{
	#ifdef DEBUG_MODE
	uint32_t callNumber = callNum++;
	printf ("----------- %d -------------\n", callNumber);
	printf ("start : %d, end : %d\n", start, end);
	printf ("[%d] 이전 : ", callNumber);
	uint32_t i; 
	for (i = start; i <= end; i++) printf ("%d ", array[i]);
	printf ("\n");
	#endif 
	// range validation
	if (start >= end) return;
	// 
	uint32_t mid = sort_func(array, start, end);
	#ifdef DEBUG_MODE
	printf ("[%d] 정렬 : ", callNumber);
	for (i = start; i <= end; i++) printf ("%d ", array[i]);
	printf ("\n");
	#endif
	// start < mid < end 
	if ( ( start < mid ) && ( mid < end ) )
	{
		quick_sort(array, start, mid);
		quick_sort(array, mid + 1, end);
	}
	// start == mid => start 부분은 정렬이 끝났다는 것을 의미 
	// 솔직히 이 부분의 검사는 필요없을 수 있다. 24line으로 검사가 가능하기 때문이다.
	// 함수를 호출하지 않는 것이 더 좋다는 생각으로 이렇게 함. 
	if ( start == mid )
	{
		if ( mid+1 < end ) quick_sort(array, mid+1, end);	// 2개에서 각각 1개로 쪼개지는 경우도 있다. 이 경우 호출하지 않기 위해 if 
	}
	// end == mid => end 부분은 정렬이 끝났음을 의미 
	if ( end == mid )
	{
		if ( start < mid ) quick_sort(array, start ,mid-1);
	}
	#ifdef DEBUG_MODE
	printf ("[%d] 이후 : ", callNumber);
	for (i = start; i <= end; i++) printf ("%d ", array[i]);
	printf ("\n");
	#endif 
}
// 내부 정렬 함수 
uint32_t sort_func(DATA array[], int32_t start, int32_t end)
{
	int32_t pivot = start;
	int32_t left_ptr = start + 1;
	int32_t right_ptr = end; 
	//
	while (1)
	{
		#ifdef ASCENDING_ORDER
		// right pivot보다 항상 큰 값이 와야한다. => 오른쪽에 더 큰값 : 오름차순 
		while ((right_ptr >= start) && (array[right_ptr] >= array[pivot]))
		{	// 중복을 허용하면 pivot index보다 왼쪽으로 right_ptr이 움직일 수 있다. 
			right_ptr--;
		}
		// left pivot보다 항상 작은 값이 와야한다. => 왼쪽에 더 작은 값 : 오름차순 
		while ( (left_ptr <= end) && (array[left_ptr] <= array[pivot]) )
		{	// 중복을 허용하면 end보다 left_ptr이 오른쪽으로 움직일 수 있다. 
			left_ptr++;
		}
		#endif
		#ifdef DESCENDING_ORDER
		// right pivot보다 항상 작은 값이 와야한다. => 오른쪽에 더 작은 값 : 내림차순 
		while ((right_ptr >= start) && (array[right_ptr] <= array[pivot]))
		{	// 중복을 허용하면 pivot index보다 왼쪽으로 right_ptr이 움직일 수 있다. 
			right_ptr--;
		}
		// left pivot보다 항상 큰 값이 와야한다. => 오른쪽에 더 작은 값 : 내림차순 
		while ( (left_ptr <= end) && (array[left_ptr] >= array[pivot]) )
		{	// 중복을 허용하면 end보다 left_ptr이 오른쪽으로 움직일 수 있다. 
			left_ptr++;
		}
		#endif
		// 검사 swap
		if ( left_ptr < right_ptr )
		{
			swap (array, left_ptr, right_ptr);
			#ifdef DEBUG_MODE
			int32_t i = 0;
			for (i=start; i<=end; i++) printf ("%d ", array[i]);
			printf ("\n");
			#endif 
		} 
		else	// 같은 경우는 없다. pivot과 같은 값인 경우는 공통이고 다른점은 크거나 작은 것. 
		{
			// 해당 array에 pivot이 가장 큰 값인 경우
			if ( left_ptr > end )
			{
				swap (array, right_ptr, pivot);
				break;
			} 
			else if ( right_ptr < start )
			{	// 해당 array에 pivot이 가장 작은 값인 경우
				right_ptr = pivot;
				break;
			}
			else
			{	// 일반적인 경우 
				swap (array, right_ptr, pivot);
				break;
			}
		}
	}
	return right_ptr;
}
// swap 함수
void swap ( DATA array[], uint32_t idx_a, uint32_t idx_b )
{
	DATA tmp = array[idx_a];
	array[idx_a] = array[idx_b];
	array[idx_b] = tmp;
}







