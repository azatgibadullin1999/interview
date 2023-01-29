#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX_SYMBOLS 255
#define MAX_LEN     300
#define BYTE_SIZE    8

struct tnode
{
    struct  tnode* left; /*used when in tree*/
    struct  tnode*right; /*used when in tree*/  
    struct  tnode*parent;/*used when in tree*/
    struct  tnode* next; /*used when in list*/ 
    float   freq;
    int     isleaf;
    char    symbol;
};


/*global variables*/
char code[MAX_SYMBOLS][MAX_LEN];
struct tnode* root=NULL; /*tree of symbols*/
struct tnode* qhead=NULL; /*list of current symbols*/
struct cnode* chead=NULL;/*list of code*/
unsigned long   all_meted_char[MAX_SYMBOLS];
unsigned long   num_of_chars;

/*
    @function   talloc
    @desc       allocates new node 
*/
struct tnode* talloc(int symbol,float freq)
{
    struct tnode* p=(struct tnode*)malloc(sizeof(struct tnode));
    if(p!=NULL)
    {
        p->left=p->right=p->parent=p->next=NULL;
        p->symbol=symbol;
        p->freq=freq;
		p->isleaf=1;
    }
    return p;
}

/*
    @function display_tnode_list
    @desc     displays the list of tnodes during code construction
*/
void pq_display(struct tnode* head)
{
    struct tnode* p=NULL;
    printf("list:");
    for(p=head;p!=NULL;p=p->next)
    {
        printf("(%c,%f) ",p->symbol,p->freq);
    }
    printf("\n");
}

/*
    @function pq_insert
    @desc     inserts an element into the priority queue
    NOTE:     makes use of global variable qhead
*/
void    pq_insert(struct tnode* p)
{
    struct tnode* curr=qhead;
    struct tnode* prev=NULL;
   if(qhead==NULL) /*qhead is null*/
   {
   		/*TODO: write code to insert when queue is empty*/
        qhead = p;
        return;
   }
   /*TODO:  write code to find correct position to insert*/
   while (curr)  {
        if (curr->freq > p->freq) {
            break;
        }
        prev = curr;
        curr = curr->next;
   }
   if(curr==qhead)
   {
   	 	/*TODO: write code to insert before the current start*/
        p->next = qhead;
        qhead = p;
   }
   else /*insert between prev and next*/
   {
	 	/*TODO: write code to insert in between*/
        p->next = prev->next;
        prev->next = p;
   }
}

/*
    @function pq_pop
    @desc     removes the first element
    NOTE:     makes use of global variable qhead
*/

struct tnode* pq_pop()
{
    struct tnode* p=qhead;
    /*TODO: write code to remove front of the queue*/
    if (!p) {
        return p;
    }
    qhead = qhead->next;
	printf("popped:%c,%f\n",p->symbol,p->freq);
    return p;
}

/*
	@function build_code
	@desc     generates the string codes given the tree
	NOTE: makes use of the global variable root
*/
void generate_code(struct tnode* root,int depth)
{
	int symbol;
	int len; /*length of code*/
	if(root->isleaf)
	{
		symbol = root->symbol;
		len = depth;
		/*start backwards*/
		code[symbol][len];
        memset(code[symbol], 0, sizeof(code[symbol]));
		/*
			TODO: follow parent pointer to the top
			to generate the code string
		*/
        while (--len >= 0) {
            if (root->parent->right == root) {
                code[symbol][len] = '1';
            } else {
                code[symbol][len] = '0';
            }
            root = root->parent;
        }
		printf("built code:%c,%s\n",symbol,code[symbol]);
	}
	else
	{
		generate_code(root->left,depth+1);
		generate_code(root->right,depth+1);
	}
	
}

/*
	@func	dump_code
	@desc	output code file
*/
void dump_code(FILE* fp)
{
	int i=0;
	for(i=0;i<MAX_SYMBOLS;i++)
	{
		if(code[i][0]) /*non empty*/
			fprintf(fp,"%d %s\n",i,code[i]);
	}
}

/*
	@func	encode
	@desc	outputs compressed stream
*/
/*
    Я пытался закодировать и записать текст побитово,
    но к сожалению при дикодировании, в еонце файла лишние символы
*/
#ifdef RAW_BITS

int encode_to_bitstream(char symbol, char *str, size_t str_size) {
    static int  i = 0;
    static int  j = BYTE_SIZE;
    static int  l = 0;
    size_t  num_of_wrote_bits = 0;

    while (i < str_size && code[symbol][l]) {
        while (j > 0 && code[symbol][l]) {
            --j;
            str[i] = str[i] | ((code[symbol][l] - '0') << j);
            ++l;
            ++num_of_wrote_bits;
        }
        if (j == 0) {
            j = BYTE_SIZE;
            ++i;
        }
    }
    if (i < str_size) {
        l = 0;
    } else {
        i = 0;
    }
    return num_of_wrote_bits;
}

