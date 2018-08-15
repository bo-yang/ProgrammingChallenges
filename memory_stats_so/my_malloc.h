#ifndef __MY_MALLOC_H__
#define __MY_MALLOC_H__

#ifndef DLL_PUBLIC
#define DLL_PUBLIC __attribute__ ((visibility("default")))
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    MALLOC_FUNC_MALLOC = 0,
    MALLOC_FUNC_CALLOC,
    MALLOC_FUNC_REALLOC,
    MALLOC_FUNC_FREE,

    MALLOC_FUNC_UNKNOWN
} func_type_t;

void * my_malloc(const char *file, int line, size_t size, func_type_t func_id);
void * my_calloc(const char *file, int line, size_t num, size_t size, func_type_t func_id);
void * my_realloc(const char *file, int line, void *p, size_t size, func_type_t func_id);
void my_free(const char *file, int line, void *p, func_type_t func_id);
void dump_stats();

#undef malloc
#define malloc(size) \
    my_malloc(__FILE__, __LINE__, (size), MALLOC_FUNC_MALLOC)
#undef calloc
#define calloc(count, size) \
    my_malloc(__FILE__, __LINE__, (count)*(size), MALLOC_FUNC_CALLOC)
#undef realloc
#define realloc(ptr, size) \
    my_realloc(__FILE__, __LINE__, (ptr), (size), MALLOC_FUNC_REALLOC)

#undef free
#define free(ptr) \
    my_free(__FILE__, __LINE__, (ptr), MALLOC_FUNC_FREE)

DLL_PUBLIC void sig_quit_handler(int sig);

#ifdef  __cplusplus
}
#endif
#endif /*__MY_MALLOC_H__*/
