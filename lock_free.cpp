#include <iostream>
#include <atomic>
#include <bits/stdc++.h>
using namespace std;

enum class FLAG{
  NOT_USE, //no concurrent actions are affecting this node
  REMOVE, //the node is logically removed, awaiting physical removal
  IN_USE, //the node is currently being concurrently accessed
  CHILD_IN_USE //the node's child is currently in use
};

enum class ROOT_STATUS{
  EMPTY, 
  MODIFIED, 
  FILLED
};

struct Node{
  int key;
  atomic<bool> removed;
  atomic<Node*> left;
  atomic<Node*> right;
  atomic<FLAG> flag; 
  Node(int x): key(x), removed(false), left(nullptr), right(nullptr), flag(FLAG::NOT_USE) {};
};

struct FamilyLine{
  bool valid;
  Node* ggp;
  Node* gp;
  Node* p;
  Node* node;
  FamilyLine(bool isValid, Node* w, Node* x, Node* y, Node* z): valid(isValid), ggp(w), gp(x), p(y), node(z) {};
};

void printTree(Node* root, int height=0, bool isLeft=0) {
    if (root == nullptr) return;
    
    if (height == 0){
        printf("Root: %d ", root->key);
        if (root->removed){
          printf("(Logically Removed)");
        }
        printf("\n");
    } else {
        for (size_t i = 0; i < height; i++){
            printf("| ");
        }
        if (isLeft){
          printf("L ");
        } else{
          printf("R ");
        }
        printf("Node: %d ", root->key);
        if (root->removed){
          printf("(Logically removed)");
        }
        printf("\n");
    }

    printTree(root->left, height+1, true);
    printTree(root->right, height+1, false);
}


class SplayTree{
public:
  Node* root;
  atomic<ROOT_STATUS> treeEmpty;

  SplayTree() {
    root = nullptr;
    treeEmpty = ROOT_STATUS::EMPTY; 
  }

  void print() {
        printTree(root);
    }

  void insert(int key){
    while(true){

      //check if tree is empty
      ROOT_STATUS expected = ROOT_STATUS::EMPTY;
      if (treeEmpty.compare_exchange_weak(expected, ROOT_STATUS::MODIFIED)){
        root = new Node(key);
        expected = ROOT_STATUS::MODIFIED;
        treeEmpty.compare_exchange_weak(expected, ROOT_STATUS::FILLED);
        return;
      } else{
        while(true){
          expected = ROOT_STATUS::FILLED;
          if (treeEmpty.compare_exchange_weak(expected, ROOT_STATUS::FILLED)){
            break;
          }
        }
      }


      //keep looping until the location to insert is not being used
      FamilyLine* location = traverse(nullptr, nullptr, nullptr, root, key);
      while (location->valid == 0){
        location = traverse(nullptr, nullptr, nullptr, root, key);
      }


      //issa duplicate, don't do anything
      if (location != nullptr && location->node->key == key){
        lazySplay(location->node);
        return;
      }
      else{ //no duplicates, time to actually insert
      //atomically change flag to CHILD_IN_USE in order to prepare for insertion
        Node* node = new Node(key);
        Node* temp;

        if (location!= nullptr && location->node->key > key){
          temp = location->node->left;
        } else{
          temp = location->node->right;
        }

        FLAG expected = FLAG::NOT_USE;
        if (location != nullptr && location->node->flag.compare_exchange_weak(expected, FLAG::CHILD_IN_USE)){
          if (temp == nullptr){
            //atomically change the pointer to point to the new node
            if (location->node->key > key){
              location->node->left.compare_exchange_weak(temp, node);
            } else{
              location->node->right.compare_exchange_weak(temp, node);
            }
            expected = FLAG::CHILD_IN_USE;
            location->node->flag.compare_exchange_weak(expected, FLAG::NOT_USE);
            lazySplay(location->node);
            return;
          } else{
            expected = FLAG::CHILD_IN_USE;
            location->node->flag.compare_exchange_weak(expected, FLAG::NOT_USE);
            }
          }
        }
        // cout << "already in use try again" << endl;, we can use this time to see how many times
        // a thread has to re
      }
    }