void encode(int fdin, int fdout)
{
    char    char_fdin;
    char    buff_fdout[MAX_LEN];
    size_t  num_of_wrote_bits = 0;
    const size_t  max_bits = sizeof(buff_fdout) * 8;
    int     ret_value;

    memset(buff_fdout, 0, sizeof(buff_fdout));
	while (ret_value = read(fdin, &char_fdin, sizeof(char_fdin))) {
        if (isprint(char_fdin) || isspace(char_fdin)) {
            num_of_wrote_bits += encode_to_bitstream(char_fdin, buff_fdout, sizeof(buff_fdout));
            if (num_of_wrote_bits == max_bits) {
                write(fdout, buff_fdout, sizeof(buff_fdout));
                memset(buff_fdout, 0, sizeof(buff_fdout));
                num_of_wrote_bits = 0;
                num_of_wrote_bits += encode_to_bitstream(char_fdin, buff_fdout, sizeof(buff_fdout));
            }
        }
	}
    write(fdout, buff_fdout, num_of_wrote_bits / 8);
}

#else


int encode_to_bytestream(char symbol, char *str, size_t str_size) {
    size_t  i = 0;
    
    while (i < str_size && code[symbol][i]) {
        str[i] = code[symbol][i];
        ++i;
    }

    return 1;
}

void encode(int fdin, int fdout)
{
    char    char_fdin;
    char    buff_fdout[MAX_LEN];
    size_t  buff_fdout_len;

    memset(buff_fdout, 0, sizeof(buff_fdout));
	while (read(fdin, &char_fdin, sizeof(char_fdin))) {
       if (isprint(char_fdin) || isspace(char_fdin)) {
            encode_to_bytestream(char_fdin, buff_fdout, sizeof(buff_fdout));
            buff_fdout_len = strlen(buff_fdout);
            if (write(fdout, buff_fdout, buff_fdout_len) == -1) {
                printf("wirte() failed, errno = %d\n", errno);
                abort();
            }
            memset(buff_fdout, 0, buff_fdout_len);
        }
    }
}

#endif

void char_coutner() {
    FILE    *file = fopen("../book.txt", "r");
    size_t  buff_size = 1024;
    char    buff[buff_size];

    num_of_chars = 0;
    memset(all_meted_char, 0, sizeof(all_meted_char));
    while (fgets(buff, buff_size, file)) {
        for (size_t i = 0; buff[i] && buff[i]; ++i) {
            if (isprint(buff[i]) || isspace(buff[i])) {
                ++all_meted_char[(size_t)buff[i]];
                ++num_of_chars;
            }
        }
    }
    fclose(file);
}

float   calculate_freq(unsigned long num) {
    return (float) num / num_of_chars;
}

void    pq_create_list() {
    for (size_t i = 0; i < MAX_SYMBOLS; ++i) {
        if (all_meted_char[i]) {
            pq_insert(talloc((char)i, calculate_freq(all_meted_char[i])));
        }
    }
}

void    create_tree_from_list() {
    struct tnode    *rc = pq_pop();
    struct tnode    *lc = pq_pop();
    struct tnode    *p=NULL;

    for (; lc && rc; lc=pq_pop(), rc=pq_pop()) {
        // lc=pq_pop();
        // rc=pq_pop();
        /*create parent*/
        p=talloc(0,lc->freq+rc->freq);
        /*set parent link*/
        lc->parent=rc->parent=p;
        /*set child link*/
        p->right=rc; p->left=lc;
        /*make it non-leaf*/
        p->isleaf=0;
        /*add the new node to the queue*/
        pq_insert(p);
    }
    /*init root*/
    root = p;
}

void    clear_resources(struct tnode *node) {
    if (node->left) {
        clear_resources(node->left);
    }
    if (node->right) {
        clear_resources(node->right);
    }
    free(node);
}
/*
    @function main
*/
int main(int argc, char **argv)
{
    /*test pq*/
	const char *CODE_FILE = "../code.txt";
#ifdef RAW_BITS
	const char *OUT_FILE = "../encoded";
#else
	const char *OUT_FILE = "../encoded.txt";
#endif
    const char *IN_FILE = "../book.txt";
	FILE    *fout = NULL;
    int     fdin;
    int     fdout;

    char_coutner();
    pq_create_list();
	// memset(code,0,sizeof(code));

    /*build tree*/
    create_tree_from_list();
	/*build code*/
	generate_code(root,0);
	/*output code*/
	puts("code:");
	fout=fopen(CODE_FILE,"w");
	dump_code(stdout);
	dump_code(fout);
	fclose(fout);

	/*encode a sample string*/
	if ((fdout = open(OUT_FILE, O_WRONLY | O_CREAT)) < 0) {
        fprintf(stderr, "open() failed, errno = %d\n", errno);
        abort();
    }
    if ((fdin = open(IN_FILE, O_RDONLY)) < 0) {
        fprintf(stderr, "open() failed, errno = %d\n", errno);
        abort();
    }
	// encode(fin, stdout);
	encode(fdin, fdout);
	close(fdout);
    close(fdin);
	getchar();
	/*TODO: clear resources*/
    clear_resources(root);
    return 0;
}
