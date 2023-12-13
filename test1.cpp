#include "sequential.cpp"
#include <chrono>
#include <iostream>

using namespace std;

int main(){
  Node* root = NULL;
  auto start = chrono::high_resolution_clock::now();
  for (int i = 0; i < 100; i++){
    root = insert(root, 1);
    root = insert(root, 2);
    root = insert(root, 3);
    int buf;  
    root = find(root, 1, buf);
    root = find(root, 2, buf);
  }
  auto stop = chrono::high_resolution_clock::now();
  printTree(root);
  auto duration = chrono::duration_cast<std::chrono::microseconds>(stop - start);


  cout << "Time taken: " << duration.count() << endl;
}
