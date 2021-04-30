PMDK_LIB := /home/hanshukai/SplitKV/third-party/pmdk

PMDK_LINK_FLAGS := -Wl,--no-whole-archive -L$(PMDK_LIB) -lpmem

all: clean libpmem_demo

libpmem_demo: libpmem_demo.cc
	g++ -O0 -std=c++11 libpmem_demo.cc -o libpmem_demo $(PMDK_LINK_FLAGS)

clean:
	rm -rf libpmem_demo