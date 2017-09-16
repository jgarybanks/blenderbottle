#include "memory.h"

void
_free_mem(void *mem, void *info)
{
	if (info) {
		BB_LOG(100, "free [%p] msg %s\n", mem, info);
		free(info);
	} else {
		BB_LOG(0, "GC asked to free NULL\n");
	}
}
		
void *
_alloc_mem(size_t sz, char *file, int line)
{
        char *info;

        info = calloc(sizeof(char), MEMINFO_SIZE);
        char *m = GC_MALLOC(sz * count);

        if (m) {
                snprintf(info, MEMINFO_SIZE, "file[%s] line[%d]", file, line);
                GC_register_finalizer(m, _collect_mem, (void*)info, NULL, NULL);
                BB_LOG(100, "alloc [%p]\n", m);
        }
        return m;
}

void
bb_mem_event(GC_EventType e)
{
	switch (e) {
	case GC_EVENT_START:
		BB_LOG(10, "GC start of collection\n");
		break;
	case GC_EVENT_MARK_START:
		BB_LOG(10, "GC mark memory start\n");
		break;
	case GC_EVENT_MARK_END:
		BB_LOG(10, "GC mark memory end\n");
		break;
	case GC_EVENT_RECLAIM_START:
		BB_LOG(10, "GC reclaim memory start\n");
		break;
	case GC_EVENT_RECLAIM_END:
		BB_LOG(10, "GC reclaim memory end\n");
		break;
	case GC_EVENT_END:
		BB_LOG(10, "GC end of collection\n");
		break;
	case GC_EVENT_PRE_STOP_WORLD:
		BB_LOG(10, "GC pre stop world event\n");
		break;
	case GC_EVENT_POST_STOP_WORLD:
		BB_LOG(10, "GC post stop world event\n");
		break;
	case GC_EVENT_PRE_START_WORLD:
		BB_LOG(10, "GC pre start world collection\n");
		break;
	case GC_EVENT_POST_START_WORLD:
		BB_LOG(10, "GC post start world collection\n");
		break;
	case GC_EVENT_THREAD_SUSPENDED:
		BB_LOG(10, "GC suspending collection thread\n");
		break;
	case GC_EVENT_THREAD_UNSUSPENDED:
		BB_LOG(10, "GC waking collection thread\n");
		break;
	default:
		BB_LOG(0, "GC unknown event %d\n", (int)e);
		break;
	}
}
