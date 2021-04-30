/*
 * @Author: your name
 * @Date: 2021-04-30 13:55:45
 * @LastEditTime: 2021-04-30 15:32:19
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
#include "timer.h"

struct worker_context_t {
public:
    uint32_t thread_id;
    uint64_t base;
    size_t size;
    uint32_t bs;
};

static int numa_0[] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 21, 22, 23, 24, 25, 26, 27, 28, 29 //, 11, 13, 15, 17, 19, 31, 33,35, 37, 39
};

static uint32_t g_num_loop = 5;

static uint32_t g_num_thread = 8;

static uint32_t g_block_size = 64;

static void random_write(worker_context_t* context)
{
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(numa_0[context->thread_id], &mask);

    if (pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask) < 0) {
        printf("threadpool, set thread affinity failed.\n");
    }

    uint64_t _start = context->base;
    size_t _end = _start + context->size;
    uint32_t _bs = context->bs;
    uint32_t _skip = _bs * 4;
    uint64_t _src = (uint64_t)malloc(_bs);

    Timer _timer;
    _timer.Start();
    for (int i = 0; i < g_num_loop; i++) {
        uint64_t _dest = _start;
        while (_dest < _end) {
            pmem_memcpy_persist((void*)_dest, (void*)_src, _bs);
            // pmem_memmove_nodrain((void*)_dest, (void*)_src, _bs);
            _dest += _skip;
        }
    }
    _timer.Stop();

    size_t _sz = g_num_loop * context->size;
    size_t _cnt = _sz / _skip;
    double _sec = _timer.GetSeconds();
    double _lat = _timer.Get() / _cnt;
    printf("[%d][cost:%.2fseconds][cnt:%zu][lat:%.2fns][iops:%.2f][bw:%.2fMB/s]\n",
        context->thread_id, _sec, _cnt, _lat, 1.0 * _cnt / _sec, 1.0 * _sz / (_sec * 1024UL * 1024));
}

static void seq_write(worker_context_t* context)
{
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(numa_0[context->thread_id], &mask);

    if (pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask) < 0) {
        printf("threadpool, set thread affinity failed.\n");
    }

    uint64_t _start = context->base;
    size_t _end = _start + context->size;
    uint32_t _bs = context->bs;
    uint64_t _src = (uint64_t)malloc(_bs);

    Timer _timer;
    _timer.Start();
    for (int i = 0; i < g_num_loop; i++) {
        uint64_t _dest = _start;
        while (_dest < _end) {
            pmem_memcpy_persist((void*)_dest, (void*)_src, _bs);
            // pmem_memmove_nodrain((void*)_dest, (void*)_src, _bs);
            _dest += _bs;
        }
    }
    _timer.Stop();

    size_t _sz = g_num_loop * context->size;
    size_t _cnt = _sz / _bs;
    double _sec = _timer.GetSeconds();
    double _lat = _timer.Get() / _cnt;
    printf("[%d][cost:%.2fseconds][cnt:%zu][lat:%.2fns][iops:%.2f][bw:%.2fMB/s]\n",
        context->thread_id, _sec, _cnt, _lat, 1.0 * _cnt / _sec, 1.0 * _sz / (_sec * 1024UL * 1024));
}

int main(int argc, char** argv)
{
    int _is_pmem;
    size_t _len = 8UL * 1024 * 1024 * 1024;
    char _path[128] = "/home/pmem0/libpmem-demo";
    void* _base = pmem_map_file(_path, _len, PMEM_FILE_CREATE, 0666, &_len, &_is_pmem);
    if (_base != nullptr) {
        printf("[pmem_map_file][addr:0x%llx][len:%.2fGB][is_pmem:%d]\n", (uint64_t)_base, 1.0 * _len / (1024UL * 1024 * 1024), _is_pmem);
    }

    worker_context_t _ctxs[64];
    std::thread _mthreads[64];
    for (int i = 0; i < g_num_thread; i++) {
        _ctxs[i].thread_id = i;
        _ctxs[i].base = (uint64_t)_base + (i * _ctxs[i].size);
        _ctxs[i].size = _len / g_num_thread;
        _ctxs[i].bs = g_block_size;
        _mthreads[i] = std::thread(random_write, &_ctxs[i]);
    }

    for (int i = 0; i < g_num_thread; i++) {
        _mthreads[i].join();
    }
    pmem_unmap(_base, _len);
    return 0;
}