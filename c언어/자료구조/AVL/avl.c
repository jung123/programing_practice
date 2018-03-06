// 추가로 책에 있는 코드와 성능비교한번해보자. 
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define LEFT_POS 1
#define RIGHT_POS 2

#define LL_ROTATE 1
#define LR_ROTATE 2
#define RR_ROTATE 3
#define RL_ROTATE 4
// calcHeightDiffer()
#define L_POS 0x2
#define R_POS 0x1

//#define DEBUG_MODE
#define DATA int

typedef unsigned int uint32_t;
typedef signed long int32_t;

// AVL Tree Node
typedef struct _AVL_Node {
	struct _AVL_Node *parent_ptr;
	struct _AVL_Node *left_ptr;
	struct _AVL_Node *right_ptr;
	uint32_t leftTreeHeight; 
	uint32_t rightTreeHeight;
	int32_t differOfHeight;
	DATA value;
}AVL_Node;
// AVL Tree Manage Struct
typedef struct _AVL_manage {
		uint32_t numberOfNode;
		uint32_t heightOfTree;
		AVL_Node *root_ptr;
} AVL_manage;
// init 
void initAVL ( AVL_manage *manage );
void initNode ( AVL_Node *node );
// create Node
AVL_Node *createAVLNode ( DATA value );
/*
	insert
	0 : 중복된 값이 있거나  다른 이유(?)로 삽입에 실패
	1 : 삽입 성공 
	: 중복된 Node는 삽입하지 않는다. 
		중복처리시 방법 :
			1. left ptr에 삽입
			2. 해당 Node에 따로 linked list를 만들어 관리. 
*/  
uint32_t insertAVLNode ( AVL_manage *manage, AVL_Node *newNode );
/*
	position Search Func
	: insert 함수에서 해당 Node가 삽입시 부모 Node를 찾아주는 함수이다. 
	- reuturn
		NULL : 이미 해당 Data를 갖는 Node가 있는 경우 
		addr : 해당 Node의 Left or Right에 Node를 연결하면 된다. 
*/
AVL_Node *insert_search ( AVL_manage *manage, AVL_Node *node );
/*
	insert_linkNode
	: new Node를 parent Node의 direction에 삽입. 
*/
int32_t insert_linkNode ( AVL_Node *parent, AVL_Node *newNode, uint32_t direction );
/*
	calcHeightDiffer
		- return 
			0 : 비정상 (  )
			1 : 정상 (  ) 
		- rotateInfo
			- 0 : 회전이 불필요하다.
			- LL_ROTATE : 1
			- LR_ROTATE : 2
			- RR_ROTATE : 3
			- RL_ROTATE : 4
*/
int32_t calcHeightDiffer ( AVL_manage *manage, AVL_Node *newNode, AVL_Node **rotatePos, uint32_t *rotateInfo);
// pop Node
 /* 이거 엄청 어려울 듯. 기능이 필요한지 생각해봐야함. AVL Tree에서 없는 기능일 수도 있다. */
// rotation Tree
void rightRotation ( AVL_manage *manage, AVL_Node *eventNode );
void leftRotation ( AVL_manage *manage, AVL_Node *eventNode ); 
uint32_t max ( uint32_t a, uint32_t b );
// search 

