/*
	Name:		Farhan Jiva
	Date:		03-06-08
	Course:		CSCI 2720
	Purpose:		Introduction to implementing algorithms
				using the tree data structure and traversing
				the tree.
	Filename:	t3solver.c
*/
/*
	Description:
	This program generates the game tree of the game tic-tac-toe given an initial board configuration.
	This program was written in C and tested on atlas (solaris).

	How to compile:
	A makefile has been included with the package.
	Type "make" while in the current working directory of the package.

	How to run:
	Type "./t3solver " along with the initial board configuration of your choice.
	The program will error and return out if the initial board configuration is invalid.
	An example of a valid command: "./t3solver x------o-"
	If you wish to generate the entire game tree, starting with a blank board,
		simply enter "./t3solver" with no arguments or "./t3solver ---------"
	
	Notes:
	Tic-tac-toe board must not contain any spaces (argv[] will see it as three arguments otherwise)
	Must use dashes (-) instead of asterisks (*) for blanks:
		gcc (or bash) has an issue with asterisks as command-line arguments
	Example of a correct input argument: x------o-
	If you wish to generate the entire game tree, you can enter
		./t3solver ---------
	or with no arguments
		./t3solver
	This input syntax also applies to option 5 in the menu.
	For part 2 of the project, I have chosen two methods of implementation:
		(a) I generate an entirely new game tree based on the new input serialized board position
			This input is accepted in the switch statement (case 5) in the main() function
		(b) I use the current game tree
			This was initially created
	I have chosen to implement two methods because part 2 was ambiguous. I was not sure whether or not
		I was to create a whole new game tree or use the original one - because of this, I have decided
		to cover both grounds. Case 5 in the switch statement comes in two flavors, one for part (a)
		and another for part (b)
*/

#include <stdio.h> /* printf() scanf() */
#include <string.h> /* strncpy() strlen() */
#include <stdlib.h> /* malloc() free() rand() srand() */
#include <time.h> /* time() */

typedef struct treenode
{
	char board[9]; /* the string which contains the current board configuration */
	struct treenode *children[9]; /* array of pointers to the children, 9 is the max */
	int nc; /* the actual "n"umber of "c"hildren this node has */
} tnode;

typedef struct listnode /* queue implementation for level order traversal */
{
	struct treenode *node; /* pointer to the tree node */
	struct listnode *next; /* pointer to the next list node */
} lnode;

tnode root; /* root of the tree */
tnode troot; /* root of the "t"race tree */
lnode *front; /* pointer to the front of the queue */
lnode *back; /* pointer to the back of the queue */
int nodes; /* total nodes in the game tree */
int tnodes; /* total "t"race nodes in the game tree */
int listnodes = 0; /* the number of nodes in the list */

/* function that counts the number of Xs on the board */
int countx(char *board)
{
	int i, count = 0;
	for(i = 0; i <= 8; i++)
	{
		if((board[i] == 'x') || (board[i] == 'X'))
		{
			count++;
		}
	}
	return count;
}

/* function that counts the number of Os on the board */
int counto(char *board)
{
	int i, count = 0;
	for(i = 0; i <= 8; i++)
	{
		if((board[i] == 'o') || (board[i] == 'O'))
		{
			count++;
		}
	}
	return count;
}

/* function that counts the number of -s on the board */
int countempty(char *board)
{
	int i, count = 0;
	for(i = 0; i <= 8; i++)
	{
		if(board[i] == '-')
		{
			count++;
		}
	}
	return count;
}

/* function that prints the 2-D board */
void print2d(char *board)
{
	int i;
	for(i = 0; i <= 8; i++)
	{
		if(i % 3 == 0)
		{
			printf("\n");
		}
		printf("%c", board[i]);
	}
	printf("\n\n");
}

/* function that prints the serialized board */
void prints(char *board)
{
	printf("%s\n", board);
}

