#include <stdio.h>
#include <stddef.h>

#define DATA int
#define ARR_SIZE 10

//#define DEBUG_MODE 
#define ASCENDING_ORDER
//#define DESCENDING_ORDER

int number = 0;
typedef unsigned int uint32_t;
// merge sort
void merge_sort(DATA array[ARR_SIZE], uint32_t start, uint32_t end, DATA outputArray[]);

int main()
{
	DATA inputArray[ARR_SIZE] = {10,5,4,2,9,1,8,3,6,7};
	DATA outputArray[ARR_SIZE] = {0};
	
	merge_sort(inputArray, 0, ARR_SIZE-1, outputArray);
	int i;
	for (i =0; i<ARR_SIZE; i++){
		printf("%d ", outputArray[i]);
	}
	printf ("\n");
	
	return 0;
}

// merge sort
void merge_sort(DATA array[ARR_SIZE], uint32_t start, uint32_t end, DATA outputArray[])
{
	#ifdef DEBUG_MODE 
	printf ("------------------ %d -------------------\n", number);
	unsigned int test = number++;
	unsigned int i = 0;
	printf ("����[%d] : ", test);
	for (i=start; i<=end; i++) printf ("%d ", array[i]);
	printf ("\n");
	#endif
	// range validation
	if (start >= end) return;
	//
	uint32_t mid = (start + end) / 2;
	// ȣ���� range validation test�� �� ���ΰ�? 
	// �� �ڵ尡 �����ϰ� �����ϴ� �κ��� ��ü�� �Ϻΰ� �ƴϴ�.
	// �ִ°� ���� ��..
	
	// left array sort
	if ( !(start >= mid) ) merge_sort(array, start, mid, outputArray);
	// right array sort
	if ( !(mid+1 >= end) ) merge_sort(array, mid + 1, end, outputArray);
	
	// merge code!
	uint32_t left_ptr = start;
	uint32_t right_ptr = mid + 1;
	uint32_t curr_ptr = start;
	
	while(1)
	{
		// left ptr valid
		if ( left_ptr > mid ) break;
		// right ptr valid
		if ( right_ptr > end ) break;
		#ifdef ASCENDING_ORDER
		// ���� ����
		if ( array[left_ptr] <= array[right_ptr] )
		{
			outputArray[curr_ptr++] = array[left_ptr++];
		}else {
			outputArray[curr_ptr++] = array[right_ptr++];
		}	
		#endif
		#ifdef DESCENDING_ORDER 
		// ���� ���� 
		if ( array[left_ptr] >= array[right_ptr] ) 
		{	
			outputArray[curr_ptr++] = array[left_ptr++];
		}else {
			outputArray[curr_ptr++] = array[right_ptr++];
		}	
		#endif
	}
	// ä��� (ä��⸦ �� ������ �������� / ���������� ���� X)
	if ( left_ptr > mid ) 
	{
		for (; right_ptr <= end ;) outputArray[curr_ptr++] = array[right_ptr++];
	}
	if ( right_ptr > end )
	{
		for (; left_ptr <= mid ;) outputArray[curr_ptr++] = array[left_ptr++];
	}
	// copy outputArray to inputArray
	unsigned int j=start;
	for (j=start; j<=end; j++) array[j] = outputArray[j];
	
	#ifdef DEBUG_MODE 
	printf ("����[%d] : ", test);
	for (i=start; i<=end; i++) printf ("%d ", outputArray[i]);
	printf ("\n");
	test++;
	#endif
	
	return;
}
