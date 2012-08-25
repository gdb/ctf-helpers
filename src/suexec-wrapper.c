/* Build via gcc -o suexec-wrapper suexec-wrapper.c */
#include <errno.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>

#define PATH "/usr/lib/apache2/suexec-real"

void setlimit(int resource, rlim_t value)
{
	struct rlimit rl;
	rl.rlim_cur = value;
	rl.rlim_max = value;
	if (setrlimit(resource, &rl) < 0) {
		perror("Error setting rlimit");
		/* TODO: exit elsewhere */
		_exit(1);
	}
}

void incnice(int inc)
{
	if (nice(inc) < 0) {
		perror("Error nicing process");
		/* TODO: exit elsewhere */
		_exit(1);
	}
}

int main(int argc, char *argv[])
{
	/* 4G - 1 bytes */
	setlimit(RLIMIT_AS, 4294967295);
	setlimit(RLIMIT_CPU, 60 * 60);
	setlimit(RLIMIT_MEMLOCK, 64 * 1024);
	setlimit(RLIMIT_NOFILE, 1024);
	setlimit(RLIMIT_NPROC, 60);

	incnice(2);

	execvp(PATH, argv);

	perror("Error execing suexec");
	return 1;
}
