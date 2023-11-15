# Lock-Free Splay Tree 

Team Members: Charles Lu, Roland Liu (czlu, rolandl)

## Summary
In our project, we plan on implementing a lock-free splay tree, a self-balancing binary tree that reduces access time of frequently accessed elements. We will build a sequential version, a lock version, and a lock-free version and run traces to compare their performance. We will likely be using pthreads for both concurrent implementations and mutex locks to implement the lock version.

## Background

## Challenges
The difficulties that come with the problem of implementing a lock-free splay tree will be in figuring out how to write correct but optimized algorithms for things like splaying the tree, inserting, and deleting. Because we are prioritizing speed first and foremost, we will want to make sure that the code does not spend too much time stuck on CAS instructions, but like we saw in lecture, minimizing the constraints on when a program can continue through a CAS instruction can lead to issues like the ABA problem. The maintaining of correctness while still looking for optimizations, we believe, will be one of our largest challenges.

Not only are these challenges present for the lock-free splay tree, but we also think that these  challenges are very relevant for the lock version as well. Since we again want to prioritize speed, we believe that there would be a way to do fine-grained locking for the lock version of the splay tree. From what we saw in lecture, fine-grained locking is a very intricate procedure and certainly would be very prone to race conditions, incorrectness, and possibly even making the performance worse.

The correctness of our implementations will definitely be a challenge, but the even more challenging part is going to be maintaining this correctness while searching for further optimizations. We ultimately want to optimize our code well enough for the performance to match our theoretical predictions: sequential < lock < lock-free.

## Resources
We will not need any special computers or equipment but it would be helpful to possibly use the PSC machines in order to test our code using higher/different numbers of processors. We won’t be using any code bases or existing code as we want to write everything ourselves to fully understand the intricacies and nuances of the structure. We will however be referring to some of the resources that discuss splay trees in 15451. Here are the links to the resources:
* https://www.cs.cmu.edu/~15451-f23/lectures/lecture08-splay-trees.pdf
* https://www.cs.cmu.edu/~sleator/papers/self-adjusting.pdf

We will also most likely be doing most of our coding/implementation on the GHC machines as they already provide a C++ compiler as well as any other libraries we may need

