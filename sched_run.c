/*
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt
 */
/*!
 * Sets process scheduling policy to SCHED_RR with the lowest priority
 * and runs the executable with any options specified in the command line.
 */

#include <unistd.h>
#include <sched.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        puts("Usage: sched_run <executable> [options]");
        return 1;
    }

    struct sched_param sp;
    sp.sched_priority = sched_get_priority_min(SCHED_RR);
    if (sched_setscheduler(0, SCHED_RR | SCHED_RESET_ON_FORK, &sp) < 0)
    {
        perror("Failed to set scheduling policy");
        return 1;
    }

    if (execve(argv[1], argv + 1, environ) < 0)
    {
        perror("Failed to run executable");
        return 1;
    }

    return 0;
}