// print
void printAVL ( AVL_Node *node , AVL_Node *root);
// delete AVL Tree
void deleteAVL ( AVL_manage *manage );
void deleteTree ( AVL_Node *node );
int main ()
{
	srand(time(NULL));
	AVL_manage *manage = (AVL_manage *) malloc ( sizeof (AVL_manage) );
	initAVL ( manage );
	//
	int i = 0;
	DATA testInput[] = { 50, 25, 75, 10, 35, 70, 85, 5, 15, 30, 42, 60, 72, 80, 95, 13, 28, 31, 40, 32 };
	for ( i ; i < 1000 ; i++)
	{
		insertAVLNode(manage, createAVLNode ( (rand()%2000) ) );	
	}
	// print
	//printAVL ( manage->root_ptr , manage->root_ptr);
	printf ("Tree에는 총 %u개의 Node가 존재합니다.\n ", manage->numberOfNode);
	printf ("Tree는 %u의 높이입니다.\n", manage->heightOfTree);
	printf ("root: ( %u, {%d} %u)\n", manage->root_ptr->leftTreeHeight, manage->root_ptr->value, manage->root_ptr->rightTreeHeight);
	//
	deleteAVL (manage);
	return 0;
}
// init 
void initAVL ( AVL_manage *manage )
{
	manage->heightOfTree = manage->numberOfNode = 0;
	manage->root_ptr = NULL;
}
void initNode ( AVL_Node *node )
{
	node->differOfHeight = node->leftTreeHeight = node->rightTreeHeight = 0;
	node->left_ptr = node->right_ptr = node->parent_ptr = NULL;
}
// create Node
AVL_Node *createAVLNode ( DATA value )
{
	AVL_Node *tmp = (AVL_Node *) malloc (sizeof (AVL_Node));
	if (tmp == NULL)
	{
		printf ("[createAVLNode] malloc error !\n");
		return NULL; // 원래는 exit(1)을 하는 것이 맞다. 이것으로 인해 발생하는 모든 예외를 처리하는 것은 거의 불가능 
	}
	//
	initNode ( tmp );
	//
	tmp->value = value; 
	return tmp;
}
/*
	insert
	0 : 중복된 값이 있거나  다른 이유(?)로 삽입에 실패
	1 : 삽입 성공 
	: 중복된 Node는 삽입하지 않는다. 
		중복처리시 방법 :
			1. left ptr에 삽입
			2. 해당 Node에 따로 linked list를 만들어 관리. 
*/ 
uint32_t insertAVLNode ( AVL_manage *manage, AVL_Node *newNode )
{
	#ifdef DEBUG_MODE
	printf ("\n------------ [insertAVLNode] newNode(%d) start ------------\n", newNode->value);
	#endif
	/*
		1. 삽입될 위치까지 찾아간다.
		2. 삽입 연산을 한다.
		3. 올라가며 height값과 difference값을 수정한다. 
		4. 
			- 수정 후 differ >= 2라면 rotation 연산을 수행한다. => <rotation 하면서 조정되는 곳의 height값과 differ값을 조정한다.>
					: Left Tree or Right Tree에 새로운 Tree나 Node가 붙은 Node드만 처리하면된다.
					 내 생각에 differ >= 2인 곳에 손자 Node까지만 처리해주면될 것 같다. ==> Rotation 함수에서 처리하도록 하자. 
			-  수정 후 differ < 2라면 계속 올라간다.
		5. root까지 올라간다.
		--- root가 ratation 되는 경우도 생각해야 한다. 
	*/ 
	// Tree가 비었는지 확인.
	if ( manage->numberOfNode == 0 ) 
	{
		manage->root_ptr = newNode;
		manage->numberOfNode = 1;
		manage->heightOfTree = 1;
		#ifdef DEBUG_MODE 
		printf ("[insertAVLNode] root = newNode(%d)\n", newNode->value);
		#endif
		return 1;
	}
	// new Node가 삽입될 position 찾기.
	AVL_Node *parent = insert_search ( manage, newNode );
	#ifdef DEBUG_MODE
	printf ("insert_search_call: newNode(%d) : parent = %d \n", newNode->value, parent->value);
	#endif
	if ( parent == NULL )
	{	// 이미 중복된 값이 있는 경우 
		//printf ("[insertAVLNode] %d 값은 이미 Tree에 존재합니다.\n");
		return 0;
	}
	// parent의 left or right 결정 및 newNode linking 
	if ( newNode->value < parent->value )	// parent의 left에 newNode 삽입 
	{
		if ( !insert_linkNode ( parent, newNode, LEFT_POS ) )
		{
			printf ("[insertAVLNode] : linking error !\n");
			return 0;
		}
	}
	else if ( newNode->value > parent->value )	// parent의 right에 newNode 삽입 
	{
		if ( !insert_linkNode ( parent, newNode, RIGHT_POS ) )
		{
			printf ("[insertAVLNode] : linking error !\n");
			return 0;
		}
	}
	// height 및 differ 값 계산
	uint32_t rotateInfo = 0;
	AVL_Node *rotatePos = NULL;
	if ( !calcHeightDiffer ( manage, newNode, &rotatePos, &rotateInfo ) )
	{	// calcHeightDiffer func 비정상 종료 
		printf ("[insertAVLNode] : calcHeightDiffer Error!\n");
		exit(1);
	}
	// rotate
	#ifdef DEBUG_MODE
	printAVL( manage->root_ptr , manage->root_ptr);
	printf ("\n");
	#endif
	if ( rotateInfo != 0 )
	{	// 회전이 필요한 경우 
		switch ( rotateInfo )
		{	// rotate하면서 height 및 differ 값을 바꾼다. 
			case LL_ROTATE :
			{	//	LL 회전 
				// balance깨진 eventNode에서 right 회전 
				#ifdef DEBUG_MODE
				printf ("[insertAVLNode] : LL_ROTATE !\n");
				#endif
				rightRotation( manage, rotatePos );
				break;
			}	
			case LR_ROTATE : 
			{	// LR 회전 
				// [1] balance 깨진 eventNode의 leftNode를 left 회전 
				// [2] balance 깨진 eventNode를 right 회전
				#ifdef DEBUG_MODE
				printf ("[insertAVLNode] : LR_ROTATE !\n");
				#endif
				leftRotation( manage, rotatePos->left_ptr ); 
				rightRotation( manage, rotatePos );
				break;
			}
			case RR_ROTATE : 
			{	// RR 회전 
				// balance 깨진 eventNode를 left 회전
				#ifdef DEBUG_MODE
				printf ("[insertAVLNode] : RR_ROTATE !\n");
				#endif
				leftRotation( manage, rotatePos ); 
				break;
			}
			case RL_ROTATE : 
			{	// RL 회전 
				// [1] : balance 깨진 eventNode의 rightNode를 right 회전 
				// [2] : balance 깨진 eventNode를 left회전
				#ifdef DEBUG_MODE
				printf ("[insertAVLNode] : RL_ROTATE !\n");
				#endif
				rightRotation( manage, rotatePos->right_ptr );
				leftRotation( manage, rotatePos ); 
				break;
			}
		}
		// Left Tree Height and Right Tree Height 비교
		if ( rotatePos->parent_ptr->leftTreeHeight != rotatePos->parent_ptr->rightTreeHeight )
		{
			printf ("[insertAVLNode] : after Rotate, balance Error!\n");
			exit (1);
		} 
	}
	// 삽입 성공 
	return 1;
	#ifdef DEBUG_MODE
	printf ("\n------------ [insertAVLNode] newNode(%d) end ------------\n", newNode->value);
	#endif
}
/*
	position Search Func
	: insert 함수에서 해당 Node가 삽입시 부모 Node를 찾아주는 함수이다. 
*/
AVL_Node *insert_search ( AVL_manage *manage, AVL_Node *node )
{
	AVL_Node *tmp = manage->root_ptr;
	AVL_Node *tmp1;
	while (1)
	{
		// Left Tree 
		if ( tmp->value > node->value )
		{
			if ( tmp->left_ptr == NULL ) return tmp;
			tmp = tmp->left_ptr;
		}
		else if ( tmp->value < node->value )
		{ // Right Tree
			if ( tmp->right_ptr == NULL ) return tmp;
			tmp = tmp->right_ptr;
		}
		else if ( tmp->value == node->value )
		{ // 중복 ! 
			return NULL;
		}
	}
}
/*
	insert_linkNode
	: new Node를 parent Node의 direction에 삽입. 
*/
int32_t insert_linkNode ( AVL_Node *parent, AVL_Node *newNode, uint32_t direction )
{
	if ( direction == RIGHT_POS )
	{
		parent->right_ptr = newNode;
		newNode->parent_ptr = parent;
	}
	else if ( direction == LEFT_POS )
	{
		parent->left_ptr = newNode;
		newNode->parent_ptr = parent;
	}
	else 
		return 0;	// 비정상 
	return 1;	// 정상 
	
}
/*
	calcHeightDiffer
		- return 
			0 : 비정상 (  )
			1 : 정상 (  ) 
		- rotateInfo
			- 0 : 회전이 불필요하다.
			- LL_ROTATE : 1
			- LR_ROTATE : 2
			- RR_ROTATE : 3
			- RL_ROTATE : 4
		- height 처리
			: AVL의 특성을 확인해야한다.
			- == 0 break
			newNode를 추가하여 newNode의 parent node에서 newNode가 추가된 방향의 subtree height값을 증가하는 것
			이 후, diff값이 0이면 newNode->parent_ptr->parent_ptr의 subtree height값은 증가하지 않아도 된다.
			왜냐하면 이미 그 높이로 계산되어 있기 때문이다.
			- >= 2
			newNode 추가한 후 균형이 깨진 상태이다.
			이때 회전을 하면 diff >= 2가 발생한 node를 eventNode라고 한다면 
			eventNode->parent_ptr은 sub Tree height값은 회전을 하기 전에는 균형이 깨져 수정이 필요하지만
			회전을하면 eventNode->parent_ptr의 sub tree height값은 수정되지 않아도 정확한 값을 가진다. 
*/
int32_t calcHeightDiffer ( AVL_manage *manage, AVL_Node *newNode, AVL_Node **rotatePos , uint32_t *rotateInfo)
{
	// Tree의 높이가 2이상일 경우멘 가능 
	AVL_Node *tmp = newNode;
	AVL_Node *tmp1 = NULL;
	uint32_t prevPath = 0;
	uint32_t prevHeight = 0;
	//
	tmp1 = tmp;
	tmp = newNode->parent_ptr;
	int32_t res = 0;
	//
	while ( ( tmp != NULL ) && ( manage->root_ptr != tmp1 ) )	
	{	// root이상으로 가면 멈춘다. 
	
		if ( tmp->left_ptr == tmp1 )
		{	// 이전의 위치가 Left Tree 
			prevPath = prevPath << 2;
			prevPath = prevPath + L_POS;
			// calc height 
			/* 이거 어렵다. 이전 노드의 높이 값을 계속해서 비교하며 올라갈까? 그러다가 높이값이 증가하지 않으면 differ는 발생하지 않으므*/
			/* height차이는 삽입된 위치에서 올라가다보면 높이가 같아진다면 loop를 종료해도 댄다. 하지만 1이라도 차이가 난다면 계속 올라가야함. */
			tmp->leftTreeHeight = prevHeight + 1;
		}
		else if ( tmp->right_ptr == tmp1 )
		{	// 이전의 위치가 Right Tree 
			prevPath = prevPath << 2;
			prevPath = prevPath + R_POS;
			// calc height 
			tmp->rightTreeHeight = prevHeight + 1;
		}
		// calc differ
		res = tmp->leftTreeHeight - tmp->rightTreeHeight;
		tmp->differOfHeight = ( ( res < 0 ) ? -res : res );
		//
		if ( tmp->differOfHeight == 0 )
		{	// 해당 서브트리는 균형 이진트리이다. (회전이 필요없다.)
			// tmp의 parent node로 갈 필요가 없는 이유는 이미 이전에 tmp->parent는 해당 방향의 height가 최신화된 상태이기 때문이다. 
			 break;
		} 
		else if ( tmp->differOfHeight >=2 ) 
		{
			*rotatePos = tmp;
			break;
		}
		// 위로 올라간다.
		/* prevHeight++
			올라가는 경우는 추가된 방향의 Sub Tree의 크기가 이전보다 더 커젔기 떼문이다.
			그래서 parent node의 해당 방향의 sub tree도 커진것. 안 커진 경우는 이전에 diff가 0인 경우는 안커지지만
			1인 경우는 parent tree의 해당 방향 sub tree height를 조정해야 한다. 
		*/  
		prevHeight++; 
		tmp1 = tmp;
		tmp = tmp->parent_ptr; 
	}
	// root에서 멈춘 경우, height확인 필요
	if ( ( tmp == NULL ) && ( manage->root_ptr == tmp1 ) ) 
	{	// root까지 올라오면서 differ >= 2인 경우는 없는 상황이지만 subtree의 height는 커짐. 
		// AVL Tree Height Info 최신화
		// root의 right Sub Tree가 더 높은 경우 
		if ( tmp1->rightTreeHeight > tmp1->leftTreeHeight ) 
		{
			manage->heightOfTree = tmp1->rightTreeHeight;
		}
		else 
		{	// root의 left Sub Tree가 더 높은 경우. 
			manage->heightOfTree = tmp1->leftTreeHeight;
		}
	}
	// [이거 제대로 동작하는지 확인 필요]
	if ( *rotatePos != NULL )
	{	// 회전이 필요한 경우 이전, 이전의 이전의 순서가 LR, LL, RR, RL이 맞는지 확인필요 아마 맞을 것이다. 
		
		if ( ( prevPath & L_POS ) > 0 )
		{	// 이전에 Left Tree에서 온 경우 
			if ( ( ( prevPath >> 2 ) & L_POS ) > 0 )
			{	// LL
				*rotateInfo = LL_ROTATE;
			}
			else if ( ( ( prevPath >> 2 ) & R_POS ) > 0 )
			{	// LR
				*rotateInfo = LR_ROTATE;
			}
		}
		else if ( ( prevPath & R_POS ) > 0 )
		{	//  이전에 Right Tree에서 온 경우 
			if ( ( ( prevPath >> 2 ) & L_POS ) > 0 )
			{	// RL
				*rotateInfo = RL_ROTATE;
			}
			else if ( ( ( prevPath >> 2 ) & R_POS ) > 0 )
			{	// RR
				*rotateInfo = RR_ROTATE;
			}
		}
		else
		{	// error !
			printf ("[calcHeightDiffer] : rotatePos logic Error\n");
			return 0;
		}
	}
	else
	{	// 회전이 필요없는 경우 
		*rotateInfo = 0;
	} 
	manage->numberOfNode++;
	return 1;
}
// rotation Tree
/* 여기 디버그 확인 */
void rightRotation ( AVL_manage *manage, AVL_Node *eventNode )
{
	#ifdef DEBUG_MODE
	printf (" Right Rotation !, eventNode(%d)\n", eventNode->value);
	#endif
	// eventNode가 root인지 확인해야 한다.
	AVL_Node *node2 = eventNode->left_ptr;
	AVL_Node *moveNode = node2->right_ptr;
	AVL_Node *parentNode = eventNode->parent_ptr;	// NULL => eventNode = root
	// [1] moveNode
	if ( moveNode != NULL )
	{
		eventNode->left_ptr = moveNode;
		moveNode->parent_ptr = eventNode;	
	}
	else
	{
		eventNode->left_ptr = NULL;
	}
	// [2] node2
	eventNode->parent_ptr = node2;
	node2->right_ptr = eventNode;
	// [3] parent
	if ( ( manage->root_ptr ==  eventNode) && ( parentNode == NULL ) )
	{	// root 인 경우 
		manage->root_ptr = node2; 
		node2->parent_ptr = NULL;
	}
	else
	{	// root가 아닌 경우 
		if ( parentNode->left_ptr == eventNode )
		{
			parentNode->left_ptr = node2;
			node2->parent_ptr = parentNode;
		}
		else if ( parentNode->right_ptr == eventNode )
		{
			parentNode->right_ptr = node2;
			node2->parent_ptr = parentNode;
		}
		else
		{	// 예외 상황 절대 불가능 
			printf ("[rightRotation] : parent Node Linking Error !\n ");
			return;
		}
	}
	// eventNode height
	if ( moveNode != NULL )
	{
		eventNode->leftTreeHeight = max ( moveNode->leftTreeHeight, moveNode->rightTreeHeight ) + 1;
	}
	else
	{
		eventNode->leftTreeHeight = 0;
	}
	// node2 height
	node2->rightTreeHeight = max ( node2->right_ptr->leftTreeHeight, node2->right_ptr->rightTreeHeight ) + 1;
	// 확인
	#ifdef DEBUG_MODE
	printf ("Right Rotation Check \n");
	printAVL ( manage->root_ptr, manage->root_ptr );
	printf ("\n");
	#endif 
}
/* 여기 디버그 확인 */
void leftRotation ( AVL_manage *manage, AVL_Node *eventNode )
{
	#ifdef DEBUG_MODE
	printf (" Left Rotation, eventNode(%d) !\n", eventNode->value);
	#endif
	AVL_Node *node2 = eventNode->right_ptr;
	AVL_Node *moveNode = node2->left_ptr;
	AVL_Node *parentNode = eventNode->parent_ptr;
	// [1] move node
	if ( moveNode != NULL )
	{
		eventNode->right_ptr = moveNode;
		moveNode->parent_ptr = eventNode;
	}
	else
	{
		eventNode->right_ptr = NULL;
	}
	// [2] node2
	node2->left_ptr = eventNode;
	eventNode->parent_ptr = node2;
	// [3] parent
	if ( ( manage->root_ptr == eventNode ) && ( parentNode == NULL ) )
	{	// eventNode가 root인 경우
		manage->root_ptr = node2; 
		node2->parent_ptr = NULL;
	}
	else
	{	// root가 아닌 경우 
		if ( parentNode->left_ptr == eventNode )
		{	// eventNode가 parent의 Left Sub Tree 
			node2->parent_ptr = parentNode;
			parentNode->left_ptr = node2;
		}
		else if ( parentNode->right_ptr == eventNode )
		{	// eventNode가 parent의 Right Sub Tree 
			node2->parent_ptr = parentNode;
			parentNode->right_ptr = node2;
		}
		else
		{	// 예외가 거의 있을 수 없다. 
			printf ("[leftRotation] : parent Node Linking Error !\n");
			return;
		}
	}
	// eventNode height
	if ( moveNode != NULL )
	{
		eventNode->rightTreeHeight = max ( moveNode->leftTreeHeight, moveNode->rightTreeHeight ) + 1;
	}
	else
	{
		eventNode->rightTreeHeight = 0;
	}
	// node2 height
	node2->leftTreeHeight = max ( node2->left_ptr->leftTreeHeight, node2->left_ptr->rightTreeHeight ) + 1;
	// 확인
	#ifdef DEBUG_MODE
	printf ("Left Rotation Check \n");
	printAVL ( manage->root_ptr, manage->root_ptr );
	printf ("\n");
	#endif 
	
}
// print
void printAVL ( AVL_Node *node , AVL_Node *root)
{
	if ( node->left_ptr != NULL ) printAVL ( node->left_ptr , root);
	if ( node != root ) printf (" ( %d [%d] %d )", node->leftTreeHeight, node->value, node->rightTreeHeight);
	else printf (" ( %d {{%d}} %d )", node->leftTreeHeight, node->value, node->rightTreeHeight);
	if ( node->right_ptr != NULL ) printAVL ( node->right_ptr , root);
}
// max
uint32_t max ( uint32_t a, uint32_t b )
{
	if ( a > b )
	{
		return a;
	}
	else
	{
		return b;
	}
}
// delete AVL Tree
void deleteAVL ( AVL_manage *manage )
{
	deleteTree ( manage->root_ptr );
	free ( manage );
}
void deleteTree ( AVL_Node *node )
{
	if ( node->left_ptr != NULL ) deleteTree ( node->left_ptr );
	if ( node->right_ptr != NULL ) deleteTree ( node->right_ptr );
	free (node);
}















