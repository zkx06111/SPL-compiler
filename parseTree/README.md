# ParseTree
Use the following command to build.
```
make clean
make
```
Use the following cammand to print the parse tree.
```
./frontend < test.pas
```
The structure of the parse tree is as follows.
```cpp
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
```

