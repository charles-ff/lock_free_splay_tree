CXX=g++
CFLAGS := -std=c++14 -fvisibility=hidden -lpthread -Wall -Wextra

all : sequential coarse

sequential : sequential.cpp sequential_test.cpp correctness.cpp
	$(CXX) -o $@ sequential_test.cpp

coarse : coarse_lock.cpp coarse_test.cpp correctness.cpp
	$(CXX) -o $@ $(CFLAGS) coarse_test.cpp

clean:
	rm -f sequential coarse

.PHONY: all clean
