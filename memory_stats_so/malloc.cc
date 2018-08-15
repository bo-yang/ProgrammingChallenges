#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include <signal.h>
#include "malloc.h"
#include "my_malloc.h"
#include "readwritelock.hh"

Allocation allocs;  // current allocations
OverallStat ovstat; // record overall stats
ReadWriteLock rwlck;

static inline std::string alloc_src(const char *file, int line)
{
    return std::string(file) + ":" + std::to_string(line);
}

void * my_malloc(const char *file, int line, size_t size, func_type_t func_id)
{
    rwlck.write_lock();

    void * ptr = (void*)new char[size];
    if (!ptr) {
        rwlck.write_unlock();
        return NULL;
    }

    AllocAttrib attr(size, alloc_src(file,line), func_id);
    allocs[ptr] = attr;

    ovstat.alloc_cnt++;
    ovstat.alloc_size += size;

    rwlck.write_unlock();
    return ptr;
}

void * my_calloc(const char *file, int line, size_t num, size_t size, func_type_t func_id)
{
    size_t sz = size * num;
    void * ptr = my_malloc(file, line, sz, func_id);
    if (!ptr)
        return NULL;

    memset(ptr, 0, sz); // initialize all bits to zero
    return ptr;
}

void * my_realloc(const char *file, int line, void *p, size_t size, func_type_t func_id)
{
    rwlck.write_lock();

    AllocAttrib attr;
    if (allocs.count(p) > 0)
        attr = allocs[p];

    delete [] (char*)p; // always free p first
    void * ptr = (void*)new char[size];
    if (!ptr) {
        rwlck.write_unlock();
        return NULL;
    }

    // update overall stats
    ovstat.alloc_cnt++; // TODO: differentiate malloc and realloc
    ovstat.alloc_size += (int)((int)size - (int)attr.size);

    // update the stat of this allocation - keep timestamp unchanged
    attr.size = size;
    attr.src = alloc_src(file,line);
    attr.func = func_id;
    allocs[ptr] = attr;

    rwlck.write_unlock();
    return ptr;
}

void my_free(const char *file, int line, void *p, func_type_t func_id)
{
    rwlck.write_lock();

    if (allocs.count(p) > 0) {
        ovstat.free_cnt++;
        ovstat.free_size += allocs[p].size;
        allocs.erase(p);
    }
    delete [] (char*)p;

    rwlck.write_unlock();
}

static std::vector<size_t> curr_alloc_by_size()
{
    // Count allocations by memory size
    //0 - 4 bytes: ##########
    //4 - 8 bytes:
    //8 - 16 bytes: ####
    //16 - 32 bytes:
    //32 - 64 bytes:
    //64 - 128 bytes:
    //128 - 256 bytes:
    //256 - 512 bytes: ##
    //512 - 1024 bytes: #
    //1024 - 2048 bytes: #
    //2048 - 4096 bytes: #
    //4096 + bytes: #######
    std::vector<size_t> stat(12, 0);
    auto idx = [&](size_t x) -> size_t {
        size_t i = 0, prod = 4;
        if (x <= 4)
            return i;
        if (x >= 4096)
            return stat.size()-1;
        while(prod < x) {
            prod = prod << 1;
            i++;
        }
        return i;
    };

    rwlck.read_lock();
    for (const auto& a : allocs)
        stat[idx(a.second.size)]++;
    rwlck.read_unlock();

    return stat;
}

static std::vector<size_t> curr_alloc_by_age()
{
    //< 1 sec: ###
    //< 10 sec: ##
    //< 100 sec: ##
    //< 1000 sec: #########################
    //> 1000 sec:
    std::vector<size_t> stat(5,0);
    auto idx = [&](time_t end, time_t begin) -> size_t {
        int secs = (int)difftime(end, begin);
        if (secs < 1)
            return 0;
        if (secs >= 1000)
            return stat.size()-1;
        size_t i = 0;
        while (secs > 0) {
            secs /= 10;
            i++;
        }
        return i;
    };

    rwlck.read_lock();
    for (const auto& a : allocs)
        stat[idx(a.second.ts, ovstat.epoch)]++;
    rwlck.read_unlock();

    return stat;
}

void dump_stats()
{
    size_t curr_alloc_size = 0;
    OverallStat tmpstat; // local copy of the overallstats

    rwlck.read_lock();
    tmpstat = ovstat;
    for (const auto& a : allocs)
        curr_alloc_size += a.second.size;
    rwlck.read_unlock();

    time_t t = time(NULL);
    char* ptm = asctime(localtime(&t));
    std::string timestr = std::string(ptm);
    if (!timestr.empty() && timestr[timestr.length()-1] == '\n')
        timestr.erase(timestr.length()-1); // remove new line
    fprintf(stderr, ">>>>>>>>>>>>> %s <<<<<<<<<<<\n", timestr.c_str());
    fprintf(stderr, "Overall stats:\n");
    fprintf(stderr, "%u overall allocations(%u MB) since start\n", tmpstat.alloc_cnt, tmpstat.alloc_size/(1024*1024));
    fprintf(stderr, "%u MB current total allocated size\n", curr_alloc_size/(1024*1024));

    fprintf(stderr, "\nCurrent allocations by size:\n");
    auto alloc_stats = curr_alloc_by_size();
    int prod = 4;
    fprintf(stderr, "%d - %d bytes: %u\n", 0, prod, alloc_stats[0]);
    size_t idx;
    for (idx = 1; idx < alloc_stats.size()-1; ++idx) {
        fprintf(stderr, "%d - %d bytes: %u\n", prod << idx-1, prod << idx, alloc_stats[idx]);
    }
    fprintf(stderr, "%d + bytes: %u\n", prod << (idx-1), alloc_stats[idx]);

    fprintf(stderr,"\nCurrent allocations by age:\n");
    alloc_stats = curr_alloc_by_age();
    prod = 1;
    for (idx = 0; idx < alloc_stats.size()-1; ++idx) {
        fprintf(stderr, "< %d sec: %u\n", prod, alloc_stats[idx]);
        prod *= 10;
    }
    fprintf(stderr, "> %d sec: %u\n", prod, alloc_stats[idx]);
}

void sig_quit_handler(int sig)
{
    fprintf(stderr, "Received signal %d\n", sig);
    dump_stats();
}
