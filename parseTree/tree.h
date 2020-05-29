#ifndef TREE_H
#define TREE_H
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct TreeNode
{
    char type[20];
    int lineNumber;
    int visit;
    struct TreeNode *child;
    struct TreeNode *sibling;
    struct TreeNode *rchild; // rightmost child
    union {
        char valc;
        int vali;
        double valf;
        char *vals;
    };
};

typedef struct TreeNode *pTNode;

inline pTNode newNode(const char *type, int lineNumber, const void *val, int vtype)
{
    pTNode p = (pTNode)malloc(sizeof(struct TreeNode));
    strcpy(p->type, type);
    p->lineNumber = lineNumber;
    p->visit = 0;
    if (val == NULL)
    {
        p->vali = 0;
    }
    else if (vtype == 2)
    {
        p->valc = *(char *)val;
    }
    else if (vtype == 0)
    {
        p->vali = *(int *)val;
    }
    else if (vtype == 1)
    {
        p->valf = *(double *)val;
    }
    else if (vtype == 3)
    {
        char *s = (char *)val;
        p->vals = (char *)malloc(sizeof(char) * (strlen(s) + 1));
        strcpy(p->vals, s);
    }
    p->child = p->sibling = NULL;
    return p;
}

inline void addLeftChild(pTNode par, pTNode ch)
{
    if (ch->lineNumber < par->lineNumber)
    {
        par->lineNumber = ch->lineNumber;
    }
    ch->sibling = par->child;
    if (par->child == NULL)
    {
        par->rchild = ch;
    }
    par->child = ch;
}

inline void addRightChild(pTNode par, pTNode ch)
{
    if (par->child == NULL)
    {
        addLeftChild(par, ch);
        return;
    }
    par->rchild->sibling = ch;
    par->rchild = ch;
}

inline pTNode getKthChild(const TreeNode *par, int k) {
    if (k == 1) {
        return par->child;
    }
    pTNode res = par->child;
    for (int i = 1; i < k; ++i) {
        res = res->sibling;
    }
    return res;
}

pTNode buildTree();
void printNode(pTNode p);
void dfs(pTNode cur, pTNode par, int faid);
void print(pTNode cur, int depth = 0);
#endif