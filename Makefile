CXX=g++
CFLAGS := -std=c++14 -fvisibility=hidden -lpthread -Wall -Wextra

all : sequential coarse finegrain free

sequential : sequential.cpp sequential_test.cpp correctness.cpp
	$(CXX) -o $@ sequential_test.cpp

coarse : coarse_lock.cpp coarse_test.cpp correctness.cpp
	$(CXX) -o $@ $(CFLAGS) coarse_test.cpp

finegrain: finegrain_lock.cpp finegrain_test.cpp correctness.cpp
	$(CXX) -o $@ $(CFLAGS) finegrain_test.cpp

free: lock_free.cpp free_test.cpp correctness.cpp
	$(CXX) -o $@ $(CFLAGS) free_test.cpp

clean:
	rm -f sequential coarse finegrain free

.PHONY: all clean
