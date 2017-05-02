#include <stdio.h>
#include <stdlib.h>

#define MAX_LIST_SIZE 100

typedef int element;

typedef struct {
	element list[MAX_LIST_SIZE];
	int length;
} ArrayListType;

void init(ArrayListType *plist);

void add(ArrayListType *plist, int position, element item);

int is_full(ArrayListType *plist);

int is_empty(ArrayListType *plist);

element delete_list(ArrayListType *plist, int position);

void display(ArrayListType *plist);

int main() {
	ArrayListType *plist;
	plist = (ArrayListType *)malloc(sizeof(ArrayListType));
	init(plist);
	add(plist, 0, 10);
	add(plist, 2, 20);
	add(plist, 3, 30);
	display(plist);
	free(plist);
}

void init(ArrayListType *plist) {
	plist->length = 0;
}

int is_full(ArrayListType *plist) {
	return plist->length == MAX_LIST_SIZE;
}

int is_empty(ArrayListType *plist) {
	return plist->length == 0;
}

void display(ArrayListType *plist) {
	for (int i = 0; i < plist->length; i++) {
		printf("%d\n", plist->list[i]);
	}
}

element delete_list(ArrayListType * plist, int position)
{
	element item=plist->list[position];

	for (int i = position; i <= plist->length - 1; i++)
		plist->list[i] = plist->list[i + 1];

	plist->length--;
	return item;
}

void add(ArrayListType *plist, int position, element item) {
	if ((!is_full(plist) && (position >= 0)) && (position <= plist->length)) {
		for (int i = (plist->length - 1); i >= position; i--)
			plist->list[i + 1] = plist->list[i];
		
		plist->list[position] = item;
		plist->length++;
	}
}