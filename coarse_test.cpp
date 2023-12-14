#include "coarse_lock.cpp"
#include <chrono>
#include <iostream>

using namespace std;

#define NUMTHREADS 4

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

// bool checkMissing(Node* root){

// }


void* doTest(void* tinfo){
  thread_info_t *thread = (thread_info_t*) tinfo;
  SplayTree* splayTree = thread->tree;

  //only these inserts, ideally im gonna make different files
  //with different intensity's, mass spread, etc...
  splayTree->insert(thread->pid*5+1);
  splayTree->insert(thread->pid*5+2);
  splayTree->insert(thread->pid*5+3);
  splayTree->insert(thread->pid*5+4);
  splayTree->insert(thread->pid*5+5);
}

int main(){
  SplayTree* tree = new SplayTree();
  thread_info_t* threads = new thread_info_t[NUMTHREADS]();
  auto start = chrono::high_resolution_clock::now();
  for (int i = 0; i < NUMTHREADS; i++){
    threads[i].tree = tree;
    threads[i].pid = i;
    pthread_create(&threads[i].thread_id, NULL, &doTest, &threads[i]);
  }

  for (int i = 0; i < NUMTHREADS; i++){
    pthread_join(threads[i].thread_id, NULL);
  }
  auto stop = chrono::high_resolution_clock::now();
  auto duration = chrono::duration_cast<std::chrono::microseconds>(stop - start);

  bool error = checkBST(tree->root);

  tree->print();

  cout << "Time taken: " << duration.count() << endl;
}
