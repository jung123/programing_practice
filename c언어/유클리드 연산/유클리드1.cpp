#include <iostream>
#include <cstdlib>
#include <iomanip>
using namespace std;

typedef struct list_node{
	int ri;
	int rip;
	int qip;
	int ripp;
	int si;
	int ti;
	struct list_node *next;
} node;

typedef struct list_head_{
	int size;
	node *list;
} list_head;
// list init
void init_list(list_head **head);
// get list
node *get_node(list_head *head, int dep);
// delete list
void delete_list(list_head **head);
// delete list node
void delete_node(list_head *head, int dep);
// print list
void print_list(list_head *head);
// push list node
void push_node(list_head *head, node *new_node);
// create node
node *create_node(int ri, int rip, int qip, int ripp, int si, int ti);




int main(){
	cout << "------ Euclidean Algorithm ------" << endl;	
	int a,b;
	cout << "두 정수 a,b를 입력하시오 !!" << endl;
	cout << "a : ";
	cin >> a;
	cout << "b : ";
	cin >> b;
	//
	int left = (a >= b ? a : b);
	int right = (left == a ? b : a);
	int tmp1 = left,tmp2 = right;
	//
	while(true){
		tmp1 = left/right;
		tmp2 = left%right;
		cout << left <<" = " << tmp1 << " * " << right << " + " << tmp2 << endl;
		if(tmp2 == 0) break;
		left = right;
		right = tmp2;
	}
	cout << "gcd = " << right << endl;
	//
	cout << "------ Extended Euclidean Algorithm ------" << endl;
	//
	left = (a >= b ? a : b);
	right = (left == a ? b : a);
	tmp1 = left,tmp2 = right;
	//
	list_head *head= NULL;
	init_list(&head);
	node *temp= NULL;
	//
	tmp1 = left/right;
	tmp2 = left%right;
	cout << left <<" = " << tmp1 << " * " << right << " + " << tmp2 << endl;
	push_node(head,create_node(left,right,tmp1,tmp2,1,0));
	left = right;
	right = tmp2;
	//
	tmp1 = left/right;
	tmp2 = left%right;
	cout << left <<" = " << tmp1 << " * " << right << " + " << tmp2 << endl;
	push_node(head,create_node(left,right,tmp1,tmp2,0,1));
	left = right;
	right = tmp2;
	//
	int i = 3;
	while(right != 0){
		tmp1 = left/right;
		tmp2 = left%right;
		cout << left <<" = " << tmp1 << " * " << right << " + " << tmp2 << endl;
		push_node(head,create_node(left,right,tmp1,tmp2,
							((get_node(head,i-2)->si)-((get_node(head,i-2)->qip)*(get_node(head,i-1)->si))),
							((get_node(head,i-2)->ti)-((get_node(head,i-2)->qip)*(get_node(head,i-1)->ti)))));
		
		left = right;
		right = tmp2;
		i++;
		if(tmp2 == 0) break;
	}
	//
	tmp1 = (right == 0? 0:left/right);
	tmp2 = (right == 0? 0:left%right);
	cout << left <<" = " << tmp1 << " * " << right << " + " << tmp2 << endl;
	push_node(head,create_node(left,right,tmp1,tmp2,(get_node(head,i-2)->si-(get_node(head,i-2)->qip * get_node(head,i-1)->si)),(( get_node(head,i-2)->ti )-(( get_node(head,i-2)->qip )*( get_node(head,i-1)->ti )))));
	//
	print_list(head);
	//
	cout << "GCD : " << left << endl;
	//
	delete_list(&head);
	delete head;
	return 0;
}

// list init
void init_list(list_head **head){
	*head = new list_head;	
	(*head)->list = NULL;
	(*head)->size = 0;
}
// get list
node *get_node(list_head *head, int dep){
	dep = (dep == 0 ? head->size : dep);
	node *temp = head->list;
	if(dep > head->size){
		cout << "[get_node]리스트의 size보다 큰 값을 찾습니다.!!" << endl;
		return NULL;
	}
	int i;
	for(i=1;i<dep;i++){
		temp = temp->next;
	}
	return temp;
}
// delete list
void delete_list(list_head **head){
	node *temp = (*head)->list;
	node *temp1 = NULL;
	while(1){
		temp1 = temp;
		temp = temp->next;
		delete temp1;
		if(temp == NULL) break;
	}
	return;
}
// delete list node
void delete_node(list_head *head, int dep){
	dep = (dep == 0 ? head->size : dep);
	node *temp = head->list;
	int i;
	if(dep == 1){
		head->list = temp->next;
		free(temp);
	}else{
		dep = dep-1;
		for(i=1;i<dep;i++){
			temp = temp->next;
		}
		node *temp1 = temp->next;
		temp->next = temp->next->next;
		delete temp1;
	}
	return ;
}
// print list
void print_list(list_head *head){
	int len = head->size;
	node *temp = head->list;
	int i = 0;
	cout <<endl<< setw(50) << "------------ PRINT LIST ------------" << endl;
	cout << setw(10) << " i | "
		<< setw(10) << " r(i) " 
		<< setw(10) << " r(i+1)" 
		<< setw(10) << " q(i+1)" 
		<< setw(10) << " r(i+2)" 
		<< setw(10) << " s(i)"  
		<< setw(10) << " t(i)" << endl;
	while(temp != NULL){
		cout << setw(7) << i++ << " | "
			<< setw(10) << temp->ri 
			<< setw(10) << temp->rip
			<< setw(10) << temp->qip
			<< setw(10) << temp->ripp
			<< setw(10) << temp->si
			<< setw(10) << temp->ti << endl;
		temp = temp->next;
	}
	return;
}
// push list node
void push_node(list_head *head, node *new_node){
	node *temp = head->list;
	if(temp == NULL){
		head->list = new_node;
	}else{
		while(temp->next != NULL){
			temp = temp->next;
		}
		temp->next = new_node;	
	}
	head->size++;
}
// create node
node *create_node(int ri, int rip, int qip, int ripp, int si, int ti){
	node *temp = new node;
	temp->next = NULL;
	temp->qip = qip;
	temp->ri = ri;
	temp->ripp = ripp;
	temp->si = si;
	temp->ti = ti;
	temp->rip = rip;
	return temp;
}

