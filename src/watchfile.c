
typedef struct {
	char file_name[69];
	int fd_in;
	int *ismodified;
	pthread_mutex_t *m;
} Watch_Thread_Args;

static void *
watch_routine(void *arg)
{
	Watch_Thread_Args *tdata = arg;
	char buf[4096] __attribute__ ((aligned(__alignof__(struct inotify_event))));
	struct inotify_event *event;
	ssize_t len;

	while (1) {
		len = read(tdata->fd_in, buf, sizeof(buf));
		if (len < 0) {
			fprintf(ERR_LOG, "ERROR: read %s\n", strerror(errno));
			exit(1);
		}

		for (char *ptr = buf; ptr < buf + len;
				ptr += sizeof(struct inotify_event) + event->len) {
			event = (struct inotify_event *)ptr;
			if (event->len > 0 && event->mask & IN_CLOSE_WRITE) {
				if (strncmp(event->name, tdata->file_name, strlen(tdata->file_name) + 1) == 0) {
					pthread_mutex_lock(tdata->m);
					*tdata->ismodified = 1;
					pthread_mutex_unlock(tdata->m);
				}
			}
		}
	}
	return NULL;
}

static pthread_mutex_t *
start_watching(const char *dir_name, const char *file_name, int *ismodified)
{
	assert((strlen(file_name) + 1) < 69);
	Watch_Thread_Args *data = malloc(sizeof(Watch_Thread_Args));
	assert(data != NULL);

	int fd_in = inotify_init();
	if (fd_in < 0) {
		fprintf(ERR_LOG, "ERROR: inotify_init %s\n", strerror(errno));
		exit(1);
	}
	const int watch_mask = IN_CLOSE_WRITE;
    int fd_watch = inotify_add_watch(fd_in, dir_name, watch_mask);
	if (fd_watch < 0) {
		fprintf(ERR_LOG, "ERROR: inotify_add_watch %s\n", strerror(errno));
		exit(1);
	}

	memcpy(data->file_name, file_name, strlen(file_name) + 1);
	data->fd_in = fd_in;
	pthread_mutex_t *m = malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(m, NULL);
	data->m = m;
	data->ismodified = ismodified;
	pthread_t watch_thread;
	/*
	pthread_attr_t pta;
	pthread_attr_init(&pta);
	pthread_attr_setdetachstate(&pta, PTHREAD_CREATE_DETACHED);
	*/
	pthread_create(&watch_thread, NULL, watch_routine, data);
	return m;
}
