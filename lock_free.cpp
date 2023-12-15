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
  bool removed;
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

void printTree(Node* root, int height=0) {
    if (root == NULL) return;
    
    if (height == 0){
        printf("Root: %d\n", root->key);
    } else {
        for (size_t i = 0; i < height; i++){
            printf("| ");
        }
        printf("Node: %d \n", root->key);
    }

    printTree(root->left, height+1);
    printTree(root->right, height+1);
}


class SplayTree{
public:
  Node* root;
  atomic<ROOT_STATUS> treeEmpty;

  SplayTree() {
    root = NULL;
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
      FamilyLine* location = traverse(NULL, NULL, NULL, root, key);
      while (location->valid == 0){
        location = traverse(NULL, NULL, NULL, root, key);
      }


      //issa duplicate, don't do anything
      if (location->node->key == key){
        return;
      }
      else{ //no duplicates, time to actually insert
      //atomically change flag to CHILD_IN_USE in order to prepare for insertion
        Node* node = new Node(key);
        Node* temp;
        if (location->node->key > key){
          temp = location->node->left;
        } else{
          temp = location->node->right;
        }

        FLAG expected = FLAG::NOT_USE;
        if (location->node->flag.compare_exchange_weak(expected, FLAG::CHILD_IN_USE)){
          if (temp == NULL){
            //atomically change the pointer to point to the new node
            if (location->node->key > key){
              location->node->left.compare_exchange_weak(temp, node);
            } else{
              location->node->right.compare_exchange_weak(temp, node);
            }
            expected = FLAG::CHILD_IN_USE;
            location->node->flag.compare_exchange_weak(expected, FLAG::NOT_USE);
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
    if (node != NULL && node->key == key){
      return new FamilyLine(true, ggp, gp, p, node);
    }

    //continue traversal
    Node* next;
    if (node ==NULL){
      next = NULL;
    } else if (node->key < key){
      next = node->right;
    } else{
      next = node->left;
    }

    //we did not find the key!
    if (next == NULL){
      return new FamilyLine(true, ggp, gp, p, node);
    }

    //if some other thread is already concurrently
    //using this node or is logically removed, we
    //are going to have to come back
    if (next->flag == FLAG::CHILD_IN_USE){
      return new FamilyLine(false, NULL, NULL, NULL, NULL);
    }
    if (next->flag == FLAG::REMOVE){
      return new FamilyLine(false, NULL, NULL, NULL, NULL);
    }
    if (next->flag == FLAG::IN_USE){
      return new FamilyLine(false, NULL, NULL, NULL, NULL);
    }

    //the node is valid, continue traversal
    return traverse(gp, p, node, next, key);
  }








};
