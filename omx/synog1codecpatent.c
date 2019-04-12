#ifndef MY_ABC_HERE
#define MY_ABC_HERE
#endif
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include "synog1codecpatent.h"

#if defined(MY_ABC_HERE)
#define SZ_BIN_SYNOCODECPATENT "/var/packages/VideoStation/target/bin/synocodectool"
#elif defined(SYNO_AUDIOSTATION)
#define SZ_BIN_SYNOCODECPATENT "/var/packages/AudioStation/target/bin/synocodectool"
#elif defined(SYNO_MEDIASERVER)
#define SZ_BIN_SYNOCODECPATENT "/var/packages/MediaServer/target/bin/synocodectool"
#else
#define SZ_BIN_SYNOCODECPATENT "/usr/syno/bin/synocodectool"
#endif

#ifdef MY_ABC_HERE

static pid_t SYNOProcForkEx(void)
{
	int pid = -1;
	int fd = -1;
	int	status = 0;
	int waitRet = 0;
	char *pEnvPath = NULL;

	fflush(NULL);

	pid = fork();
	if (0 > pid) {
		return -1;
	} else if (pid > 0) {
		while (-1 == (waitRet = waitpid(pid, &status, 0)) && EINTR == errno) {
			//do nothing
		}
		if (-1 != waitRet && 0 == WEXITSTATUS(status)) {
			return 1;
		} else if (-1 == waitRet && errno == ECHILD) {
			return 1;
		}
		return -1;
	}

	pEnvPath = getenv("PATH");
	if (pEnvPath) {
		setenv("PATH", pEnvPath, 1);
	}

	if (-1 != (fd = open("/dev/null", O_RDWR, 0))) {
		(void) dup2(fd, STDIN_FILENO);
		(void) dup2(fd, STDOUT_FILENO);
		(void) dup2(fd, STDERR_FILENO);
		if (2 < fd)
			(void) close(fd);
	}

	setsid();
	chdir("/");

	return 0;
}

static int SYNOExecl(const char *szPath, const char* szArg, ...)
{
    int ret=-1, pid, nowait, status=0;
	int	fChildAlive = 0;
	struct sigaction	saOrg;
	size_t i = 0;
	size_t argc = 0;
	const char *arg = NULL;
	const char **argv = NULL;
	va_list ap;

	// remove SA_NOCLDWAIT flags if set
	sigaction(SIGCHLD, NULL, &saOrg);
	nowait = (saOrg.sa_flags & SA_NOCLDWAIT);
	saOrg.sa_flags &= ~SA_NOCLDWAIT;
	sigaction(SIGCHLD, &saOrg, NULL);

	if ((pid = SYNOProcForkEx()) == 0) {
		arg = szArg;
		va_start(ap, szArg);
		for(argc = 2; NULL != arg; ++argc) {
			arg = va_arg(ap, const char*);
		}
		va_end(ap);

		argv = (const char**) calloc (argc, sizeof(const char*));

		arg = szArg;
		argv[0] = szPath;
		argv[1] = szArg;
		va_start(ap, szArg);
		for (i = 2; NULL != arg; ++i) {
			arg = va_arg(ap, const char*);
			argv[i] = arg;
		}
		va_end(ap);

		execv(szPath, (char *const *) argv);

		free(argv);
		// If any of the exec functions returns, an error will have occurred.  The return value is 255
		_exit(-1);
	} else if (pid != -1) {
		while (-1 == (fChildAlive = waitpid(pid, &status, 0)) && EINTR == errno);
		if (-1 != fChildAlive && WIFEXITED(status)) {
			//child alive --> wait until child done and check its exit code.
			ret = WEXITSTATUS(status);
		} else if ( fChildAlive == -1 && errno == ECHILD ) {
			//child doesn't alive --> see as success
			ret = 0;
		}
	}
	// restore SA_NOCLDWAIT flag if orginal set
	if (nowait) {
		sigaction(SIGCHLD, NULL, &saOrg);
		saOrg.sa_flags |= SA_NOCLDWAIT;
		sigaction(SIGCHLD, &saOrg, NULL);
	}
	return ret;
}

static int FileExit(const char *szPath)
{
	int iRet = 0;
	struct stat statBuf;

	if (!szPath) {
		goto Exit;
	}
	if (0 == stat(szPath, &statBuf) && S_ISREG(statBuf.st_mode)) {
		iRet = 1;
	}
Exit:
	return iRet;

}

/**
 * check the codec can use,
 * active, 0
 * not active or error, -1
 */
int ActivateCodec(const char *szCodecName)
{
	int iRet = -1;
	char *szEnvSkipActivate = NULL;

	//for some case like avformat_find_stream_info, don't need avtivate mechanism.
	szEnvSkipActivate = getenv(SZ_SYNO_CODEC_SKIP_ACTIVATION);
	if (szEnvSkipActivate && 0 == strcmp(szEnvSkipActivate, "yes")) {
		return 0;
	}

	if (!szCodecName) {
		goto Exit;
	}

	if (!FileExit(SZ_BIN_SYNOCODECPATENT)) {
		goto Exit;
	}

	if (0 != SYNOExecl(SZ_BIN_SYNOCODECPATENT, "--check_codec_activated", szCodecName, "--pkgname", SZ_PKG_NAME, NULL)) {
		if (0 != SYNOExecl(SZ_BIN_SYNOCODECPATENT, "--activate_codec", szCodecName, "--pkgname", SZ_PKG_NAME, NULL)) {
			goto Exit;
		}
	}

	iRet = 0;
Exit:
	return iRet;
}
#endif //MY_ABC_HERE
