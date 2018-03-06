// �߰��� å�� �ִ� �ڵ�� ���ɺ��ѹ��غ���. 
#include <stdio.h>
#include <stdlib.h>

#define LEFT_POS 1
#define RIGHT_POS 2

#define LL_ROTATE 1
#define LR_ROTATE 2
#define RR_ROTATE 3
#define RL_ROTATE 4
// calcHeightDiffer()
#define L_POS 0x2
#define R_POS 0x1

#define DEBUG_MODE
#define DATA int

typedef unsigned int uint32_t;
typedef unsigned int int32_t;

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
	0 : �ߺ��� ���� �ְų�  �ٸ� ����(?)�� ���Կ� ����
	1 : ���� ���� 
	: �ߺ��� Node�� �������� �ʴ´�. 
		�ߺ�ó���� ��� :
			1. left ptr�� ����
			2. �ش� Node�� ���� linked list�� ����� ����. 
*/  
uint32_t insertAVLNode ( AVL_manage *manage, AVL_Node *newNode );
/*
	position Search Func
	: insert �Լ����� �ش� Node�� ���Խ� �θ� Node�� ã���ִ� �Լ��̴�. 
	- reuturn
		NULL : �̹� �ش� Data�� ���� Node�� �ִ� ��� 
		addr : �ش� Node�� Left or Right�� Node�� �����ϸ� �ȴ�. 
*/
AVL_Node *insert_search ( AVL_manage *manage, AVL_Node *node );
/*
	insert_linkNode
	: new Node�� parent Node�� direction�� ����. 
*/
int32_t insert_linkNode ( AVL_Node *parent, AVL_Node *newNode, uint32_t direction );
/*
	calcHeightDiffer
		- return 
			0 : ������ (  )
			1 : ���� (  ) 
		- rotateInfo
			- 0 : ȸ���� ���ʿ��ϴ�.
			- LL_ROTATE : 1
			- LR_ROTATE : 2
			- RR_ROTATE : 3
			- RL_ROTATE : 4
*/
int32_t calcHeightDiffer ( AVL_manage *manage, AVL_Node *newNode, AVL_Node **rotatePos, uint32_t *rotateInfo);
// pop Node
 /* �̰� ��û ����� ��. ����� �ʿ����� �����غ�����. AVL Tree���� ���� ����� ���� �ִ�. */
// rotation Tree
void rightRotation ( AVL_manage *manage, AVL_Node *eventNode );
void leftRotation ( AVL_manage *manage, AVL_Node *eventNode ); 
uint32_t nodeMax ( AVL_Node *node );
// search 

// print
void printAVL ( AVL_Node *node );

