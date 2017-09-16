#ifndef _BB_MEMORY_H
#define _BB_MEMORY_H

#include "conf.h"
#include "gc.h"

void *_alloc_mem(size_t, char *, char *);
void bb_mem_event(GC_EventType);

#define alloc_mem(S) _alloc_mem(S, __FILE__, __LINE__)
#define free_mem(S) do {} while (0)

#endif /* _BB_MEMORY_H */
