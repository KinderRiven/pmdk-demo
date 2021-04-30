/*
 * @Author: your name
 * @Date: 2021-04-30 13:55:45
 * @LastEditTime: 2021-04-30 14:21:13
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /pmdk-demo/libpmem_demo.cc
 */

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