int main ()
{
	AVL_manage *manage = (AVL_manage *) malloc ( sizeof (AVL_manage) );
	initAVL ( manage );
	//
	int i = 0;
	if ( !insertAVLNode(manage, createAVLNode ( 1 ) ) ) 
	{
		printf ("[main] %d func", i);
		return 0;
	}
	if ( !insertAVLNode(manage, createAVLNode ( 2 ) ) ) 
	{
		printf ("[main] %d func", i);
		return 0;
	}
	if ( !insertAVLNode(manage, createAVLNode ( 3 ) ) ) 
	{
		printf ("[main] %d func", i);
		return 0;
	}
	// print
	printAVL ( manage->root_ptr );
	
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
		return NULL; // ������ exit(1)�� �ϴ� ���� �´�. �̰����� ���� �߻��ϴ� ��� ���ܸ� ó���ϴ� ���� ���� �Ұ��� 
	}
	//
	initNode ( tmp );
	//
	tmp->value = value; 
	return tmp;
}
/*
	insert
	0 : �ߺ��� ���� �ְų�  �ٸ� ����(?)�� ���Կ� ����
	1 : ���� ���� 
	: �ߺ��� Node�� �������� �ʴ´�. 
		�ߺ�ó���� ��� :
			1. left ptr�� ����
			2. �ش� Node�� ���� linked list�� ����� ����. 
*/ 
uint32_t insertAVLNode ( AVL_manage *manage, AVL_Node *newNode )
{
	#ifdef DEBUG_MODE
	printf ("------------ [insertAVLNode] newNode(%d) start ------------\n", newNode->value);
	#endif
	/*
		1. ���Ե� ��ġ���� ã�ư���.
		2. ���� ������ �Ѵ�.
		3. �ö󰡸� height���� difference���� �����Ѵ�. 
		4. 
			- ���� �� differ >= 2��� rotation ������ �����Ѵ�. => <rotation �ϸ鼭 �����Ǵ� ���� height���� differ���� �����Ѵ�.>
					: Left Tree or Right Tree�� ���ο� Tree�� Node�� ���� Node�常 ó���ϸ�ȴ�.
					 �� ������ differ >= 2�� ���� ���� Node������ ó�����ָ�� �� ����. ==> Rotation �Լ����� ó���ϵ��� ����. 
			-  ���� �� differ < 2��� ��� �ö󰣴�.
		5. root���� �ö󰣴�.
		--- root�� ratation �Ǵ� ��쵵 �����ؾ� �Ѵ�. 
	*/ 
	// Tree�� ������� Ȯ��.
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
	// new Node�� ���Ե� position ã��.
	AVL_Node *parent = insert_search ( manage, newNode );
	#ifdef DEBUG_MODE
	printf ("insert_search / newNode(%d) / parent = %d \n", newNode->value, parent->value);
	#endif
	if ( parent == NULL )
	{	// �̹� �ߺ��� ���� �ִ� ��� 
		printf ("[insertAVLNode] %d ���� �̹� Tree�� �����մϴ�.\n");
		return 0;
	}
	// parent�� left or right ���� �� newNode linking 
	if ( newNode->value < parent->value )	// parent�� left�� newNode ���� 
	{
		if ( !insert_linkNode ( parent, newNode, LEFT_POS ) )
		{
			printf ("[insertAVLNode] : linking error !\n");
			return 0;
		}
	}
	else if ( newNode->value > parent->value )	// parent�� right�� newNode ���� 
	{
		if ( !insert_linkNode ( parent, newNode, RIGHT_POS ) )
		{
			printf ("[insertAVLNode] : linking error !\n");
			return 0;
		}
	}
	// ���� ��ŷ���� �� �����̴�. ���⼭ ������ؾ���. !  
	printAVL ( manage->root_ptr );
	printf ("\n");
	// height �� differ �� ���
	uint32_t rotateInfo = 0;
	AVL_Node *rotatePos = NULL;
	if ( !calcHeightDiffer ( manage, newNode, &rotatePos, &rotateInfo ) )
	{	// calcHeightDiffer func ������ ���� 
		printf ("[insertAVLNode] : calcHeightDiffer Error!\n");
		exit(1);
	}
	// rotate
	if ( rotateInfo != 0 )
	{	// ȸ���� �ʿ��� ��� 
		switch ( rotateInfo )
		{	// rotate�ϸ鼭 height �� differ ���� �ٲ۴�. 
			case LL_ROTATE :
			{	//	LL ȸ�� 
				// balance���� eventNode���� right ȸ�� 
				#ifdef DEBUG_MOD
				printf ("[insertAVLNode] : LL_ROTATE !\n");
				#endif
				rightRotation( manage, rotatePos );
				break;
			}	
			case LR_ROTATE : 
			{	// LR ȸ�� 
				// [1] balance ���� eventNode�� leftNode�� left ȸ�� 
				// [2] balance ���� eventNode�� right ȸ��
				#ifdef DEBUG_MOD
				printf ("[insertAVLNode] : LR_ROTATE !\n");
				#endif
				leftRotation( manage, rotatePos->left_ptr ); 
				rightRotation( manage, rotatePos );
				break;
			}
			case RR_ROTATE : 
			{	// RR ȸ�� 
				// balance ���� eventNode�� left ȸ��
				#ifdef DEBUG_MOD
				printf ("[insertAVLNode] : RR_ROTATE !\n");
				#endif
				leftRotation( manage, rotatePos ); 
				break;
			}
			case RL_ROTATE : 
			{	// RL ȸ�� 
				// [1] : balance ���� eventNode�� rightNode�� right ȸ�� 
				// [2] : balance ���� eventNode�� leftȸ��
				#ifdef DEBUG_MOD
				printf ("[insertAVLNode] : RL_ROTATE !\n");
				#endif
				rightRotation( manage, rotatePos->right_ptr );
				leftRotation( manage, rotatePos ); 
				break;
			}
		}
		// Left Tree Height and Right Tree Height ��
		if ( rotatePos->leftTreeHeight != rotatePos->rightTreeHeight )
		{
			printf ("[insertAVLNode] : after Rotate, balance Error!\n");
			exit (1);
		} 
	}
	// ���� ���� 
	return 1;
	#ifdef DEBUG_MODE
	printf ("------------ [insertAVLNode] newNode(%d) end ------------\n", newNode->value);
	#endif
}
/*
	position Search Func
	: insert �Լ����� �ش� Node�� ���Խ� �θ� Node�� ã���ִ� �Լ��̴�. 
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
		{ // �ߺ� ! 
			return NULL;
		}
	}
}
/*
	insert_linkNode
	: new Node�� parent Node�� direction�� ����. 
*/
int32_t insert_linkNode ( AVL_Node *parent, AVL_Node *newNode, uint32_t direction )
{
	if ( direction == RIGHT_POS )
	{
		parent->right_ptr = newNode;
	}
	else if ( direction == LEFT_POS )
	{
		parent->left_ptr = newNode;
	}
	else 
		return 0;	// ������ 
	return 1;	// ���� 
	
}
/*
	calcHeightDiffer
		- return 
			0 : ������ (  )
			1 : ���� (  ) 
		- rotateInfo
			- 0 : ȸ���� ���ʿ��ϴ�.
			- LL_ROTATE : 1
			- LR_ROTATE : 2
			- RR_ROTATE : 3
			- RL_ROTATE : 4
		- height ó��
			: AVL�� Ư���� Ȯ���ؾ��Ѵ�.
			- == 0 break
			newNode�� �߰��Ͽ� newNode�� parent node���� newNode�� �߰��� ������ subtree height���� �����ϴ� ��
			�� ��, diff���� 0�̸� newNode->parent_ptr->parent_ptr�� subtree height���� �������� �ʾƵ� �ȴ�.
			�ֳ��ϸ� �̹� �� ���̷� ���Ǿ� �ֱ� �����̴�.
			- >= 2
			newNode �߰��� �� ������ ���� �����̴�.
			�̶� ȸ���� �ϸ� diff >= 2�� �߻��� node�� eventNode��� �Ѵٸ� 
			eventNode->parent_ptr�� sub Tree height���� ȸ���� �ϱ� ������ ������ ���� ������ �ʿ�������
			ȸ�����ϸ� eventNode->parent_ptr�� sub tree height���� �������� �ʾƵ� ��Ȯ�� ���� ������. 
*/
int32_t calcHeightDiffer ( AVL_manage *manage, AVL_Node *newNode, AVL_Node **rotatePos , uint32_t *rotateInfo)
{
	// Tree�� ���̰� 2�̻��� ���� ���� 
	AVL_Node *tmp = newNode;
	AVL_Node *tmp1 = NULL;
	uint32_t prevPath = 0;
	uint32_t prevHeight = 0;
	//
	tmp1 = tmp;
	tmp = newNode->parent_ptr;
	int32_t res = 0;
	while ( ( tmp != NULL ) && ( manage->root_ptr != tmp1 ) )	
	{	// root�̻����� ���� �����. 
		if ( tmp->left_ptr == tmp1 )
		{	// ������ ��ġ�� Left Tree 
			prevPath << 2;
			prevPath = prevPath | L_POS;
			// calc height 
			/* �̰� ��ƴ�. ���� ����� ���� ���� ����ؼ� ���ϸ� �ö󰥱�? �׷��ٰ� ���̰��� �������� ������ differ�� �߻����� ������*/
			/* height���̴� ���Ե� ��ġ���� �ö󰡴ٺ��� ���̰� �������ٸ� loop�� �����ص� ���. ������ 1�̶� ���̰� ���ٸ� ��� �ö󰡾���. */
			tmp->leftTreeHeight = prevHeight + 1;
		}
		else if ( tmp->right_ptr == tmp1 )
		{	// ������ ��ġ�� Right Tree 
			prevPath << 2;
			prevPath = prevPath | R_POS;
			// calc height 
			tmp->rightTreeHeight = prevHeight + 1;
		}
		// calc differ
		res = tmp->leftTreeHeight - tmp->rightTreeHeight;
		tmp->differOfHeight = ( ( res < 0 ) ? -1 * res : res );
		if ( tmp->differOfHeight == 0 )
		{	// �ش� ����Ʈ���� ���� ����Ʈ���̴�. (ȸ���� �ʿ����.)
			// tmp�� parent node�� �� �ʿ䰡 ���� ������ �̹� ������ tmp->parent�� �ش� ������ height�� �ֽ�ȭ�� �����̱� �����̴�. 
			 break;
		} 
		else if ( tmp->differOfHeight >=2 ) 
		{
			*rotatePos = tmp;
			break;
		}
		// ���� �ö󰣴�.
		/* prevHeight++
			�ö󰡴� ���� �߰��� ������ Sub Tree�� ũ�Ⱑ �������� �� Ŀ���� �����̴�.
			�׷��� parent node�� �ش� ������ sub tree�� Ŀ����. �� Ŀ�� ���� ������ diff�� 0�� ���� ��Ŀ������
			1�� ���� parent tree�� �ش� ���� sub tree height�� �����ؾ� �Ѵ�. 
		*/  
		prevHeight++; 
		tmp1 = tmp;
		tmp = tmp->parent_ptr; 
	}
	// root���� ���� ���, heightȮ�� �ʿ�
	if ( ( tmp == NULL ) && ( manage->root_ptr == tmp1 ) ) 
	{	// root���� �ö���鼭 differ >= 2�� ���� ���� ��Ȳ������ subtree�� height�� Ŀ��. 
		manage->numberOfNode++;
		// AVL Tree Height Info �ֽ�ȭ
		// root�� right Sub Tree�� �� ���� ��� 
		if ( tmp1->rightTreeHeight > tmp1->leftTreeHeight ) 
			manage->heightOfTree = tmp1->rightTreeHeight;
		else 
			// root�� left Sub Tree�� �� ���� ���. 
			manage->heightOfTree = tmp1->leftTreeHeight;
	}
	// [�̰� ����� �����ϴ��� Ȯ�� �ʿ�]
	if ( *rotatePos != NULL )
	{	// ȸ���� �ʿ��� ��� ����, ������ ������ ������ LR, LL, RR, RL�� �´��� Ȯ���ʿ� �Ƹ� ���� ���̴�. 
		if ( ( prevPath & L_POS ) > 0 )
		{	// ������ Left Tree���� �� ��� 
			if ( ( ( prevPath >> 2 ) & L_POS ) > 0 )
			{	// LL
				*rotateInfo = LL_ROTATE;
				return 1; 
			}
			else if ( ( ( prevPath >> 2 ) & R_POS ) > 0 )
			{	// LR
				*rotateInfo = LR_ROTATE;
				return 1; 
			}
		}
		else if ( ( prevPath & R_POS ) > 0 )
		{	//  ������ Right Tree���� �� ��� 
			if ( ( ( prevPath >> 2 ) & L_POS ) > 0 )
			{	// RL
				*rotateInfo = RL_ROTATE;
				return 1;
			}
			else if ( ( ( prevPath >> 2 ) & R_POS ) > 0 )
			{	// RR
				*rotateInfo = RR_ROTATE;
				return 1;
			}
		}
	}
	else
	{	// ȸ���� �ʿ���� ��� 
		*rotateInfo = 0;
		return 1;
	} 
}
// rotation Tree
/* ���� ����� Ȯ�� */
void rightRotation ( AVL_manage *manage, AVL_Node *eventNode )
{
	#ifdef DEBUG_MODE
	printf (" Right Rotation !\n");
	#endif
	// eventNode�� root���� Ȯ���ؾ� �Ѵ�.
	AVL_Node *node2 = eventNode->left_ptr;
	AVL_Node *moveNode = eventNode->left_ptr->right_ptr;
	AVL_Node *parentNode = eventNode->parent_ptr;	// NULL => eventNode = root
	// [1] moveNode
	if ( moveNode != NULL )
	{
		eventNode->left_ptr = moveNode;
		moveNode->parent_ptr = eventNode;	
	}
	// [2] node2
	eventNode->parent_ptr = node2;
	node2->right_ptr = eventNode;
	// [3] parent
	if ( ( manage->root_ptr ==  eventNode) && ( parentNode == NULL ) )
	{	// root �� ��� 
		manage->root_ptr = node2; 
		node2->parent_ptr = NULL;
	}
	else
	{	// root�� �ƴ� ��� 
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
		{	// ���� ��Ȳ ���� �Ұ��� 
			printf ("[rightRotation] : parent Node Linking Error !\n ");
			return;
		}
	}
	// eventNode height
	uint32_t eventHeight = nodeMax ( eventNode );
	// node2 Height
	node2->rightTreeHeight = eventHeight + 1;
	// node2�� left and right Sub Tree�� Height�� ���� ���� indesertAVLNode���� �� 
}
/* ���� ����� Ȯ�� */
void leftRotation ( AVL_manage *manage, AVL_Node *eventNode )
{
	#ifdef DEBUG_MODE
	printf (" Left Rotation !\n");
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
	// [2] node2
	node2->left_ptr = eventNode;
	eventNode->parent_ptr = node2;
	// [3] parent
	if ( ( manage->root_ptr == eventNode ) && ( parentNode == NULL ) )
	{	// eventNode�� root�� ���
		manage->root_ptr = node2; 
		node2->parent_ptr = NULL;
	}
	else
	{	// root�� �ƴ� ��� 
		if ( parentNode->leftTreeHeight == eventNode )
		{	// eventNode�� parent�� Left Sub Tree 
			node2->parent_ptr = parentNode;
			parentNode->leftTreeHeight = node2;
		}
		else if ( parentNode->rightTreeHeight == eventNode )
		{	// eventNode�� parent�� Right Sub Tree 
			node2->parent_ptr = parentNode;
			parentNode->rightTreeHeight = node2;
		}
		else
		{	// ���ܰ� ���� ���� �� ����. 
			printf ("[leftRotation] : parent Node Linking Error !\n");
			return;
		}
	}
	// height
	uint32_t eventHeight = nodeMax ( eventNode );
	node2->leftTreeHeight = eventHeight + 1;
}
// �ش� node�� left and right sub tree�� ũ�� �� 
/* ���� ����� Ȯ�� */
uint32_t nodeMax ( AVL_Node *node )
{
	uint32_t res = 0;
	// node right height
	AVL_Node *right = node->right_ptr;
	if ( right != NULL )
	{
		if ( right->rightTreeHeight > right->leftTreeHeight )
		{
			node->rightTreeHeight = right->rightTreeHeight + 1;
			res = ( res > node->rightTreeHeight ? res : node->rightTreeHeight );
		}
		else
		{
			node->rightTreeHeight = right->leftTreeHeight + 1;
			res = ( res > node->rightTreeHeight ? res : node->rightTreeHeight );
		}
	}
	else
	{	// NULL �� ��� 
		node->rightTreeHeight = 0;
	}
	// node left height
	AVL_Node *left = node->left_ptr;
	if ( left != NULL )
	{
		if ( left->rightTreeHeight > left->leftTreeHeight )
		{
			node->leftTreeHeight = left->rightTreeHeight;
			res = ( res > node->leftTreeHeight ? res : node->leftTreeHeight );
		}
		else
		{
			node->leftTreeHeight = left->leftTreeHeight;
			res = ( res > node->leftTreeHeight ? res : node->leftTreeHeight );
		}
	}
	else
	{	// NULL �� ��� 
		node->leftTreeHeight = 0;
	}
	return res; 
}
// print
void printAVL ( AVL_Node *node )
{
	if ( node->left_ptr != NULL ) printAVL ( node->left_ptr );
	printf (" %d[LH : %d, RH : %d]", node->value, node->leftTreeHeight, node->rightTreeHeight);
	if ( node->right_ptr != NULL ) printAVL ( node->right_ptr );
}

