  FamilyLine* traverse(Node* ggp, Node* gp, Node* p, Node* node, int key){
    //recursive base case: we found the key!
    if (node != nullptr && node->key == key){
      return new FamilyLine(true, ggp, gp, p, node);
    }

    //continue traversal
    Node* next;
    if (node ==nullptr){
      next = nullptr;
    } else if (node->key < key){
      next = node->right;
    } else{
      next = node->left;
    }

    //we did not find the key!
    if (next == nullptr){
      return new FamilyLine(true, ggp, gp, p, node);
    }

    //if some other thread is already concurrently
    //using this node or is logically removed, we
    //are going to have to come back
    if (next->flag == FLAG::CHILD_IN_USE){
      return new FamilyLine(false, nullptr, nullptr, nullptr, nullptr);
    }
    if (next->flag == FLAG::REMOVE){
      return new FamilyLine(false, nullptr, nullptr, nullptr, nullptr);
    }
    if (next->flag == FLAG::IN_USE){
      return new FamilyLine(false, nullptr, nullptr, nullptr, nullptr);
    }

    //the node is valid, continue traversal
    return traverse(gp, p, node, next, key);
  }

  bool find(int key) {
    FamilyLine* location = traverse(nullptr, nullptr, nullptr, root, key);
    while (location->valid == 0) location = traverse(nullptr, nullptr, nullptr, root, key);
    // cout << "HELOLO" << endl;
    bool found = (location != NULL);
    if (found) lazySplay(location->node);
    return found;
  }

  void remove(int key){
    while(true){
      // cout << "attempting removal of " << key << endl;
      //keep looping until the path to the node of deletion is not being concurrently acted upon
      FamilyLine* location = traverse(nullptr, nullptr, nullptr, root, key);
      while (location->valid == 0){
        location = traverse(nullptr, nullptr, nullptr, root, key);
      }

      if (location->node->key != key){ //the key is not in the the tree, do nothing
        // cout << "i aint find nothing" << endl;
        return;
      } else { //the key is in the tree, time to delete
          FLAG expected = FLAG::NOT_USE;
          if (location->node->flag.compare_exchange_weak(expected, FLAG::REMOVE)){
            //node has 2 children, lets delay deletion for a bit, too much contention if we do now
            if (location->node->left != nullptr && location->node->right != nullptr){
              // cout << "logically rm" << endl;
              bool expected2 = false;
              if(location->node->removed.compare_exchange_weak(expected2, true)){ //logically remove
                // cout << "set logic rm flag" << endl;
              }
              expected = FLAG::REMOVE;
              location->node->flag.compare_exchange_weak(expected, FLAG::NOT_USE);
              return;
            } else{ //node has <2 children, there won't be too much contention, we can do now
                // cout << "physically rm" << endl;
                bool expected2 = false;
                location->node->removed.compare_exchange_weak(expected2, true); //logically remove
                Node* temp;
                if (location->node->left == nullptr && location->node->right == nullptr){
                  temp = nullptr;
                } else if (location->node->left == nullptr && location->node->right != nullptr){
                  temp = location->node->right;
                } else{
                  temp = location->node->left;
                }

                if (location->p == nullptr){ //its the root
                  root = temp;
                  if (temp == nullptr){
                    ROOT_STATUS expectedRoot = ROOT_STATUS::FILLED;
                    treeEmpty.compare_exchange_weak(expectedRoot, ROOT_STATUS::EMPTY);
                  }
                  //prob free some shit
                  return;
                } else{
                  expected = FLAG::NOT_USE;
                  while(!location->p->flag.compare_exchange_weak(expected, FLAG::CHILD_IN_USE) && (temp == nullptr || temp->flag.compare_exchange_weak(expected, FLAG::NOT_USE))){
                  }
                }
                bool isLeft = false;
                if (location->p->left == location->node) isLeft = true;
                Node* expectedChild = location->node;
                
                //physically removing
                if (isLeft){
                  location->p->left.compare_exchange_weak(expectedChild, temp);
                } else{
                  location->p->right.compare_exchange_weak(expectedChild, temp);
                }
                //can prob free shit here

                expected = FLAG::CHILD_IN_USE;
                location->p->flag.compare_exchange_weak(expected, FLAG::NOT_USE);

                return;
              }
          }
      }
    }
  }

