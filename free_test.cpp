#include "lock_free.cpp"
#include <chrono>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

#define NUMTHREADS 4

typedef struct{
  pthread_t thread_id;
  int pid;
  SplayTree* tree;
  vector<string> work;
} thread_info_t;


// bool checkMissing(Node* root){

// }

bool BSTinvariant(Node* root, int left, int right){
    if (root == NULL)  return true;
    if(root->key < right && root->key > left){
        return BSTinvariant(root->left, left, root->key) && BSTinvariant(root->right, root->key, right);
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


void* doTest(void* tinfo){
  thread_info_t *thread = (thread_info_t*) tinfo;
  SplayTree* splayTree = thread->tree;
  for (auto task : thread->work){
    if (task[0] == 'f'){
      // splayTree->find(stoi(task.substr(2)));
    } else if (task[0] == 'i'){
      splayTree->insert(stoi(task.substr(2)));
    } else if (task[0] == 'r'){
      splayTree->remove(stoi(task.substr(2)));
    }
  }
}

int main(int argc, char *argv[]){
  if (argc != 2){
    cout << "Usage: ./coarse \"test case file\" " << endl;
    return 1;
  }

  SplayTree* tree = new SplayTree();
  thread_info_t* threads = new thread_info_t[NUMTHREADS]();

  ifstream file(argv[1]);
  string line;

  if (file.is_open()){
    int worker = 0;
    while (getline(file, line)){
      threads[worker].work.push_back(line);
      if (worker == NUMTHREADS-1){
        worker = 0;
      } else{
        worker++;
      }
    }
  } else{
    cout << "Unable to open file" << endl;
    return 1;
  }
  auto start = chrono::high_resolution_clock::now();
  for (int j = 0; j < 1; j++){
    tree = new SplayTree();
    for (int i = 0; i < NUMTHREADS; i++){
      threads[i].tree = tree;
      threads[i].pid = i;
      pthread_create(&threads[i].thread_id, NULL, &doTest, &threads[i]);
    }

    for (int i = 0; i < NUMTHREADS; i++){
      pthread_join(threads[i].thread_id, NULL);
    }
  }
  auto stop = chrono::high_resolution_clock::now();
  auto duration = chrono::duration_cast<std::chrono::milliseconds>(stop - start);

  bool correct = checkBST(tree->root);
  if (!correct){
    cout << "Violated BST invariant!!" << endl;
  }

  tree->print();

  cout << "Time taken: " << duration.count() << endl;
}
