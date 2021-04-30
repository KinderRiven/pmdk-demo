PMDK_LIB := /home/hanshukai/SplitKV/third-party/pmdk

PMDK_LINK_FLAGS := -Wl,--no-whole-archive -L$(PMDK_LIB) -lpmem -lpmemobj -lpmempool

all: clean libpmem_demo

libpmem_demo: libpmem_demo.cc
	g++ -std=c++11 libpmem_demo.cc -o libpmem_demo $(PMDK_LINK_FLAGS)

clean:
	rm -rf libpmem_demo