  void lazySplay(Node* node) {
      FamilyLine* location = traverse(nullptr, nullptr, nullptr, root, node->key);
      while (location->valid == 0){
        // free(location);
        location = traverse(nullptr, nullptr, nullptr, root, node->key);
      }

      if (location == NULL || location->node != node) return;
      if (location->node == NULL || location->p == NULL || location->gp == NULL || location->ggp == NULL) return;

      bool flags = false;
    
      FLAG expected = FLAG::NOT_USE;
      if (location->ggp->flag.compare_exchange_weak(expected, FLAG::IN_USE)) {
        expected = FLAG::NOT_USE;
        if (location->gp->flag.compare_exchange_weak(expected, FLAG::IN_USE)) {
          expected = FLAG::NOT_USE;
          if (location->p->flag.compare_exchange_weak(expected, FLAG::IN_USE)) {
            expected = FLAG::NOT_USE;
            if (location->node->flag.compare_exchange_weak(expected, FLAG::IN_USE)) {
              flags &= 1;
            }
          }
        }
      }

    if (flags == false) {
      // RESET FLAGS
      expected = FLAG::IN_USE;
      location->ggp->flag.compare_exchange_weak(expected, FLAG::NOT_USE);
      expected = FLAG::IN_USE;
      location->gp->flag.compare_exchange_weak(expected, FLAG::NOT_USE);
      expected = FLAG::IN_USE;
      location->p->flag.compare_exchange_weak(expected, FLAG::NOT_USE);
      expected = FLAG::IN_USE;
      location->node->flag.compare_exchange_weak(expected, FLAG::NOT_USE);
      return;

    } else {
      Node* gp = new Node(location->gp->key);
      Node* p = new Node(location->p->key);
      Node* n = new Node(location->node->key);
      Node* tmp;

      if (p->key < node->key) {
        // PARENT 
        //        \\ 
        //          NODE
        if (gp->key < p->key) {
            // zig zig case
            // GRANDPARENT                            NODE
            //        \\                            //
            //          PARENT       =>          PARENT
            //              \\                  //
            //                NODE      GRANDPARENT
            tmp = nullptr;
            gp->left.compare_exchange_weak(tmp, location->gp->left);
            gp->right.compare_exchange_weak(tmp, location->p->left);
            p->left.compare_exchange_weak(tmp, gp);
            p->right.compare_exchange_weak(tmp, location->node->left);
            n->left.compare_exchange_weak(tmp, p);
            n->right.compare_exchange_weak(tmp, location->node->right);
        } else if (gp->key > p->key) {
            // zig zag case
            //              GRANDPARENT 
            //             // 
            //          PARENT              =>         NODE
            //              \\                      //      ||
            //                NODE               PARENT     GRANDPARENT
            tmp = nullptr;
            gp->left.compare_exchange_weak(tmp, location->node->right);
            gp->right.compare_exchange_weak(tmp, location->gp->right);
            p->left.compare_exchange_weak(tmp, location->p->left);
            p->right.compare_exchange_weak(tmp, location->node->left);
            n->left.compare_exchange_weak(tmp, p);
            n->right.compare_exchange_weak(tmp, gp);
        } else {
            // grandparent node has same key as current node
            assert(false);
        }
    } else if (p->key > node->key) {
        //            PARENT 
        //         // 
        //    NODE
        if (gp->key > p->key) {
            // zig zig case
            //          GRANDPARENT               NODE
            //        //                                ||  
            //      PARENT                =>              PARENT
            //      //                                        ||
            //    NODE                                          GRANDPARENT
            tmp = nullptr;
            gp->left.compare_exchange_weak(tmp, location->p->right);
            gp->right.compare_exchange_weak(tmp, location->gp->right);
            p->left.compare_exchange_weak(tmp, location->gp->left);
            p->right.compare_exchange_weak(tmp, gp);
            n->left.compare_exchange_weak(tmp, location->node->left);
            n->right.compare_exchange_weak(tmp, p);
        } else if (gp->key < p->key) {
            // zig zag case
            //   GRANDPARENT                             
            //           \\                           
            //          PARENT      =>            NODE
            //           //                     //        ||
            //       NODE             GRANDPARENT           PARENT
            tmp = nullptr;
            gp->left.compare_exchange_weak(tmp, location->gp->left);
            gp->right.compare_exchange_weak(tmp, location->node->left);
            p->left.compare_exchange_weak(tmp, location->node->right);
            p->right.compare_exchange_weak(tmp, location->p->right);
            n->left.compare_exchange_weak(tmp, gp);
            n->right.compare_exchange_weak(tmp, p);
        } else {
            // the grandparent is the node wtf!???
            assert(false);
        }
      }

      if (location->ggp->key < gp->key) {
        tmp = location->ggp->right;
        location->ggp->right.compare_exchange_weak(tmp,gp);
      } else {
        tmp = location->ggp->left;
        location->ggp->right.compare_exchange_weak(tmp,gp);
      }

      // free(location->gp);
      // free(location->p);
      // free(location->node);
      
      
      expected = FLAG::IN_USE;
      location->ggp->flag.compare_exchange_weak(expected, FLAG::NOT_USE);
      free(location);
      return;
    }
  }



};
