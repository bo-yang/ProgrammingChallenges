#ifndef __MALLOC_H__
#define __MALLOC_H__

#include <ctime>
#include <cstdlib>
#include <string>
#include <unordered_map>
#include "my_malloc.h"

// Required Stats:
//  - overall allocations since start
//  - current total allocated memory(in MB)
//  - current allocation by size
//  - current allocatioj by age

// Memory allocation attribution
struct AllocAttrib {
    size_t size;        // in bytes
    time_t ts;     // timestamp
    std::string src;    // source file:line
    func_type_t func;   // function type

    AllocAttrib(size_t sz=0, std::string source="", func_type_t fid=MALLOC_FUNC_UNKNOWN):
        size(sz),src(source), func(fid) {
        ts = std::time(nullptr);
    }
};
typedef std::unordered_map<void*,AllocAttrib> Allocation;

struct OverallStat {
    size_t alloc_cnt;   // number of allocations
    size_t alloc_size;  // allocated memory size
    size_t free_cnt;    // number of frees
    size_t free_size;   // freed memory size
    std::time_t epoch;  // start time stamp

    OverallStat() {
        alloc_cnt = 0;
        alloc_size = 0;
        free_cnt = 0;
        free_size = 0;
        epoch = std::time(nullptr);
    }
};

#endif /*__MALLOC_H__*/
