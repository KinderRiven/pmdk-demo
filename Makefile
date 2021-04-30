all: clean libpmem_demo

libpmem_demo: libpmem_demo.cc
	g++ -std=c++11 libpmem_demo.cc -o libpmem_demo

clean:
	rm -rf libpmem_demo