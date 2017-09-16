#ifndef _LOG_H
#define _LOG_H

#include <limits.h>
#include <execinfo.h>
#include <syslog.h>

extern char *progname;

#define LOGL 3
#define LOGM 2
#define LOGH 1
#define LOGX 0

#define LOG_RING_MAX 4096	/* Pulled this out of my rear     */
#define LOG_MSG_MAX 4096

struct bb_logmsg {
	int  log_lvl;
	char *logmsg; /* TCP syslog RFC says must be < 4096 bytes */
	struct bb_logmsg *next;
};

struct bb_logmsg bb_logmsgs;
extern int bb_error_raised;
int ringlog_cnt;

extern int bb_debug_level;

void bb_log(int, char *, int, char *, ...);
void bb_nlog(int, char *, int, char *, ...);

#define BB_LOG(level, ...) do {					\
	bb_log(level, __FILE__, __LINE__, __VA_ARGS__);		\
	bb_nlog(level, __FILE__, __LINE__, __VA_ARGS__);	\
} while (0)

#define LOG_V 2000
#define LOG_S 3000

void __attribute__ ((destructor)) dump_nlog(void);

/*
** Shorted macros are provided because I don't like to type.
*/

#define VERBOSE_LOG(msg) do {						\
	if (bb_dbg_level >= LOG_V) {					\
		fprintf(stderr, " VERBOSE_LOG : %s ", msg);		\
		fprintf(stderr, "%s %d\n", __FILE__,  __LINE__);	\
	}								\
} while (0)

#define VLOG(msg) do { VERBOSE_LOG(msg); } while (0)

#define MAX_STACK 100
void stack_log(void);

#define STACK_LOG(msg) do {						\
	if (bb_dbg_level >= LOG_S) {					\
		fprintf(stderr, "STACK_LOG : %s", msg);			\
		fprintf(stderr, "%s %s %d",				\
			__DATE__, __FILE__, __LINE__);			\
		stack_log();						\
	}								\
} while(0)

#define SLOG(msg) do { STACK_LOG(msg); } while (0)

#define _F(func, ...) ({VLOG("Calling " #func); func;})
#define CF(func, ...) _F(func, __VA_ARGS__)
#define CFUNC(func, ...) _F(func, __VA_ARGS__)
#define FUNCTION(func, ...) _F(func, __VA_ARGS__) 

#define ENTER_FUNCTION do {						\
	char msg[MAX_STACK];						\
	sprintf(msg, "%s %s\n", "Entering ", __func__);			\
	VLOG(msg);							\
} while (0)

#define LEAVE_FUNCTION do {						\
	char msg[MAX_STACK];						\
	sprintf(msg, "%s %s\n", "Leaving", __func__);			\
	VLOG(msg);							\
} while (0)


void init_logging(void);
void fini_logging(void);

#endif
