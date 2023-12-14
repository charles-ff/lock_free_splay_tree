#include "sequential.cpp"
#include "correctness.cpp"
#include <chrono>
#include <iostream>
#include <string>

using namespace std;


int main(int argc, char *argv[]){
  if (argc != 2){
    cout << "Usage: ./coarse \"test case file\" " << endl;
    return 1;
  }

  vector<string> work;
  ifstream file(argv[1]);
  string line;
  if (file.is_open()){
    while (getline(file, line)){
      work.push_back(line);
    }
  } else{
    cout << "Unable to open file" << endl;
    return 1;
  }

  SplayTree tree;
  auto start = chrono::high_resolution_clock::now();
  for (auto task : work){
    if (task[0] == 'f'){
      tree.find(stoi(task.substr(2)));
    } else if (task[0] == 'i'){
      tree.insert(stoi(task.substr(2)));
    }
  }
  auto stop = chrono::high_resolution_clock::now();
  auto duration = chrono::duration_cast<std::chrono::microseconds>(stop - start);

  bool error = checkBST(tree.root);

  tree.print();

  cout << "Time taken: " << duration.count() << endl;
}
