#include <bits/stdc++.h>
#include <pthread.h>
using namespace std;

struct Node {
    int key;
    // int val;
    // int height;
    pthread_mutex_t lock;
    Node* left;
    Node* right;
    Node* parent;
    Node(): key(0), left(nullptr), right(nullptr), parent(nullptr) {
        pthread_mutex_init(&lock, NULL);
    };
    Node(int x): key(x), left(nullptr), right(nullptr), parent(nullptr) {
        pthread_mutex_init(&lock, NULL);
    };
    Node(int x, Node* p): key(x), left(nullptr), right(nullptr), parent(p) {
        pthread_mutex_init(&lock, NULL);
    };

    void cleanup() {
        if (pthread_mutex_destroy(&lock) != 0) {
            // perror("pthread_mutex_destroy");
        }
    }
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

    // root->height = 1 + max(height(root->left), height(root->right));
    // L->height = 1 + max(height(L->left), height(L->right));

    

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

    // root->height = 1 + max(height(root->left), height(root->right));
    // R->height = 1 + max(height(R->left), height(R->right));
    
    

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

class SplayTree {
public:
    Node* root;
    pthread_mutex_t global_lock;

    SplayTree() {
        pthread_mutex_init(&global_lock, NULL);
        root = NULL;
    }


    void print() {
        printTree(root);
    }

    void insert(int key) {
        // if (root == NULL) {
        //     // EMPTY TREE 
        //     root = new Node(key);
        // }
        
        Node* arms[4];

        pthread_mutex_lock(&global_lock);
        if (root == NULL) {
            root = new Node(key);
            pthread_mutex_unlock(&global_lock);
            return;
        }

        arms[0] = root;
        for (int i = 0; i < 3; i++) {
            int j = i+1;
            pthread_mutex_lock(&arms[i]->lock);
            if (arms[i]->key > key) {
                arms[j] = arms[i]->left;
                if (arms[j] == NULL) {
                    arms[i]->left = new Node(key, arms[i]); 
                    lazy_splay(arms[i]->left);
                    pthread_mutex_unlock(&global_lock);
                    for (int k = 0; k <= i; k++) {
                        pthread_mutex_unlock(&arms[k]->lock);
                    }
                    return;
                } 
            } else if (arms[i]->key < key){
                arms[j] = arms[i]->right;
                if (arms[j] == NULL) {
                    arms[i]->right = new Node(key, arms[i]); 
                    lazy_splay(arms[i]->right);
                    pthread_mutex_unlock(&global_lock);
                    for (int k = 0; k <= i; k++) {
                        pthread_mutex_unlock(&arms[k]->lock);
                    }
                    return;
                }
                
            } else if (arms[i]->key == key){
                lazy_splay(arms[i]);
                pthread_mutex_unlock(&global_lock);
                for (int k = 0; k <= i; k++) {
                    pthread_mutex_unlock(&arms[k]->lock);
                }
                return; 
            }
        }

        pthread_mutex_unlock(&global_lock);

        int i = 3;
        while (true) {
            int j = (i+1) % 4; 
            pthread_mutex_lock(&arms[i]->lock);
            if (arms[i]->key > key) {
                Node* newArm = arms[i]->left;
                if (newArm == NULL) {
                    arms[i]->left = new Node(key, arms[i]); 
                    lazy_splay(arms[i]->left);
                    for (int k = 0; k < 4; k++) {
                        pthread_mutex_unlock(&arms[k]->lock);
                    }
                    return;
                } 
                pthread_mutex_unlock(&arms[j]->lock);
                arms[j] = newArm;
            } else if (arms[i]->key < key){
                Node* newArm = arms[i]->right;
                if (newArm == NULL) {
                    arms[i]->right = new Node(key, arms[i]); 
                    lazy_splay(arms[i]->right);
                    for (int k = 0; k < 4; k++) {
                        pthread_mutex_unlock(&arms[k]->lock);
                    }
                    return;
                }
                pthread_mutex_unlock(&arms[j]->lock);
                arms[j] = newArm;

            } else if (arms[i]->key == key){
                lazy_splay(arms[i]);
                for (int k = 0; k < 4; k++) {
                    pthread_mutex_unlock(&arms[k]->lock);
                }
                // cout << "DONE" << endl;
                return; 
            }

            i = (i + 1) % 4;
        }
        
    }

