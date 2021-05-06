/*
 * @Author: your name
 * @Date: 2021-04-30 13:55:45
 * @LastEditTime: 2021-05-06 18:41:02
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

static double g_run_time = 30.0;
static uint32_t g_num_thread = 4;
static uint32_t g_block_size = 64;

typedef void* (*fun_write)(void*, const void*, size_t);

static void random_write(worker_context_t* context)
{
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(numa_0[context->thread_id], &mask);

    if (pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask) < 0) {
        printf("threadpool, set thread affinity failed.\n");
    }

    uint64_t _cnt = 0;
    uint64_t _start = context->base;
    uint32_t _bs = context->bs;
    size_t _end = (_start + context->size - _bs);
    uint32_t _skip = _bs * 4;
    uint64_t _src = (uint64_t)aligned_alloc(256UL, _bs);

    // fun_write _fun = nontemporal_store;
    fun_write _fun = pmem_memcpy_persist;

    // if (_bs < 256) {
    //     _fun = nontemporal_store;
    // } else {
    //     _fun = pmem_memcpy_persist;
    // }

    printf("[rw][%d][0x%llx][bs:%dB][size:%.2fMB]\n", context->thread_id, _start, _bs, 1.0 * context->size / (1024UL * 1024));
    Timer _timer;
    _timer.Start();
    while (true) {
        uint32_t __one_loop_cnt = 0;
        uint64_t _dest = _start;
        while (_dest < _end) {
            _fun((char*)_dest, (char*)_src, _bs);
            _dest += _skip;
            __one_loop_cnt++;
        }
        _cnt += __one_loop_cnt;
        _timer.Stop();
        if (_timer.GetSeconds() > g_run_time) {
            break;
        }
    }
    _timer.Stop();

    size_t _sz = _cnt * _bs;
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

    uint64_t _cnt = 0;
    uint64_t _start = context->base;
    uint32_t _bs = context->bs;
    size_t _end = (_start + context->size - _bs);
    uint32_t _skip = _bs * 4;
    uint64_t _src = (uint64_t)aligned_alloc(256UL, _bs);

    // fun_write _fun = nontemporal_store;
    fun_write _fun = pmem_memcpy_persist;

    // if (_bs < 256) {
    //     _fun = nontemporal_store;
    // } else {
    //     _fun = pmem_memcpy_persist;
    // }

    printf("[sw][%d][0x%llx][bs:%dB][size:%.2fMB]\n", context->thread_id, _start, _bs, 1.0 * context->size / (1024UL * 1024));
    Timer _timer;
    _timer.Start();
    while (true) {
        uint32_t __one_loop_cnt = 0;
        uint64_t _dest = _start;
        while (_dest < _end) {
            _fun((char*)_dest, (char*)_src, _bs);
            _dest += _bs;
            __one_loop_cnt++;
        }
        _cnt += __one_loop_cnt;
        _timer.Stop();
        if (_timer.GetSeconds() > g_run_time) {
            break;
        }
    }
    _timer.Stop();

    size_t _sz = _cnt * _bs;
    double _sec = _timer.GetSeconds();
    double _lat = _timer.Get() / _cnt;
    double _iops = 1.0 * _cnt / _sec;
    double _bw = 1.0 * _sz / (_sec * 1024UL * 1024);
    printf("[%d][cost:%.2fseconds][cnt:%zu][lat:%.2fns][iops:%.2f][bw:%.2fMB/s]\n",
        context->thread_id, _sec, _cnt, _lat, _iops, _bw);
    context->lat = _lat;
    context->bw = _bw;
}

static void random_read(worker_context_t* context)
{
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(numa_0[context->thread_id], &mask);

    if (pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask) < 0) {
        printf("threadpool, set thread affinity failed.\n");
    }

    uint64_t _cnt = 0;
    uint64_t _start = context->base;
    uint32_t _bs = context->bs;
    size_t _end = (_start + context->size - _bs);
    uint32_t _skip = _bs * 4;
    uint64_t _src = (uint64_t)aligned_alloc(256UL, _bs);

    printf("[rr][%d][0x%llx][bs:%dB][size:%.2fMB]\n", context->thread_id, _start, _bs, 1.0 * context->size / (1024UL * 1024));
    Timer _timer;
    _timer.Start();
    while (true) {
        uint32_t __one_loop_cnt = 0;
        uint64_t _dest = _start;
        while (_dest < _end) {
            memcpy((char*)_src, (char*)_dest, _bs);
            _dest += _skip;
            __one_loop_cnt++;
        }
        _cnt += __one_loop_cnt;
        _timer.Stop();
        if (_timer.GetSeconds() > g_run_time) {
            break;
        }
    }
    _timer.Stop();

    size_t _sz = _cnt * _bs;
    double _sec = _timer.GetSeconds();
    double _lat = _timer.Get() / _cnt;
    double _iops = 1.0 * _cnt / _sec;
    double _bw = 1.0 * _sz / (_sec * 1024UL * 1024);
    printf("[%d][cost:%.2fseconds][cnt:%zu][lat:%.2fns][iops:%.2f][bw:%.2fMB/s]\n",
        context->thread_id, _sec, _cnt, _lat, _iops, _bw);
    context->lat = _lat;
    context->bw = _bw;
}

static void seq_read(worker_context_t* context)
{
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(numa_0[context->thread_id], &mask);

    if (pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask) < 0) {
        printf("threadpool, set thread affinity failed.\n");
    }

    uint64_t _cnt = 0;
    uint64_t _start = context->base;
    uint32_t _bs = context->bs;
    size_t _end = (_start + context->size - _bs);
    uint32_t _skip = _bs * 4;
    uint64_t _src = (uint64_t)aligned_alloc(256UL, _bs);

    printf("[sr][%d][0x%llx][bs:%dB][size:%.2fMB]\n", context->thread_id, _start, _bs, 1.0 * context->size / (1024UL * 1024));
    Timer _timer;
    _timer.Start();
    while (true) {
        uint32_t __one_loop_cnt = 0;
        uint64_t _dest = _start;
        while (_dest < _end) {
            memcpy((char*)_src, (char*)_dest, _bs);
            _dest += _skip;
            __one_loop_cnt++;
        }
        _cnt += __one_loop_cnt;
        _timer.Stop();
        if (_timer.GetSeconds() > g_run_time) {
            break;
        }
    }
    _timer.Stop();

    size_t _sz = _cnt * _bs;
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
    char _type[64] = "rw";
    for (int i = 0; i < argc; i++) {
        char __junk;
        uint64_t __n;
        if (sscanf(argv[i], "--bs=%llu%c", &__n, &__junk) == 1) {
            g_block_size = __n;
        } else if (sscanf(argv[i], "--thread=%llu%c", &__n, &__junk) == 1) {
            g_num_thread = __n;
        } else if (strncmp(argv[i], "--type=", 7) == 0) {
            strcpy(_type, argv[i] + 7);
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
    printf("------------------------------------------\n");
    printf("[num_thread:%d][bs:%d]\n", g_num_thread, g_block_size);
    worker_context_t _ctxs[64];
    std::thread _mthreads[64];
    for (int i = 0; i < g_num_thread; i++) {
        _ctxs[i].thread_id = i;
        _ctxs[i].size = _len / g_num_thread;
        _ctxs[i].base = (uint64_t)_base + (i * _ctxs[i].size);
        _ctxs[i].bs = g_block_size;
        if (memcmp(_type, "rw", 2) == 0) {
            _mthreads[i] = std::thread(random_write, &_ctxs[i]);
        } else if (memcmp(_type, "sw", 2) == 0) {
            _mthreads[i] = std::thread(seq_write, &_ctxs[i]);
        } else if (memcmp(_type, "rr", 2) == 0) {
            _mthreads[i] = std::thread(random_read, &_ctxs[i]);
        } else if (memcmp(_type, "sr", 2) == 0) {
            _mthreads[i] = std::thread(seq_read, &_ctxs[i]);
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
    printf("------------------------------------------\n");
    return 0;
}