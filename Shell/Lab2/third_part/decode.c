#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

# include <unistd.h>
# include <fcntl.h>

#define MAX_SYMBOLS 255
#define MAX_LEN     300
#define BYTE_SIZE    8


struct tnode
{
    struct  tnode* left; /*used when in tree*/
    struct  tnode*right; /*used when in tree*/  
    int     isleaf;
    char     symbol;
};

struct code
{
	int		symbol;
	char	strcode[MAX_LEN];
};

/*global variables*/
struct tnode* root=NULL; /*tree of symbols*/

/*
    @function   talloc
    @desc       allocates new node 
*/

void freetree(struct tnode* root);

void	fatal(const char *err) {
	fprintf(stderr, err);
	freetree(root);
	abort();
}

struct tnode* talloc()
{
    struct tnode* p=(struct tnode*)malloc(sizeof(struct tnode));
    if(p!=NULL)
    {
        p->left=p->right=NULL;
        p->symbol=0;
	p->isleaf=0;
    }
    return p;
}

/*
    @function build_tree
    @desc     builds the symbol tree given the list of symbols and code.h
	NOTE: alters the global variable root that has already been allocated in main
*/
void build_tree(FILE *fin)
{
	int	symbol;
	char	strcode[MAX_LEN];
	int		items_read;
	int		i,len;
	struct	tnode* curr=NULL;

	while(!feof(fin))
	{
		memset(strcode, 0, sizeof(strcode));
		items_read=fscanf(fin,"%d %s\n", &symbol,strcode);
		if(items_read!=2) break;
		curr=root;
		len=strlen(strcode);

		for(i = 0; i < len; i++)
		{
			/*TODO: create the tree as you go*/
			if (strcode[i] == '1') {
				if (curr->right == NULL && (curr->right = talloc()) == NULL) {
					fatal("malloc() faild\n");
				}
				curr = curr->right;
			} else if (strcode[i] == '0') {
				if (curr->left == NULL && (curr->left = talloc()) == NULL) {
					fatal("malloc() faild\n");
				}
				curr = curr->left;
			}
		}
		/*assign code*/
		curr->isleaf=1;
		curr->symbol=symbol;
		printf("inserted symbol:%c\n",symbol);
	}
}

/*
	function decode
*/
#ifdef WRITE_RAW_BITS
void decode(int fdin,int fdout)
{
	char c;
	struct tnode*	curr=root;
	char		buff[MAX_LEN];
	int		i = 0;
	int		j = BYTE_SIZE;

	memset(buff, 0, sizeof(buff));
	while(read(fdin, &c, sizeof(c)))
	{
		/*TODO:
			traverse the tree
			print the symbols only if you encounter a leaf node
		*/
		j = BYTE_SIZE;
		while (j > 0) {
			--j;
			buff[i] = ((c >> j) & 0x1) + '0';
			if (buff[i] == '1') { 
				curr = curr->right;
			} else {
				curr = curr->left;
			}
			++i;
			if (curr->isleaf) {
				write(fdout, &curr->symbol, sizeof(curr->symbol));
				memset(buff, 0, sizeof(buff));
				i = 0;
				curr = root;
			}
		}
	}
}
#else
void decode(int fdin, int fdout)
{
	char c;
	struct tnode* curr=root;
	while(read(fdin, &c, sizeof(c)))
	{
		/*TODO:
			traverse the tree
			print the symbols only if you encounter a leaf node
		*/
		if (c == '1') { 
			curr = curr->right;
		} else {
			curr = curr->left;
		}
		if (curr->isleaf) {
			write(fdout, &curr->symbol, sizeof(curr->symbol));
			curr = root;
		}
	}
}
#endif
/*
	@function freetree
	@desc	  cleans up resources for tree
*/

void freetree(struct tnode* root)
{
	if(root==NULL)
		return;
	freetree(root->right);
	freetree(root->left);
	free(root);
}
int main()
{
#ifdef WRITE_RAW_BITS
	const char* IN_FILE="../encoded";
#else
	const char* IN_FILE="../encoded.txt";
#endif
	const char* CODE_FILE="../code.txt";
	const char* OUT_FILE="../decoded.txt";
	int fdout;
	int fdin;
	FILE	*fin;
	/*allocate root*/
	root = talloc();
	fin = fopen(CODE_FILE, "r");
	/*build tree*/
	build_tree(fin);
	fclose(fin);

	/*decode*/
	fdin = open(IN_FILE, O_RDONLY);
	fdout = open(OUT_FILE,O_WRONLY | O_CREAT);
	decode(fdin, fdout);
	close(fdin);
	close(fdout);
	/*cleanup*/
	freetree(root);
	getchar();
	return 0;
}

