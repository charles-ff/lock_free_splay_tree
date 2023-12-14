#include <bits/stdc++.h>
using namespace std;


struct Node {
    int key;
    int val;
    int height;
    Node* left;
    Node* right;
    Node* parent;
    Node(): key(0), height(1), left(nullptr), right(nullptr), parent(nullptr) {};
    Node(int x): key(x), height(1), left(nullptr), right(nullptr), parent(nullptr) {};
    Node(int x, Node* p): key(x), height(1), left(nullptr), right(nullptr), parent(p) {};
};

int height(Node* root) {
    if (root == NULL) return 0;
    return root->key;
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

    L->parent = root->parent;
    if (L->parent) {
        if (L->key < L->parent->key) {
            L->parent->left = L;
        } else {
            L->parent->right = L;
        }
    }


    root->parent = L;
    if (LR) LR->parent = root; 

    root->height = 1 + max(height(root->left), height(root->right));
    L->height = 1 + max(height(L->left), height(L->right));

    

    return L;
}

Node* leftRotate(Node* root) {
    Node* R = root->right;
    Node* RL = root->right->left;

    R->left = root;
    root->right = RL;
    
    R->parent = root->parent;
    if (R->parent) {
        if (R->key < R->parent->key) {
            R->parent->left = R;
        } else {
            R->parent->right = R;
        }
    }

    root->parent = R; 
    if (RL) RL->parent = root; 

    root->height = 1 + max(height(root->left), height(root->right));
    R->height = 1 + max(height(R->left), height(R->right));
    
    

    return R;
}

void printTree(Node* root, int height=0) {
    if (root == NULL) return;
    
    if (height == 0){
        printf("Root: %d\n", root->key);
    } else {
        for (size_t i = 0; i < height; i++){
            printf("| ");
        }
        printf("Node: %d, Parent: %d\n", root->key, root->parent->key);
    }

    printTree(root->left, height+1);
    printTree(root->right, height+1);
}

// AVL INTERFACE

// Node* insert_AVL(Node* root, int val) {
//     // 
//     if (root == NULL) return new Node(val); 

//     if (val < root->val) {
//         // GOING LEFT
//         root->left = insert_AVL(root->left, val);
//     } else if (val == root->val) {
//         // already exists
//         return root;
//     } else {
//         root->right = insert_AVL(root->right, val);
//     }

//     root->height = 1 + max(height(root->left), height(root->right));
//     int b = balance(root);
//     //LL
//     if (b > 1 && val < root->left->val) return rightRotate(root);
//     //LR
//     if (b > 1 && val > root->left->val) {
//         root->left = leftRotate(root->left);
//         return rightRotate(root);
//     }

//     // RR
//     if (b < -1 && val > root->right->val) return leftRotate(root);
    
//     // RL
//     if (b < -1 && val < root->right->val) {
//         root->right = rightRotate(root->right);
//         return leftRotate(root);
//     }

//     // BALANCED
//     return root;
// }

// return 1 if found, 0 if not found
// int find_AVL(Node* root, int val) {
//     if (root == NULL) return 0;
//     if (val < root->val) {
//         return find_AVL(root->left, val);
//     } else if (val == root->val) {
//         return 1;
//     } else {
//         return find_AVL(root->right, val);
//     }
// }

/// SPLAY TREE INTERFACE

class SplayTree {
private:
    Node* root;

public:
    SplayTree() {
        root = NULL;
    }


    void print() {
        printTree(root);
    }


    void splay(Node* node) {
        int key = node->key;


        while (node->parent != NULL) {
            
            // while the current node isn't the root

            Node* p = node->parent;

            if (p->key < key) {
                // PARENT 
                //        \\ 
                //          NODE
                Node* gp = p->parent;
                if (gp == NULL) {
                    // zig case
                    node = leftRotate(p);
                } else if (gp->key < p->key) {
                    // zig zig case
                    // GRANDPARENT 
                    //        \\ 
                    //          PARENT
                    //              \\ 
                    //                NODE 
                    node = leftRotate(leftRotate(gp));
                } else if (gp->key > p->key) {
                    // zig zag case
                    //              GRANDPARENT 
                    //             // 
                    //          PARENT
                    //              \\ 
                    //                NODE
                    leftRotate(p);
                    node = rightRotate(gp);
                } else {
                    // grandparent node has same key as current node
                    assert(false);
                }
            } else if (p->key > key) {
                //            PARENT 
                //         // 
                //    NODE
                Node* gp = p->parent;
                if (gp == NULL) {
                    // zig case
                    node = rightRotate(p);
                } else if (gp->key > p->key) {
                    // zig zig case
                    //          GRANDPARENT 
                    //        // 
                    //      PARENT
                    //      // 
                    //    NODE 
                    node = rightRotate(rightRotate(gp));
                } else if (gp->key < p->key) {
                    // zig zag case
                    //   GRANDPARENT 
                    //           \\ 
                    //          PARENT
                    //           // 
                    //       NODE
                    rightRotate(p);
                    node = leftRotate(gp);
                } else {
                    // the grandparent is the node wtf!???
                    assert(false);
                }
            } else {
                // the parent is the node wtf
                assert(false);
            }

        }

        root = node; 
    }

    void insert(int key) {
        if (root == NULL) {
            // EMPTY TREE WOOO
            root = new Node(key);
        }

        Node* ptr = root; 
        while (ptr->key != key) { 
            // do something
            if (ptr->key > key) {
            //GO LEFT 
                if (ptr->left == NULL) {
                    // insert here
                    ptr->left = new Node(key, ptr);
                } 
                ptr = ptr->left;
            } else {
                if (ptr->right == NULL) {
                    // insert here
                    ptr->right = new Node(key, ptr);
                } 
                ptr = ptr->right;
            }
        }

        splay(ptr);
    }

    Node* find(int key) {
        Node* ptr = root;
        while (ptr != NULL && ptr->key != key) {
            if (ptr->key > key) {
                ptr = ptr->left;
            } else {
                ptr = ptr->right;
            }
        }

        if (ptr != NULL) splay(ptr);
        return ptr;
    }


};

int main() {
    SplayTree S;
    S.insert(1);
    S.insert(2);
    S.insert(3);
    // S.print();
    cout << S.find(1)->key << endl;
    cout << S.find(2)->key << endl;
    cout << ((S.find(5) == NULL) ? "GOOD" : "BAD") << endl;
    // S.print();
    for (int i = 4; i < 20; i++) {
        S.insert(i);
        // S.print();
        // cout << "-----" << endl;
    }
    S.find(4);
    S.print();
}