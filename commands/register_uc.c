#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <err.h>

#define SCHED_UNACLOUD 7

int main(int argc, char *argsv[]) {
    struct sched_param sp;
    if (argc != 1)
        err(EXIT_FAILURE, _("must pass process pid as parameter."));

    int pid = argv[1];
    if (sched_setscheduler(pid, SCHED_UNACLOUD, &sp) == -1)
        err(EXIT_FAILURE, _("failed to set pid %d's policy"), pid);
}