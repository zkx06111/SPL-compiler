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
    };
};

typedef struct TreeNode *pTNode;

pTNode newNode(const char *type, int lineNumber, const void *val, int vtype)
{
    pTNode p = (pTNode)malloc(sizeof(struct TreeNode));
    strcpy(p->type, type);
    p->lineNumber = lineNumber;
    p->visit = 0;
    if (val == NULL)
    {
        p->vali = 0;
    }
    else if (vtype == 0)
    {
        p->valc = *(char *)val;
    }
    else if (vtype == 2)
    {
        p->vali = *(int *)val;
    }
    else if (vtype == 1)
    {
        p->valf = *(double *)val;
    }
    p->child = p->sibling = NULL;
    return p;
}

void addLeftChild(pTNode par, pTNode ch)
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

void addRightChild(pTNode par, pTNode ch)
{
    if (par->child == NULL)
    {
        addLeftChild(par, ch);
        return;
    }
    par->rchild->sibling = ch;
    par->rchild = ch;
}
#endif