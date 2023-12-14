#include "coarse_lock.cpp"
#include "correctness.cpp"
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


void* doTest(void* tinfo){
  thread_info_t *thread = (thread_info_t*) tinfo;
  SplayTree* splayTree = thread->tree;

  for (auto task : thread->work){
    if (task[0] == 'f'){
      splayTree->find(stoi(task.substr(2)));
    } else if (task[0] == 'i'){
      splayTree->insert(stoi(task.substr(2)));
    }
  }
  //only these inserts, ideally im gonna make different files
  //with different intensity's, mass spread, etc...
  // splayTree->insert(thread->pid*5+1);
  // splayTree->insert(thread->pid*5+2);
  // splayTree->insert(thread->pid*5+3);
  // splayTree->insert(thread->pid*5+4);
  // splayTree->insert(thread->pid*5+5);
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
      if (worker = NUMTHREADS-1){
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