    bool find(int key) {
        Node* arms[4];

        pthread_mutex_lock(&global_lock);
        if (root == NULL) {
            pthread_mutex_unlock(&global_lock);
            return false;
        }

        arms[0] = root;
        for (int i = 0; i < 3; i++) {
            int j = i+1;
            pthread_mutex_lock(&arms[i]->lock);
            if (arms[i]->key > key) {
                arms[j] = arms[i]->left;
                if (arms[j] == NULL) {
                    pthread_mutex_unlock(&global_lock);
                    for (int k = 0; k <= i; k++) {
                        pthread_mutex_unlock(&arms[k]->lock);
                    }
                    return false;
                } 
            } else if (arms[i]->key < key){
                arms[j] = arms[i]->right;
                if (arms[j] == NULL) {
                    pthread_mutex_unlock(&global_lock);
                    for (int k = 0; k <= i; k++) {
                        pthread_mutex_unlock(&arms[k]->lock);
                    }
                    return false;
                }
            } else if (arms[i]->key == key){
                lazy_splay(arms[i]);
                pthread_mutex_unlock(&global_lock);
                for (int k = 0; k <= i; k++) {
                    pthread_mutex_unlock(&arms[k]->lock);
                }
                return true; 
            }
        }

        pthread_mutex_unlock(&global_lock);

        int i = 3;
        while (true) {
            int j = (i+1) % 4; 
            pthread_mutex_lock(&arms[i]->lock);
            if (arms[i]->key > key) {
                Node* newArm = arms[i]->left;
                if (newArm == NULL) {
                    for (int k = 0; k < 4; k++) {
                        pthread_mutex_unlock(&arms[k]->lock);
                    }
                    return false;
                } 
                pthread_mutex_unlock(&arms[j]->lock);
                arms[j] = newArm;
            } else if (arms[i]->key < key){
                Node* newArm = arms[i]->right;
                if (newArm == NULL) {
                    for (int k = 0; k < 4; k++) {
                        pthread_mutex_unlock(&arms[k]->lock);
                    }
                    return false;
                }
                pthread_mutex_unlock(&arms[j]->lock);
                arms[j] = newArm;

            } else if (arms[i]->key == key){
                lazy_splay(arms[i]);
                for (int k = 0; k < 4; k++) {
                    pthread_mutex_unlock(&arms[k]->lock);
                }
                return true;
            }

            i = (i + 1) % 4;
        }
        
        return false;
    }

    bool remove(int key) {
        // cout << "REMOVING" << key << endl;
        Node* arms[2];

        pthread_mutex_lock(&global_lock);
        if (root == NULL) {
            pthread_mutex_unlock(&global_lock);
            return false;
        }

        arms[0] = root;
        
        pthread_mutex_lock(&arms[0]->lock);
        if (root->key == key) {
            Node* tmp = root;
            root = NULL;
            if (tmp->right) { 
                pthread_mutex_lock(&tmp->right->lock);
                root = tmp->right; 
                root->parent = NULL;
                pthread_mutex_unlock(&tmp->right->lock);
            }

            if (tmp->left) {
                pthread_mutex_lock(&tmp->left->lock);
                if (root == NULL) {
                    root = tmp->left;
                    root->parent = NULL; 
                } else {
                    root->left = tmp->left;
                    root->left->parent = root;
                }
                pthread_mutex_unlock(&tmp->left->lock);
            }
            pthread_mutex_unlock(&global_lock);

            tmp->cleanup();
            free(tmp);


            return true;
        } else if (root->key < key) {
            arms[1] = root->right;
        } else {
            arms[1] = root->left;
        }


        pthread_mutex_unlock(&global_lock);
        int i = 1;
        while (true) {
            

            // have lock to j

            int j = (i + 1) % 2;
            if (arms[i] == NULL) {
                pthread_mutex_unlock(&arms[j]->lock);
                return false;
            }

            pthread_mutex_lock(&arms[i]->lock);
            // acquire lock i, now have both locks j -> i
            if (arms[i]->key == key) {
                Node* tmp = arms[i];
                Node* newNode = NULL;
                
                if (tmp->right) { 
                    // grabbing child locks
                    pthread_mutex_lock(&tmp->right->lock);
                    newNode = tmp->right; 
                    newNode->parent = arms[j];
                }

                if (tmp->left) {
                    pthread_mutex_lock(&tmp->left->lock);
                    if (newNode == NULL) {
                        newNode = tmp->left;
                        newNode->parent = arms[j]; 
                    } else {
                        newNode->left = tmp->left;
                        newNode->left->parent = newNode;
                    }
                }

                if (tmp->right) pthread_mutex_unlock(&tmp->right->lock);
                if (tmp->left) pthread_mutex_unlock(&tmp->left->lock);

                if (arms[j]->key > key) {
                    arms[j]->left = newNode;
                } else {
                    arms[j]->right = newNode;
                }

                tmp->cleanup();
                free(tmp);
                pthread_mutex_unlock(&arms[j]->lock);
                return true;
            } else if (arms[i]->key < key) {
                pthread_mutex_unlock(&arms[j]->lock);
                arms[j] = arms[i]->right;
            } else {
                pthread_mutex_unlock(&arms[j]->lock);
                arms[j] = arms[i]->left;
            }
            i = j;
        }
        
        return false;
    }

private:
    void lazy_splay(Node* node) {
        // you already hold the locks
        int key = node->key;

        if (node->parent != NULL) {
            
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

        // only one thread can hold the root node at a time?
        if (node->parent == NULL) root = node; 
    }


};

// int main() {
//     SplayTree S;
//     S.insert(1);
//     S.insert(2);
//     S.insert(3);
//     // S.print();
//     // cout << S.find(1)->key << endl;
//     // cout << S.find(2)->key << endl;
//     // cout << ((S.find(5) == NULL) ? "GOOD" : "BAD") << endl;
//     // S.print();
//     for (int i = 4; i < 20; i++) {
//         S.insert(i);
//         // S.print();
//         // cout << "-----" << endl;
//     }

//     S.print();

//     S.find(4);
//     S.print();
//     S.remove(4);
//     S.print();
//     S.find(6);
//     S.find(8);
//     cout << "IDOSAHDJSA" << endl;
//     S.print();
//     S.remove(7);
//     S.find(19);
//     cout << "----" << endl;
//     S.print();
//     S.remove(15);
//     // S.find(4);
//     S.print();
// }