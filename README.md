# Lock-Free Splay Tree 

Team Members: Charles Lu, Roland Liu (czlu, rolandl)

## Summary
In our project, we plan on implementing a lock-free splay tree, a self-balancing binary tree that reduces access time of frequently accessed elements. We will build a sequential version, a lock version, and a lock-free version and run traces to compare their performance. We will likely be using pthreads for both concurrent implementations and mutex locks to implement the lock version.

## Background
Self-balancing binary trees have been a valuable data structure with two core properties: the ordering invariant, to support ordered datasets, and the height invariant, to support log(n) accesses, deletes, and insertions. The splay tree incorporates an additional property, which is that recently accessed elements tend to be near the root of the tree. This results in “hot” nodes having shorter access times, while “cold” nodes take longer to traverse through the tree. Splay trees have found niche applications in caching, garbage collection, and dataset processing.

The core difference between splay trees lies in how the rotations are performed when auto-balancing, where the rotations are split into three main cases: zig, zig zag, and zig zig. The type of rotation is dependent on the positioning of a node, its parent, and its grandparent. The set of rotations can be seen below (Figure 1, 2, 3), where node **x** is accessed. As shown, the node that is accessed is always moved closer to the root.  

![zig rotation](images/figure1.png)

*Figure 1: zig rotation*

![zig zag rotation](images/figure2.png)

*Figure 2: zig zag rotation*

![zig zig rotation](images/figure3.png)

*Figure 3: zig zig rotation*

Applications from splay-trees can benefit from parallelism to support concurrent accesses. Parallel threads can operate upon the same tree structure in shared memory, as long as race conditions and illegal accesses are prevented. One simple solution would be to use locks, locking the structure whenever a thread is accessing the tree. However, an algorithm with locks is a blocking algorithm, where a single thread may forever prevent any thread from accessing the data structure. In the case where the thread holding the lock stalls or crashes, no other threads can proceed with their work. A lock-free implementation, on the other hand, will ensure that at any given moment, a thread will be accessing the tree.

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
For our goals, we plan on achieving bug-free and high-performance implementations for sequential, lock, and lock-free. While not expecting perfect speedup, we hope to have a speedup of lock-free > lock > sequential. 

To benchmark these implementations, we will also be building large demo traces where the correctness will be evaluated via the final state of the tree. Our demo programs will be programmed such that no matter the computation order, the final state of the tree should be identical. In addition, we will have different demo programs to mimic different types of access patterns. 

If these goals happen to be met faster than we expected, we could spend more time on gaining further optimizations; however, if we deem that to not be possible, we could also try making programs that could take advantage of the lock-free splay tree. We would do this just as a showcase of the algorithm and the structure, and possibly use the three different implementations on this program to show even further the performance differences. If we choose not to do this in the event we have more time, we could also use the knowledge that we gained while implementing the lock-free splay tree to try implementing another lock-free tree structure.

If the converse were to happen and we end up behind schedule, we most likely would start chasing the completion and correctness of the algorithm instead of speed. We would at worst still want to implement the lock-free splay tree and not worry about the speed. If things are moving even slower than that, we would want to finish the fine-grained lock implementation splay tree. For this, we hope that if we are this behind schedule, we can still at least try to optimize the fine-grained lock implementation as much as possible.

For a demo program, we can run our algorithm and show its modifications and rotations . The demo would have very simple constraints, such as only 2-4 threads and a short data stream. For each access, we can display the most recent rotation, the tree’s change in structure, and which thread was responsible. We also plan on showing the speedup graphs from the different implementations.


## Platform Choice
We have chosen to use C++ to do our implementations. C++ is a lower level language and thus will allow us to have more granularity of control when we write our code. Having more control over what our code can do will also allow us to optimize our code better. We will also be using pthreads to run our traces as pthreads allow for an easy and effective way to spawn multiple concurrent processes. Because we do not care so much about the actual parallel processes and more about how the processes interact with the splay tree, we are not so concerned about finding the most optimized parallel system. And because all the implementations will be ran using the same parallel system and we only really care about the comparison between each implementation and not the overall exact speed, pthreads will give us an easy and efficient way to run performance tests. 

As for the computers that we will be using, we will be using the GHC machines as we do not have the same exact specifications on our local machines. We both have different core counts which could affect the results of some of our performance tests. As we don’t want to introduce any confounding variables to our performance tests if we run tests separately on our local systems, we want to use something that we know will be identical in terms of hardware, thus we decided upon using the GHC machines.

## Schedule

* 11/15 - 11/22: Sequential Algo & Source Reading
* 11/23 - 11/26: Thanksgiving Break
* 11/27 - 12/4: Trace Construction & Finegrained locking 
* 12/5 - 12/7: Debugging and testing Finegrained locking implementation
* 12/7 - 12/10: Work on completing lock-free implementation
* 12/10 - 12/12: Debugging and testing of lock-free implementation
* 12/12 - 12/13: Collect all the data we need, write more specficic test cases if needed
* 12/13 - 12/14: Work on Final Report 

