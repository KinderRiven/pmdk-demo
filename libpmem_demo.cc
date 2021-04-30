/*
 * @Author: your name
 * @Date: 2021-04-30 13:55:45
 * @LastEditTime: 2021-04-30 16:36:49
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
#include "persist.h"
#include "timer.h"

struct worker_context_t {
public:
    uint32_t thread_id;
    uint64_t base;
    size_t size;
    uint32_t bs;

public:
    double bw;
    double lat;
};

static int numa_0[] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 21, 22, 23, 24, 25, 26, 27, 28, 29 //, 11, 13, 15, 17, 19, 31, 33,35, 37, 39
};

static uint32_t g_num_loop = 5;

static uint32_t g_num_thread = 4;

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
    uint64_t _src = (uint64_t)aligned_alloc(256UL, _bs);

    printf("[rand][%d][0x%llx][bs:%dB][loop:%d][size:%.2fMB]\n", context->thread_id, _start, _bs, g_num_loop, 1.0 * context->size / (1024UL * 1024));
    Timer _timer;
    _timer.Start();
    for (int i = 0; i < g_num_loop; i++) {
        uint64_t _dest = _start;
        while (_dest < _end) {
            nontemporal_store((char*)_dest, (char*)_src, _bs);
            // pmem_memcpy_persist((void*)_dest, (void*)_src, _bs);
            // pmem_memmove_persist((void*)_dest, (void*)_src, _bs);
            // pmem_memmove_nodrain((void*)_dest, (void*)_src, _bs);
            _dest += _skip;
            // assert(_dest % 64 == 0);
            // assert(_src % 64 == 0);
        }
    }
    _timer.Stop();

    size_t _sz = g_num_loop * context->size / 4;
    size_t _cnt = _sz / _bs;
    double _sec = _timer.GetSeconds();
    double _lat = _timer.Get() / _cnt;
    double _iops = 1.0 * _cnt / _sec;
    double _bw = 1.0 * _sz / (_sec * 1024UL * 1024);
    printf("[%d][cost:%.2fseconds][cnt:%zu][lat:%.2fns][iops:%.2f][bw:%.2fMB/s]\n",
        context->thread_id, _sec, _cnt, _lat, _iops, _bw);
    context->lat = _lat;
    context->bw = _bw;
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
    uint64_t _src = (uint64_t)aligned_alloc(256UL, _bs);

    printf("[seq][%d][0x%llx][bs:%dB][loop:%d][size:%.2fMB]\n", context->thread_id, _start, _bs, g_num_loop, 1.0 * context->size / (1024UL * 1024));
    Timer _timer;
    _timer.Start();
    for (int i = 0; i < g_num_loop; i++) {
        uint64_t _dest = _start;
        while (_dest < _end) {
            nontemporal_store((char*)_dest, (char*)_src, _bs);
            // pmem_memcpy_persist((void*)_dest, (void*)_src, _bs);
            // pmem_memmove_persist((void*)_dest, (void*)_src, _bs);
            // pmem_memmove_nodrain((void*)_dest, (void*)_src, _bs);
            _dest += _bs;
            // assert(_dest % 64 == 0);
            // assert(_src % 64 == 0);
        }
    }
    _timer.Stop();

    size_t _sz = g_num_loop * context->size;
    size_t _cnt = _sz / _bs;
    double _sec = _timer.GetSeconds();
    double _lat = _timer.Get() / _cnt;
    double _iops = 1.0 * _cnt / _sec;
    double _bw = 1.0 * _sz / (_sec * 1024UL * 1024);
    printf("[%d][cost:%.2fseconds][cnt:%zu][lat:%.2fns][iops:%.2f][bw:%.2fMB/s]\n",
        context->thread_id, _sec, _cnt, _lat, _iops, _bw);
    context->lat = _lat;
    context->bw = _bw;
}

int main(int argc, char** argv)
{
    int _rand = 0;
    for (int i = 0; i < argc; i++) {
        char __junk;
        uint64_t __n;
        if (sscanf(argv[i], "--bs=%llu%c", &__n, &__junk) == 1) {
            g_block_size = __n;
        } else if (sscanf(argv[i], "--thread=%llu%c", &__n, &__junk) == 1) {
            g_num_thread = __n;
        } else if (sscanf(argv[i], "--rand=%llu%c", &__n, &__junk) == 1) {
            _rand = __n;
        } else if (i > 0) {
            printf("error (%s)!\n", argv[i]);
            return 0;
        }
    }

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
        _ctxs[i].size = _len / g_num_thread;
        _ctxs[i].base = (uint64_t)_base + (i * _ctxs[i].size);
        _ctxs[i].bs = g_block_size;
        if (_rand) {
            _mthreads[i] = std::thread(random_write, &_ctxs[i]);
        } else {
            _mthreads[i] = std::thread(seq_write, &_ctxs[i]);
        }
    }

    double _total_bw = 0;
    double _avg_lat = 0;
    for (int i = 0; i < g_num_thread; i++) {
        _mthreads[i].join();
        _total_bw += _ctxs[i].bw;
        _avg_lat += _ctxs[i].lat;
    }
    _avg_lat /= g_num_thread;
    printf("[%d][%.2fMB/s][%.2fns]\n", g_num_thread, _total_bw, _avg_lat);
    pmem_unmap(_base, _len);
    return 0;
}