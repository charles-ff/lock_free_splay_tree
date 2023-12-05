#include <bits/stdc++.h>
using namespace std;

struct TreeNode {
    int val; 
    TreeNode* left;
    TreeNode* right;
    // initializer
    TreeNode(): val(0), left(nullptr), right(nullptr) {};
    TreeNode(int x): val(x), left(nullptr), right(nullptr) {};
};

class SplayTree {
public:

    SplayTree() {
        root = NULL;
        size = 0;
    }

    bool isEmpty() {
        return (!size);
    }
    int getSize() {
        return size;
    }

    void insert(int x) {
        if (root == NULL) {
            // EMPTY TREE
            root = new TreeNode(x);
            size = 1; 
            return;
        }

        root = splay(x);
        
        if (root->val == x) {
            // duplicate case
            return; 
        } else {
            TreeNode* res = new TreeNode(x);
            if (x < root->val) {
                res->right = root;
                res->left = root->left;
                root->left = NULL;
            } else {
                res->left = root;
                res->right = root->right;
                root->right = NULL;
            }
            root = res;
            size++;

        }
    }

    void remove(int x) {
        if (root == NULL) return;
        splay(x);
        if (root->val == x) {
            if (root->left == NULL) {
                root = root->right;
            } else {
                TreeNode* tmp = root->right;
                root = root->left;
                splay(x);
                root->right = tmp;
            }
            size--;
        } else {
            // not in tree case
            return;
        }
    }


    TreeNode* find(int x) {
        if (root == NULL) return NULL;
        root = splay(x);
        if (root->val == x) {
            return root;
        } else {
            return NULL;
        }
    }

    void print() {
        printHelper(root, 0);
    }

private:
    TreeNode* root;
    int size; 

    TreeNode* splay(int x) {
        return splayHelper(root, x);
    }

    TreeNode* rotateRight(TreeNode* curr) {
        if (curr->left == NULL) return curr;
        TreeNode* res = curr->left;
        curr->left = res->right; 
        res->right = curr;
        return res;
    }

    TreeNode* rotateLeft(TreeNode* curr) {
        if (curr->right == NULL) return curr;
        TreeNode* res = curr->right;
        curr->right = res->left; 
        res->left = curr;
        return res;
    }
    

    TreeNode* splayHelper(TreeNode* curr, int x) {
        if (curr == NULL || curr->val == x) return curr;
        if (x < curr->val) {
            // GOING LEFT
            if (curr->left == NULL) {
                // EMPTY LEFT
                return curr;
            } else if (curr->left->val == x) {
                // zig
                return rotateRight(curr);
            } else if (x < curr->left->val) {
                // LEFT LEFT< zigzig

                curr->left->left = splayHelper(curr->left->left, x);
                return rotateRight(rotateRight(curr));
                
            } else if (curr->left->val < x) {
                // LEFT RIGHT < zigzag
                
                curr->left->right = splayHelper(curr->left->right, x);
                curr->left = rotateLeft(curr->left);
                return rotateRight(curr->left);
            } 

        } else if (curr->val < x) {
            // GOING RIGHT
            if (curr->right == NULL) {
                // EMPTY RIGHT
                return curr;
            } else if (x == curr->right->val) {
                // zag
                return rotateLeft(curr);
            } else if (x < curr->right->val) {
                // GOING RIGHT LEFT (zag zig)
                curr->right->left = splayHelper(curr->right->left, x);
                curr->right = rotateRight(curr->right);
                return rotateLeft(curr);
            } else if (curr->right->val < x) {
                curr->right->right = splayHelper(curr->right->right, x);
                return rotateLeft(rotateLeft(curr));
            }
        }

        // not actually possible.
        return NULL;
    }

    void printHelper(TreeNode* curr, int height) {
        if (curr == NULL) return;

        if (height == 0){
            printf("Root: %d\n", root->val);
        } else {
            for (size_t i = 0; i < height; i++){
                printf("| ");
            }
            printf("Node: %d\n", curr->val);
        }

        printHelper(curr->right, height+1);
        printHelper(curr->left, height+1);
    }



    
};

int main() {
    SplayTree S;
    S.insert(1);
    S.insert(2);
    S.insert(3);
    S.find(1);
    S.find(2);
    S.print();
}