/* function that returns 1 if x's turn, returns 0 if o's turn */
int nextturn(char *board)
{
	if(countx(board) > counto(board))
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

/* function which checks whether the initial board is in a final state or playable */
/* returns -1 on error, 0 if ok */
int checki(char *board)
{
	if(strlen(board) != 9) /* checks length */
	{
		return -1;
	}
	
	int i;
	for(i = 0; i <= 8; i++) /* checks for valid characters */
	{
		if((board[i] != 'x') && (board[i] != 'X') && (board[i] != 'o') && (board[i] != 'O') && (board[i] != '-'))
		{
			return -1;
		}
	}
	
	int diff = countx(board) - counto(board);
	switch(diff) /* ensures the number of xs is only 0 or 1 more than number of os */
	{
		case 0:
			return 0;
			break;
		case 1:
			return 0;
			break;
		default:
			return -1;
	}
	
	return 0;
}

/* function that checks for winners */
/* returns -1 for still playable, 0 if o is winner, 1 if x is winner, 2 if draw */
int check(char *board)
{
	/* the following conditional statements check to see if game has been won for x */
	if((board[0] == 'x' || board[0] == 'X') && (board[1] == 'x' || board[1] == 'X') && (board[2] == 'x' || board[2] == 'X'))
		return 1;
	if((board[3] == 'x' || board[3] == 'X') && (board[4] == 'x' || board[4] == 'X') && (board[5] == 'x' || board[5] == 'X'))
		return 1;
	if((board[6] == 'x' || board[6] == 'X') && (board[7] == 'x' || board[7] == 'X') && (board[8] == 'x' || board[8] == 'X'))
		return 1;
	if((board[0] == 'x' || board[0] == 'X') && (board[3] == 'x' || board[3] == 'X') && (board[6] == 'x' || board[6] == 'X'))
		return 1;
	if((board[1] == 'x' || board[1] == 'X') && (board[4] == 'x' || board[4] == 'X') && (board[7] == 'x' || board[7] == 'X'))
		return 1;
	if((board[2] == 'x' || board[2] == 'X') && (board[5] == 'x' || board[5] == 'X') && (board[8] == 'x' || board[8] == 'X'))
		return 1;
	if((board[0] == 'x' || board[0] == 'X') && (board[4] == 'x' || board[4] == 'X') && (board[8] == 'x' || board[8] == 'X'))
		return 1;
	if((board[2] == 'x' || board[2] == 'X') && (board[4] == 'x' || board[4] == 'X') && (board[6] == 'x' || board[6] == 'X'))
		return 1;
	
	/* the following conditional statements check to see if game has been won for o */
	if((board[0] == 'o' || board[0] == 'O') && (board[1] == 'o' || board[1] == 'O') && (board[2] == 'o' || board[2] == 'O'))
		return 0;
	if((board[3] == 'o' || board[3] == 'O') && (board[4] == 'o' || board[4] == 'O') && (board[5] == 'o' || board[5] == 'O'))
		return 0;
	if((board[6] == 'o' || board[6] == 'O') && (board[7] == 'o' || board[7] == 'O') && (board[8] == 'o' || board[8] == 'O'))
		return 0;
	if((board[0] == 'o' || board[0] == 'O') && (board[3] == 'o' || board[3] == 'O') && (board[6] == 'o' || board[6] == 'O'))
		return 0;
	if((board[1] == 'o' || board[1] == 'O') && (board[4] == 'o' || board[4] == 'O') && (board[7] == 'o' || board[7] == 'O'))
		return 0;
	if((board[2] == 'o' || board[2] == 'O') && (board[5] == 'o' || board[5] == 'O') && (board[8] == 'o' || board[8] == 'O'))
		return 0;
	if((board[0] == 'o' || board[0] == 'O') && (board[4] == 'o' || board[4] == 'O') && (board[8] == 'o' || board[8] == 'O'))
		return 0;
	if((board[2] == 'o' || board[2] == 'O') && (board[4] == 'o' || board[4] == 'O') && (board[6] == 'o' || board[6] == 'O'))
		return 0;
	
	/* the following for-loop checks if any dashes exist, if so, game is still playable, since above conditions have not been satisfied */
	int i;
	for(i = 0; i <= 8; i++)
	{
		if(board[i] == '-')
		{
			return -1;
		}
	}
		
	return 2; /* if none of the above passed, the game is a draw */
}

/* function which configs the root node */
void configroot(char *board)
{
	strncpy(root.board, board, 9);
	root.nc = countempty(root.board);
	nodes++;
}

/* function which configs the troot node */
void configtroot(char *board)
{
	strncpy(troot.board, board, 9);
	troot.nc = countempty(troot.board);
	tnodes++;
}

/* recursive function which makes the game tree - populates root */
/* takes a pointer to a node, along with the number of children that node has */
void populateroot(tnode *node, int nc)
{
	if(nc == 0 || check(node->board) == 0 || check(node->board) == 1 || check(node->board) == 2) // or == NULL, check ok? kthxbye
	{
		node->nc = 0; /* explicitly setting nc to zero because base case has been achieved */
		return;
	}
	else
	{
		int next = nextturn(node->board);
		int i, j = -1;
		for(i = 0; i < nc; i++) /* iterating through children */
		{
			tnode *new = (tnode *)malloc(sizeof(tnode));
			node->children[i] = new;
			nodes++;
			strncpy(new->board, node->board, 9);
			new->nc = nc - 1;
			for(j = j + 1; j <= 8; j++) /* iterating through board */
			{
				if(new->board[j] == '-')
				{
					switch(next)
					{
						case 0:
							new->board[j] = 'o';
							populateroot(new, new->nc); /* recursive call */
							break;
						case 1:
							new->board[j] = 'x';
							populateroot(new, new->nc); /* recursive call */
							break;
					}
					break; /* after finding first blank, continues with next child, j is not reinitialized */
				}
			}
		}
	}
}

/* recursive function which makes the game tree - populates troot */
/* takes a pointer to a node, along with the number of children that node has */
void populatetroot(tnode *node, int nc)
{
	if(nc == 0 || check(node->board) == 0 || check(node->board) == 1 || check(node->board) == 2) // or == NULL, check ok? kthxbye
	{
		node->nc = 0; /* explicitly setting nc to zero because base case has been achieved */
		return;
	}
	else
	{
		int next = nextturn(node->board);
		int i, j = -1;
		for(i = 0; i < nc; i++) /* iterating through children */
		{
			tnode *new = (tnode *)malloc(sizeof(tnode));
			node->children[i] = new;
			tnodes++;
			strncpy(new->board, node->board, 9);
			new->nc = nc - 1;
			for(j = j + 1; j <= 8; j++) /* iterating through board */
			{
				if(new->board[j] == '-')
				{
					switch(next)
					{
						case 0:
							new->board[j] = 'o';
							populatetroot(new, new->nc); /* recursive call */
							break;
						case 1:
							new->board[j] = 'x';
							populatetroot(new, new->nc); /* recursive call */
							break;
					}
					break; /* after finding first blank, continues with next child, j is not reinitialized */
				}
			}
		}
	}
}

/* recursive function which performs a preorder traversal */
void preorder(tnode *node)
{
	prints(node->board);
	int i;
	for(i = 0; i < node->nc; i++)
	{
		preorder(node->children[i]); /* recursive call */
	}
}

/* recursive function which performs a postorder traversal */
void postorder(tnode *node)
{
	int i;
	for(i = 0; i < node->nc; i++)
	{
		postorder(node->children[i]); /* recursive call */
	}
	prints(node->board);
}

/* list function which adds a list node to the queue */
void enqueue(tnode *node)
{
	if(listnodes == 0)
	{
		lnode *new = (lnode *)malloc(sizeof(lnode));
		new->node = node;
		front = new;
		back = new;
		listnodes++;
	}
	else
	{
		lnode *new = (lnode *)malloc(sizeof(lnode));
		new->node = node;
		back->next = new;
		back = new;
		listnodes++;
	}
}

/* list function which removes a list node from the list, returns the tnode pointed by the lnode */
tnode * dequeue()
{
	tnode *toreturn = front->node;
	lnode *tofree = front;
	front = front->next;
	listnodes--;
	free(tofree);
	if(listnodes == 0) /* if list becomes empty, nullify pointers */
	{
		front = NULL;
		back = NULL;
	}
	return toreturn;
}

/* function which performs a levelorder traversal */
void levelorder(tnode *node)
{
	tnode *rnode; /* pointer to the "r"eturned node */
	
	enqueue(node);
	while(listnodes != 0)
	{
		rnode = dequeue();
		prints(rnode->board);
		int i;
		for(i = 0; i < rnode->nc; i++)
		{
			if(rnode->children[i] != NULL)
			{
				enqueue(rnode->children[i]);
			}
		}
	}
}

/* recursive function which traces a random path from node to a leaf */
void trace(tnode *node)
{
	if(node->nc == 0)
	{
		prints(node->board);
		return;
	}
	else
	{
		prints(node->board);
		srand(time(NULL)); /* initializes random seed */
		int child = rand() % node->nc; /* randomly choose a child between 0 and node->nc */
		trace(node->children[child]); /* recursive call */
	}
}

/* recursive function which performs a postorder free() call */
/* this was implemented to avoid memory leaks, since multiple trace trees could be created */
void deletetree(tnode *node)
{
	int i;
	for(i = 0; i < node->nc; i++)
	{
		deletetree(node->children[i]); /* recursive call */
	}
	free(node);
	tnodes--;
}

int main(int argc, char *argv[])
{
	if(argc > 2)
	{
		printf("usage: t3solver <tic-tac-toe board>\n");
		return -1;
	}
	
	if(argc == 1) /* user wants entire game tree */
	{
		char *tempboard = "---------";
		configroot(tempboard);
		populateroot(&root, root.nc);
	}
	else /* user specified the initial board configuration */
	{
		if(checki(argv[1]) == -1)
		{
			printf("Error: invalid initial board configuration\n");
			return -1;
		}
		configroot(argv[1]);
		populateroot(&root, root.nc);
	}
	
	int choice = 0;
	char board[9];
	while(choice != 6) /* a menu */
	{
		printf("What do you want to do?\n\n");
		printf("1. Do a Preorder traversal\n");
		printf("2. Do a Postorder traversal\n");
		printf("3. Do a Level order traversal\n");
		printf("4. View the total number of nodes\n");
		printf("5. Trace a path to a leaf\n");
		printf("6. Exit\n");
		
		scanf("%d", &choice);
		
		switch(choice)
		{
			case 1:
				preorder(&root);
				break;
			case 2:
				postorder(&root);
				break;
			case 3:
				levelorder(&root);
				break;
			case 4:
				printf("Total number of nodes: %d\n\n", nodes);
				break;
			case 5:
				printf("Enter the board which you would like to trace to a leaf: ");
				scanf("%s", board);
				if(checki(board) == -1)
				{
					printf("Error: invalid board configuration\n");
					break;
				}
				configtroot(board);
				populatetroot(&troot, troot.nc);
				trace(&troot);
				deletetree(&troot);
				break;
			/*
			case 5:
				trace(&root);
				break;
			*/
			case 6:
				choice = 6;
				break;
			default:
				printf("Sorry, %d is not a valid option\n\n", choice);
		}
	}
	
	return 0;
}