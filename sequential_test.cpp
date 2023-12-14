#include "sequential.cpp"
#include <chrono>
#include <iostream>

using namespace std;


typedef struct{
  pthread_t thread_id;
  int pid;
  SplayTree* tree;
} thread_info_t;


bool BSTinvariant(Node* root, int left, int right){
    if (root == NULL)  return true;
    if(root->val < right && root->val > left){
        return BSTinvariant(root->left, left, root->val) && BSTinvariant(root->right, root->val, right);
    }
    else{
      return false;
    } 
}

bool checkBST(Node* root) {
    int min = -100000000;
    int max = 10000000;
    return BSTinvariant(root, min, max);
}


int main(){
  SplayTree tree;
  auto start = chrono::high_resolution_clock::now();
  for (int i = 1; i <= 20; i ++){
    tree.insert(i);
  }
  auto stop = chrono::high_resolution_clock::now();
  auto duration = chrono::duration_cast<std::chrono::microseconds>(stop - start);

  bool error = checkBST(tree.root);

  tree.print();

  cout << "Time taken: " << duration.count() << endl;
}
