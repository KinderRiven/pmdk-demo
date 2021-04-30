/*
 * @Author: your name
 * @Date: 2021-04-30 13:55:45
 * @LastEditTime: 2021-04-30 14:22:18
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /pmdk-demo/libpmem_demo.cc
 */
#include <algorithm>
#include <assert.h>
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <dirent.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <map>
#include <mutex>
#include <pthread.h>
#include <queue>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <thread>
#include <time.h>
#include <unistd.h>
#include <vector>

#include "libpmem.h"

int main(int argc, char** argv)
{
    size_t _len = 8UL * 1024 * 1024 * 1024;
    char _path[128] = "/home/pmem0/libpmem-demo";
    int _is_pmem;
    void* _base = pmem_map_file(_path, _len, PMEM_FILE_CREATE, 0666, &_len, &_is_pmem);
    printf("%llx %d\n", (uint64_t)_base, _is_pmem);
    return 0;
}