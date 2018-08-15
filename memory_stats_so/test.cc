#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <list>
#include <thread>
#include <mutex>
#include <chrono>
#include <signal.h>
#include "my_malloc.h"

std::mutex mtx;
std::list<void*> pointer_buffer;
size_t alloc_cnt = 0;
size_t free_cnt = 0;

void memory_allocater()
{
    while(true) {
        int sz = rand()%5000+1;
        void * ptr = malloc(sz);
        if (ptr != NULL) {
            mtx.lock();
            pointer_buffer.push_back(ptr);
            alloc_cnt++;
            mtx.unlock();
        }
        if (alloc_cnt % 1000 == 0)
            std::cout << std::this_thread::get_id() << ": " << alloc_cnt << " allocated." << std::endl;
        std::this_thread::sleep_for (std::chrono::milliseconds(rand()%50+1));
    }
}

void memory_freer()
{
    while(true) {
        mtx.lock();
        if (pointer_buffer.size()) {
            auto ptr = pointer_buffer.front();
            free(ptr);
            pointer_buffer.pop_front();
            free_cnt++;
        }
        mtx.unlock();
        if (free_cnt % 1000 == 0)
            std::cout << std::this_thread::get_id() << ": " << free_cnt << " freed." << std::endl;
        std::this_thread::sleep_for (std::chrono::milliseconds(rand()%80+1));
    }
}

int main()
{
    srand(time(NULL));

    struct sigaction sa;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = sig_quit_handler;
    if (sigaction(SIGQUIT, &sa, NULL) == -1) {
        perror("Failed to set SIGQUIT handler.");
        exit(1);
    }

    const int nthreads = 3;
    std::list<std::thread> thread_alloc, thread_free;
    for (int i = 0; i < nthreads; ++i) {
        thread_alloc.push_back(std::thread(memory_allocater));
        thread_free.push_back(std::thread(memory_freer));
    }

    for (auto& thrd : thread_alloc)
        thrd.join();
    for (auto& thrd : thread_free)
        thrd.join();

    return 0;
}
