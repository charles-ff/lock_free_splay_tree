#include <bits/stdc++.h>
using namespace std;


struct Node {
    int val;
    int height;
    Node* left;
    Node* right;
    Node(): val(0), height(1), left(nullptr), right(nullptr) {};
    Node(int x): val(x), height(1), left(nullptr), right(nullptr) {};
};

int height(Node* root) {
    if (root == NULL) return 0;
    return root->val;
}

int balance(Node* root) {
    if (root == NULL) return 0;
    return height(root->left) - height(root->right);
}

// GENERAL INTERFACE

Node* rightRotate(Node* root) {
    Node* L = root->left;
    Node* LR = root->left->right;

    L->right = root;
    root->left = LR;

    root->height = 1 + max(height(root->left), height(root->right));
    L->height = 1 + max(height(L->left), height(L->right));

    return L;
}

Node* leftRotate(Node* root) {
    Node* R = root->right;
    Node* RL = root->right->left;

    R->left = root;
    root->right = RL;

    root->height = 1 + max(height(root->left), height(root->right));
    R->height = 1 + max(height(R->left), height(R->right));

    return R;
}

void printTree(Node* root, int height=0) {
    if (root == NULL) return;
    
    if (height == 0){
        printf("Root: %d\n", root->val);
    } else {
        for (size_t i = 0; i < height; i++){
            printf("| ");
        }
        printf("Node: %d\n", root->val);
    }

    printTree(root->left, height+1);
    printTree(root->right, height+1);
}

// AVL INTERFACE

Node* insert_AVL(Node* root, int val) {
    if (root == NULL) return new Node(val); 

    if (val < root->val) {
        // GOING LEFT
        root->left = insert_AVL(root->left, val);
    } else if (val == root->val) {
        // already exists
        return root;
    } else {
        root->right = insert_AVL(root->right, val);
    }

    root->height = 1 + max(height(root->left), height(root->right));
    int b = balance(root);
    //LL
    if (b > 1 && val < root->left->val) return rightRotate(root);
    //LR
    if (b > 1 && val > root->left->val) {
        root->left = leftRotate(root->left);
        return rightRotate(root);
    }

    // RR
    if (b < -1 && val > root->right->val) return leftRotate(root);
    
    // RL
    if (b < -1 && val < root->right->val) {
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }

    // BALANCED
    return root;
}

// return 1 if found, 0 if not found
int find_AVL(Node* root, int val) {
    if (root == NULL) return 0;
    if (val < root->val) {
        return find_AVL(root->left, val);
    } else if (val == root->val) {
        return 1;
    } else {
        return find_AVL(root->right, val);
    }
}

/// SPLAY TREE INTERFACE

Node* splay(Node* root, int val) {
    if (root == NULL || root->val == val) return root;
    if (val < root->val) {
        // GOING LEFT
        if (root->left == NULL) {
            // EMPTY LEFT
            return root;
        } else if (root->left->val == val) {
            // zig
            return rightRotate(root);
        } else if (val < root->left->val) {
            // LEFT LEFT< zigzig

            root->left->left = splay(root->left->left, val);
            return rightRotate(rightRotate(root));
            
        } else if (root->left->val < val) {
            // LEFT RIGHT < zigzag
            
            root->left->right = splay(root->left->right, val);
            root->left = leftRotate(root->left);
            return rightRotate(root->left);
        } 

    } else if (root->val < val) {
        // GOING RIGHT
        if (root->right == NULL) {
            // EMPTY RIGHT
            return root;
        } else if (val == root->right->val) {
            // zag
            return leftRotate(root);
        } else if (val < root->right->val) {
            // GOING RIGHT LEFT (zag zig)
            root->right->left = splay(root->right->left, val);
            root->right = rightRotate(root->right);
            return leftRotate(root);
        } else if (root->right->val < val) {
            root->right->right = splay(root->right->right, val);
            return leftRotate(leftRotate(root));
        }
    }

    // not actually possible.
    return NULL;
}

Node* insert(Node* root, int val) {
    if (root == NULL) {
        // EMPTY TREE
        return new Node(val);
    }

    root = splay(root, val);
        
    if (root->val == val) {
        // duplicate case
        return root; 
    } else {
        Node* res = new Node(val);
        if (val < root->val) {
            res->right = root;
            res->left = root->left;
            root->left = NULL;
        } else {
            res->left = root;
            res->right = root->right;
            root->right = NULL;
        }
        return res;
    }
}

Node* find(Node* root, int val, int& found) {
    if (root == NULL) {
        found = 0;
        return NULL;
    }
    root = splay(root, val);
    if (root->val == val) {
        found = 1;
    } else {
        found = 0;
    }
    return root;
}

// int main() {
//     Node* A = NULL, *S = NULL;
//     S = insert(S, 1);
//     S = insert(S, 2);
//     S = insert(S, 3);
//     printTree(S);
//     int buf;
//     S = find(S, 1, buf);
//     S = find(S, 2, buf);
//     printTree(S);
// }