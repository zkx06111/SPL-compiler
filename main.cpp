#include <iostream>

#include "tree.h"
#include "sem.h"
#include "gen.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <file name>" << std::endl;
        return -1;
    }

    freopen(argv[1], "r", stdin);

    TreeNode *root = buildTree();
    print(root);
    if (root == nullptr) {
        return -1;
    }
    //dfs(root, NULL, 0);
    bool sem_ret = sem::CheckSem(root);
    if (!sem_ret) {
        std::cout << "sem error occurred" << std::endl;
        return -2;
    }

    gen::GenCode(root, argv[1]);

    gen::GenExe();

    return 0;
}