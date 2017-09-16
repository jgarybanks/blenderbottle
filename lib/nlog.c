#include "nlog.h"

/*
** This code does not use garbage collected allocation and free, as it makes
** use of asprintf, which has no analog. Feel free to write one.
*/
static int
inet_socket(const char *servername, const char *port, int *socket_type)
{
	int fd, errcode, i, type = -1;
	struct addrinfo hints, *res;
	const char *p = port;

	memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = SOCK_STREAM;
	type = TYPE_TCP;
	if (port == NULL)
		p = "syslog-conn";

	hints.ai_family = AF_UNSPEC;
	errcode = getaddrinfo(servername, p, &hints, &res);
	if (errcode != 0) {
		fprintf(stderr, "failed to resolve name %s port %s: %s",
		     servername, p, strerror(errcode));
		return -1;
	}
	if ((fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
		freeaddrinfo(res);
		fprintf(stderr, "Unable to get socket for name %s port %s: %s",
		     servername, p, strerror(errno));
		return -1;
	}
	if (connect(fd, res->ai_addr, res->ai_addrlen) == -1) {
		fprintf(stderr, "Unable to connect : %s\n", strerror(errno));
		freeaddrinfo(res);
		return -1;
	}
	freeaddrinfo(res);

	if (fd == -1) {
		fprintf(stderr, "failed to connect to %s port %s", servername, p);
		return -1;
	}

	*socket_type = TYPE_TCP;

	return fd;
}

#define next_iovec(ary, idx) __extension__ ({		\
		assert(idx >= 0);			\
		&ary[idx++];				\
})

#define iovec_add_string(ary, idx, str, len)		\
	do {						\
		struct iovec *v = next_iovec(ary, idx);	\
		v->iov_base = (void *) str;		\
		v->iov_len = len ? len : strlen(str);	\
	} while (0)

#define iovec_memcmp(ary, idx, str, len)		\
		memcmp((ary)[(idx) - 1].iov_base, str, len)

/* writes generated buffer to desired destination. For TCP syslog,
 * we use RFC6587 octet-stuffing (unless octet-counting is selected).
 * This is not great, but doing full blown RFC5425 (TLS) looks like
 * it is too much for the logger utility. If octet-counting is
 * selected, we use that.
 */
static void
write_output(const struct nlogger *ctl, const char *const msg)
{
	struct iovec iov[4];
	int iovlen = 0;
	char *octet = NULL;
	const char *cp = msg;
	int end;
	
	while (*cp && *cp == '\n') {
		cp++;
	}
	end = strlen(cp);

	/* 1) octen count */
	if (ctl->octet_count) {
		size_t len = asprintf(&octet, "%zu ", strlen(ctl->hdr) + end);
		iovec_add_string(iov, iovlen, octet, len);
	}

	/* 2) header */
	iovec_add_string(iov, iovlen, ctl->hdr, 0);

	/* 3) message */
	iovec_add_string(iov, iovlen, cp, 0);

	if (!ctl->noact) {
		struct msghdr message = { 0 };

		/* 4) add extra \n to make sure message is terminated */
		if ((ctl->socket_type == TYPE_TCP) && !ctl->octet_count) {
			if (cp[end] != '\n') {
				iovec_add_string(iov, iovlen, "\n", 1);
			}
		}

		message.msg_iov = iov;
		message.msg_iovlen = iovlen;

		if (sendmsg(ctl->fd, &message, 0) < 0)
			fprintf(stderr, "send message failed on %d: %s\n",
					ctl->fd, strerror(errno));
	}

	if (ctl->stderr_printout) {
		/* make sure it's terminated for stderr */
		if (iovec_memcmp(iov, iovlen, "\n", 1) != 0)
			iovec_add_string(iov, iovlen, "\n", 1);

		writev(STDERR_FILENO, iov, iovlen);
	}

	free(octet);
}

#define ADDMSEC(A, B) do {	\
	int o = (B) % 1000;	\
	A.tv_sec += o;		\
	A.tv_usec += (B) - 1000 * o; \
} while (0)

/* Some field mappings may be controversial, thus I give the reason
 * why this specific mapping was used:
 * APP-NAME <-- tag
 *    Some may argue that "logger" is a better fit, but we think
 *    this is better inline of what other implementations do. In
 *    rsyslog, for example, the TAG value is populated from APP-NAME.
 * PROCID <-- pid
 *    This is a relatively straightforward interpretation from
 *    RFC5424, sect. 6.2.6.
 * MSGID <-- msgid (from --msgid)
 *    One may argue that the string "logger" would be better suited
 *    here so that a receiver can identify the sender process.
 *    However, this does not sound like a good match to RFC5424,
 *    sect. 6.2.7.
 * Note that appendix A.1 of RFC5424 does not provide clear guidance
 * of how these fields should be used. This is the case because the
 * IETF working group couldn't arrive at a clear agreement when we
 * specified RFC5424. The rest of the field mappings should be
 * pretty clear from RFC5424. -- Rainer Gerhards, 2015-03-10
 */
