#ifndef _NLOGGER_H
#define _NLOGGER_H

#define _GNU_SOURCE 1
#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <getopt.h>
#include <pwd.h>
#include <signal.h>
#include <sys/uio.h>

#include "tchLog.h"

#define NILVALUE "-"
#include <syslog.h>

#include <sys/timex.h>

#ifndef NAME_MAX
#define NAME_MAX 255
#endif

enum {
	TYPE_UDP = (1 << 1),
	TYPE_TCP = (1 << 2),
	ALL_TYPES = TYPE_UDP | TYPE_TCP
};

struct nlogger {
	int fd;
	int pri;
	int inc;
	pid_t pid;			/* zero when unwanted */
	char *hdr;			/* the syslog header (based on protocol) */
	char *tag;
	char *msgid;
	char *unix_socket;		/* -u <path> or default to _PATH_DEVLOG */
	char *server;
	char *port;
	int socket_type;
	size_t max_message_size;

	void (*syslogfp)(struct nlogger *ctl);

	unsigned int
			unix_socket_errors:1,	/* whether to report or not errors */
			noact:1,		/* do not write to sockets */
			prio_prefix:1,		/* read priority from input */
			stderr_printout:1,	/* output message to stderr */
			rfc5424_time:1,		/* include time stamp */
			rfc5424_tq:1,		/* include time quality markup */
			rfc5424_host:1,		/* include hostname */
			skip_empty_lines:1,	/* do not send empty lines when processing files */
			octet_count:1;		/* use RFC6587 octet counting */
};

#undef HAVE_NTP_GETTIME		/* force to default non-NTP */

#define logger_gettimeofday(x, y)	gettimeofday(x, y)
#define logger_xgethostname		xgethostname
#define logger_getpid			getpid

struct nlogger *nlog_open(char*, char*, char*);
void nlog_msg(struct nlogger*, struct bb_logmsg);
void nlog_close(const struct nlogger*);

#endif
