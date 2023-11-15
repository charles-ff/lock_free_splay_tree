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

## Goals and Deliverables


## Platform Choice
We have chosen to use C++ to do our implementations. C++ is a lower level language and thus will allow us to have more granularity of control when we write our code. Having more control over what our code can do will also allow us to optimize our code better. We will also be using pthreads to run our traces as pthreads allow for an easy and effective way to spawn multiple concurrent processes. Because we do not care so much about the actual parallel processes and more about how the processes interact with the splay tree, we are not so concerned about finding the most optimized parallel system. And because all the implementations will be ran using the same parallel system and we only really care about the comparison between each implementation and not the overall exact speed, pthreads will give us an easy and efficient way to run performance tests. 
As for the computers that we will be using, we will be using the GHC machines as we do not have the same exact specifications on our local machines. We both have different core counts which could affect the results of some of our performance tests. As we don’t want to introduce any confounding variables to our performance tests if we run tests separately on our local systems, we want to use something that we know will be identical in terms of hardware, thus we decided upon using the GHC machines.

## Schedule
