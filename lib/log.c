#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include "dateTime.h"
#include "tchLog.h"
#include "dblogging.h"

#include "conf.h"
#include "log.h"
#include "memory.h"

int bb_dbg_level = 0;
struct bb_logmsg *curr_msg = &bb_logmsgs;
int bb_error_raised = 0;

void
bb_loglower(int lvl)
{
	int i = 0;
	char *cp = NULL;
	struct nlogger *ctl = NULL;
	struct bb_logmsg *logmsg = bb_logmsgs.next;

	cp = getenv("PROGNAME") ?
		getenv("PROGNAME") : progname;

	if (!cp) {
		return;
	}
	cp = strrchr(cp, '/');
	if (cp) {
		cp++;
	} else {
		cp = getenv("PROGNAME") ?
			getenv("PROGNAME") : progname;
	}

	ctl = nlog_open(cp, "localhost", "514");
		
	for (logmsg && logmsg != curr_msg; logmsg = logmsg->next) {
	for (i = 0; i < ringlog_cnt; i++) {
		if (bb_error_raised || bb_logmsgs->log_lvl <= lvl) {
			nlog_msg(ctl, logmsg);
		}
	}
	nlog_close(ctl);
	free_mem(ctl);

}

void
bb_dumplog(void)
{
	bb_loglower(INT_MAX);
}

void
bb_nlog(int log_level, char *fn, int line, char *format, ...)
{
	char cp[LOG_MSG_MAX];
	va_list va_cpy;
	struct bb_logmsg *bb_logmsg;

	/*
	** We save *everything*, so that if things go south we
	** can print all the things upstream that might have been
	** of interest.
	*/
	if (ringlog_cnt == LOG_RING_MAX) {
		fprintf(stderr, "LOG dumping early at %d\n", ringlog_cnt);
		bb_loglower(bb_debug_level);
		ringlog_cnt = 0;
		curr_msg = &bb_logmsgs;
	}
	bb_logmsg = alloc_mem(sizeof(*bb_logmsg));
	bb_logmsg->log_lvl = log_level;

	va_start(va_cpy, format);

	cp += snprintf(cp, LOG_LEN_MAX, "[%s, %d] ", fn, line);
	vsnprintf(cp, LOG_LEN_MAX, format, va_cpy);

	va_end(va_cpy);
	ringlog_cnt++;

	bb_logmsg->logmsg = alloc_mem(strlen(cp));
	strncpy(bb_logmsg->logmsg, cp, LOG_MSG_MAX);
	curr_msg->next = bb_logmsg;
	curr_msg = bb_logmsg;
}

void
bb_log(int log_level, char *filename, int line, char *format, ...)
{
	char *cp;
	char time_string[7];
	int db_loglevel;

	va_list arguments;
	va_start(arguments, format);

	pid_t pid = getpid();

	fmt_dttime(set_curr_datetime(), "%H%M%S", time_string);

	if (isatty(STDERR_FILENO)) {
		vfprintf(stderr, format, arguments);
	} else {
		// critical error messages should be 0 so that they show up all the time.
		// depending on the severity of the message, the log_level should be higher.
		if (log_level <= bb_dbg_level) {
			fprintf(stderr, "%d(%s) [%s, %d] ", pid, time_string, filename, line);
			vfprintf(stderr, format, arguments);
		}
	}
	va_end(arguments);
}

/*
** Prints the stacktrace of the current process/thread(s)
*/
inline void
stack_log(void)
{
	ENTER_FUNCTION;
	int cnt;
	void *stack_ptrs[MAX_STACK];

	cnt = backtrace(stack_ptrs, MAX_STACK);
	if (cnt >= MAX_STACK) {
		fprintf(stderr, "(Truncated stack\n");
	}

	backtrace_symbols_fd(stack_ptrs, cnt, STDERR_FILENO);
	fprintf(stderr, "\n");
	LEAVE_FUNCTION;
}

void
init_logging(void)
{
	char *env_ptr = getenv("BB_DEBUG");

	bb_dbg_level = envPtr ? atoi(envPtr) : 0;

	if (bb_dbg_level > 10) {
		GC_set_on_collection_event(bb_mem_event);
	}


	ringlog_cnt = 0;
}

void
fini_logging(void)
{
	struct GC_prof_stats_s s;

	BB_LOG(0, "libBBShared %s exit\n", lib_bb_ver);

	GC_gcollect();

	if (bb_dbg_level >= 200) {
		GC_get_prof_stats(&s, sizeof(s));
		TCH_LOG(200, "GC heap size : %d\n", s.heapsize_full);
		TCH_LOG(200, "GC free bytes in heap : %d\n", s.free_bytes_full);
		TCH_LOG(200, "GC alloc since last gc : %d\n", s.bytes_allocd_since_gc);
		TCH_LOG(200, "GC non-reachable bytes : %d\n", s.non_gc_bytes);
		TCH_LOG(200, "GC gc cycle # : %d\n", s.gc_no);
	}

	return;
}
