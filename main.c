#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>

int found; ///global variable used to determine what action to take concerning the tested word

typedef struct Node{
char * word;
struct Node *left;
struct Node * right;
}Node;

Node * constructor(char * word)
{
    Node * t = malloc(sizeof(Node));
    t->left = NULL;
    t->right = NULL;
    t->word = malloc(strlen(word)+1);
    strcpy(t->word,word);
    return t;
}

Node* insert(Node * root, char * word); ///inserts a node with passed word into tree
Node *search(Node * root, char* key); ///searches for a value in tree
Node *readTree(char * filename); ///reads words from dictionary file to tree

int count(Node *root); ///gets the size of the tree
int height(Node *root); ///gets the height of the tree
int maxNum(int a, int b); ///gets the maximum of 2 numbers (used in height function)
int wordValidator (char * s); ///validates that the word consists of letters only

void inorderSucc (Node * root, char* key, Node ** succ); ///gets the inorder successor
void inorderPred (Node * root, char* key, Node ** pred); ///gets the preorder successor
void startingPrint(Node *t); ///prints success of loading file, height and size once program starts
void searchDictionary(Node * dTree); ///looking up word in dictionary

int main()
{
    Node * dTree = readTree("EN-US-Dictionary.txt"); ///NB: text file should be in the same directory as the main file
    if(dTree != NULL)
    {
        startingPrint(dTree);
        searchDictionary(dTree);
    }
    else
        printf("Failed to Load Dictionary!\n");

    return 0;
}

void searchDictionary(Node * dTree)
{
    printf("\nEnter a sentence (enter Ctrl + z when finished):\n\n");
    char *sentence = malloc(10000);
    while(gets(sentence)) ///keeps looping until user enters the null character (ctrl+z)
    {
        printf("\n");
        char *token = strtok(sentence," ");
        while(token)
        {
            if(wordValidator(token)) ///if the word contains letters only
            {
                Node *temp = search(dTree,token); ///search for it in the tree
                if(found)
                    printf("%s  -->  CORRECT!\n",token); ///once it's found, it's correct
                else
                {
                    ///construct nodes for predecessor and successor that has empty word (to be used in case no pre/succ is found)
                    Node *succ = constructor(""); ///if no successor is found, an empty string is stored in it
                    Node *pred= constructor(""); ///if no predecessor is found, an empty string is stored in it
                    char * leaf = temp->word; ///storing the last leaf we've reached
                    inorderSucc(dTree,temp->word,&succ);
                    inorderPred(dTree,temp->word,&pred);
                    char * successor = succ->word;
                    char * predessecor = pred->word;
                    printf("%s  -->  INCORRECT .. Suggestions: [  %s  -  %s  -  %s  ]\n",token, leaf,successor,predessecor);
                }
            }
            else ///if word contains symbols other than letters it's considered as a wrong entry
                printf("%s --> INCORRECT .. Enter words that consist of letters only!\n",token);
            token = strtok(NULL," ");
        }
        printf("\n................................................\n");
        printf("\nEnter a sentence (enter Ctrl + z when finished):\n\n");
    }
    free(sentence);
    free(dTree->word);
    free(dTree);
}

Node* insert(Node * root, char * word)
{
    if (root == NULL)
        return constructor(word);
    if (strcasecmp(word,root->word)<0)
        root->left = insert(root->left,word);
    else if (strcasecmp(word,root->word)>0)
        root->right = insert(root->right,word);
    return root;
}

int count(Node * root)
{
    if(root == NULL) return 0;
    else return 1 + count(root->left) + count(root->right);
}

int maxNum(int a, int b)
{
    if (a>b) return a;
    else return b;
}

int height (Node * root)
{
    if(root == NULL)
        return -1;
    else return 1+ maxNum(height(root->left),height(root->right));
}

Node *readTree(char * filename)
{
    FILE *f = fopen(filename, "r");
    Node * t = NULL;
    if(f)
    {
        while(!feof(f))
        {
            char * temp = malloc(1000);
            fscanf(f,"%s\n",temp); ///scans word from file and inserts it to tree
            t = insert(t,temp);
        }
        fclose(f);
    }
    else
        printf("FILE NOT FOUND!!\n");
    return t;
}

Node *search(Node * root, char *key)
{
    found = 0; ///global flag variable to determine if the word is found or not
    if(root == NULL) return NULL;
    if(strcasecmp(key,root->word) == 0) ///word is found
    {
        found = 1;
        return root;
    }
    else if (strcasecmp(key,root->word)<0)
    {
        if(root->left == NULL) return root; ///if this is a leaf node, return it to be used in suggestions
        else return search(root->left,key);
    }
    else{
        if(root->right == NULL) return root; ///if this is a leaf node, return it to be used in suggestions
        else return search(root->right,key);
    }
}

void inorderSucc (Node * root, char* key, Node**succ)
{
    ///this function take a pointer to a pointer to a node, so it changes the node pointer itself throughout the code
    ///(Call by reference)
    ///NB: there's no case of equality of words as this function is only called when the word doesn't exist in tree
    if(root == NULL)
        *succ=NULL;
    if(strcasecmp(key,root->word)==0)
    {
        Node *temp;
        if (root->right != NULL) ///in case the leaf node we've stopped at has right child
        {
            temp = root->right ;
            while (temp->left)
            {
                temp = temp->left ;
                *succ = temp ;
            }
        }
    }
    if(strcasecmp(key,root->word)<0) ///if the leaf word we've stopped at < current root's word, successor then is this root
    {
        *succ=root;
        inorderSucc(root->left,key,succ);
    }
    else if(strcasecmp(key,root->word)>0)///if leaf word we've stopped at > current root's word, move into right subtree without changing the successor
    {
        inorderSucc(root->right,key,succ);
    }
}

void inorderPred (Node * root, char* key, Node **pred)
{
    ///this function take a pointer to a pointer to a node, so it changes the node pointer itself throughout the code
    ///(Call by reference)
    ///NB: there's no case of equality of words as this function is only called when the word doesn't exist in tree
    if(root == NULL)
        *pred=NULL;
    if(strcasecmp(key,root->word) == 0)
    {
        Node * temp;
        if (root->left != NULL) ///in case the leaf node we've stopped at has left child
        {
            temp = root->left;
            while (temp->right)
            {
                temp = temp->right;
                *pred = temp;
            }
        }
    }
    if(strcasecmp(key,root->word)<0) ///if leaf word we've stopped at < current root's word, move into left subtree without changing the predecessor
    {
        inorderPred(root->left,key,pred);
    }
    else if(strcasecmp(key,root->word)>0) ///if the leaf word we've stopped at > current root's word, predecessor then is this root
    {
        *pred=root;
        inorderPred(root->right,key,pred);
    }
}

void startingPrint(Node * t)
{
    printf("Dictionary Loaded Successfully .. !\n");
    printf("......................................\n");
    printf("Size = %d\n",count(t));
    printf("......................................\n");
    printf("Height = %d\n",height(t));
    printf("......................................\n");
}

int wordValidator(char *s)
{
    int i;
    for(i=0;i<strlen(s);i++)
    {
        if(!((s[i]>='a' && s[i]<='z') || (s[i]>='A' && s[i]<='Z'))&& s[i]!='\'')///if a character is neither a letter nor an apostrophe, return 0 (false)
            return 0;
    }
    return 1;
}