static void
syslog_rfc5424_header(struct nlogger *const ctl)
{
	char *time;
	char hostname[NAME_MAX];
	char *const app_name = ctl->tag;
	char *procid;
	char *const msgid = strdup(ctl->msgid ? ctl->msgid : NILVALUE);
	struct list_head *sd;

	if (ctl->fd < 0)
		return;

	if (ctl->rfc5424_time) {
		struct timeval tv;
		struct tm *tm;

		logger_gettimeofday(&tv, NULL);
		if ((tm = localtime(&tv.tv_sec)) != NULL) {
			char fmt[64];
			const size_t i = strftime(fmt, sizeof(fmt),
						  "%Y-%m-%dT%H:%M:%S.%%06u%z ", tm);
			/* patch TZ info to comply with RFC3339 (we left SP at end) */
			fmt[i - 1] = fmt[i - 2];
			fmt[i - 2] = fmt[i - 3];
			fmt[i - 3] = ':';
			asprintf(&time, fmt, tv.tv_usec);
		} else {
			fprintf(stderr, "localtime() failed");
			return;
		}
	} else
		time = strdup(NILVALUE);

	if (ctl->rfc5424_host) {
		gethostname(hostname, NAME_MAX);
	} else
		strcpy(hostname, NILVALUE);

	if (48 < strlen(ctl->tag)) {
		fprintf(stderr, "tag '%s' is too long", ctl->tag);
		return;
	}

	if (ctl->pid)
		asprintf(&procid, "%d", ctl->pid);
	else
		procid = strdup(NILVALUE);

	asprintf(&ctl->hdr, "<%d>1 %s %s %s %s %s ",
		ctl->pri,
		time,
		hostname,
		app_name,
		procid,
		msgid);

	free(time);
	/* app_name points to ctl->tag, do NOT free! */
	free(procid);
	free(msgid);
	ctl->inc += 1;
}

static void
generate_syslog_header(struct nlogger *const ctl)
{
	if (ctl->hdr) {
		free(ctl->hdr);
		ctl->hdr = NULL;
	}
	free(ctl->hdr);
	ctl->syslogfp(ctl);
}

static void
logger_open(char *progname, struct nlogger *ctl)
{
	ctl->fd = inet_socket(ctl->server, ctl->port, &ctl->socket_type);
	if (!ctl->syslogfp) {
		ctl->syslogfp = syslog_rfc5424_header;
	}
	if (!ctl->tag) {
		ctl->tag = progname;
	}
	generate_syslog_header(ctl);
}

void
nlog_close(const struct nlogger *ctl)
{
	if (ctl->fd != -1 && close(ctl->fd) != 0) {
		fprintf(stderr, "close failed");
		return;
	}
	if (ctl->hdr) {
		free(ctl->hdr);
		ctl->hdr = NULL;
	}
}

struct nlogger *
nlog_open(char *progname, char *server, char *port)
{
	struct nlogger *ctl;
	
	ctl = alloc_mem(sizeof(*ctl));
	ctl->fd = -1;
	ctl->pid = getpid();
	ctl->pri = LOG_LOCAL7 | LOG_DEBUG;
	ctl->prio_prefix = 0;
	ctl->inc = 0;
	ctl->tag = progname;
	ctl->server = server;
	ctl->port = port;
	ctl->max_message_size = LOG_LEN_MAX;
	ctl->socket_type = TYPE_TCP;
	ctl->hdr = NULL;
	ctl->msgid = NULL;
	ctl->socket_type = ALL_TYPES;
	ctl->rfc5424_time = 1;
	ctl->rfc5424_tq = 1;
	ctl->rfc5424_host = 1;
	ctl->skip_empty_lines = 0;

	logger_open(progname, ctl);
	return ctl;
}

void
nlog_msg(struct nlogger *ctl, struct bb_logmsg *logmsg)
{
	char msg[LOG_LEN_MAX];

	if (ctl->fd == -1) {
		return;
	}
	if (!logmsg) {
		return;
	}

	snprintf(msg, LOG_LEN_MAX, "%d LVL[%d] %s",
			ctl->inc, logmsg.log_lvl, logmsg->logmsg);
	generate_syslog_header(ctl);
	write_output(ctl, msg);
}
