#ifndef TREE_H
#define TREE_H
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct TreeNode
{
    char type[20];
    int lineNumber;
    struct TreeNode *child;
    struct TreeNode *sibling;
    union {
        char valc;
        int vali;
        double valf;
    };
};

typedef struct TreeNode *pTNode;

pTNode newNode(const char *type, int lineNumber, const void *val)
{
    pTNode p = (pTNode)malloc(sizeof(struct TreeNode));
    strcpy(p->type, type);
    p->lineNumber = lineNumber;
    if (val == NULL)
    {
        p->vali = 0;
    }
    else if (sizeof(val) == sizeof(char))
    {
        p->valc = *(char *)val;
    }
    else if (sizeof(val) == sizeof(int))
    {
        p->vali = *(int *)val;
    }
    else if (sizeof(val) == sizeof(double))
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
    par->child = ch;
}
#endif