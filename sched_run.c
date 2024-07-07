/*
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt
 */
/*!
 * Sets process scheduling policy and/or niceness level and runs
 * the executable with any options specified in the command line.
 */

#include <unistd.h>
#include <sched.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <sys/time.h>
#include <sys/resource.h>

static void print_help()
{
    puts
    (
        "Usage: sched_run [sched_run options] -- <executable> [executable options]\n"
        "\n"
        "    sched_run options:\n"
        "\n"
        "      -h, --help             - Print this help message\n"
        "\n"
        "      -o, --other            - Set SCHED_OTHER scheduling policy\n"
        "      -f, --fifo             - Set SCHED_FIFO scheduling policy\n"
        "      -r, --rr               - Set SCHED_RR scheduling policy\n"
        "      -b, --batch            - Set SCHED_BATCH scheduling policy\n"
        "      -i, --idle             - Set SCHED_IDLE scheduling policy\n"
        "      -d, --deadline         - Set SCHED_DEADLINE scheduling policy\n"
        "      -R, --reset-on-fork    - Add SCHED_RESET_ON_FORK flag to the scheduling policy\n"
        "\n"
        "      -n <x>, --niceness <x> - Set niceness of the process to x, which must be an integer between -20 and 19\n"
    );
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        print_help();
        return 1;
    }

    int arg_index = 1;
    bool set_scheduler_policy = false, set_niceness = false;
    int scheduler_policy = 0, niceness = 0;
    for (; arg_index < argc; ++arg_index)
    {
        if (strcmp(argv[arg_index], "--") == 0)
        {
            ++arg_index;
            break;
        }
        else if (strcmp(argv[arg_index], "-h") == 0 || strcmp(argv[arg_index], "--help") == 0)
        {
            print_help();
            return 0;
        }
        else if (strcmp(argv[arg_index], "-o") == 0 || strcmp(argv[arg_index], "--other") == 0)
        {
            scheduler_policy = SCHED_OTHER | (scheduler_policy & SCHED_RESET_ON_FORK);
            set_scheduler_policy = true;
        }
        else if (strcmp(argv[arg_index], "-f") == 0 || strcmp(argv[arg_index], "--fifo") == 0)
        {
            scheduler_policy = SCHED_FIFO | (scheduler_policy & SCHED_RESET_ON_FORK);
            set_scheduler_policy = true;
        }
        else if (strcmp(argv[arg_index], "-r") == 0 || strcmp(argv[arg_index], "--rr") == 0)
        {
            scheduler_policy = SCHED_RR | (scheduler_policy & SCHED_RESET_ON_FORK);
            set_scheduler_policy = true;
        }
        else if (strcmp(argv[arg_index], "-b") == 0 || strcmp(argv[arg_index], "--batch") == 0)
        {
            scheduler_policy = SCHED_BATCH | (scheduler_policy & SCHED_RESET_ON_FORK);
            set_scheduler_policy = true;
        }
        else if (strcmp(argv[arg_index], "-i") == 0 || strcmp(argv[arg_index], "--idle") == 0)
        {
            scheduler_policy = SCHED_IDLE | (scheduler_policy & SCHED_RESET_ON_FORK);
            set_scheduler_policy = true;
        }
        else if (strcmp(argv[arg_index], "-d") == 0 || strcmp(argv[arg_index], "--deadline") == 0)
        {
            scheduler_policy = SCHED_DEADLINE | (scheduler_policy & SCHED_RESET_ON_FORK);
            set_scheduler_policy = true;
        }
        else if (strcmp(argv[arg_index], "-R") == 0 || strcmp(argv[arg_index], "--reset-on-fork") == 0)
        {
            scheduler_policy |= SCHED_RESET_ON_FORK;
        }
        else if (strcmp(argv[arg_index], "-n") == 0 || strcmp(argv[arg_index], "--niceness") == 0)
        {
            ++arg_index;
            if (arg_index >= argc)
            {
                puts("Niceness value not specified");
                return 1;
            }

            int res = sscanf(argv[arg_index], "%d", &niceness);
            if (res != 1)
            {
                printf("Failed to parse niceness value: %s\n", argv[arg_index]);
                return 1;
            }

            set_niceness = true;
        }
        else
        {
            printf("Unrecognized option: %s\n", argv[arg_index]);
            return 1;
        }
    }

    if (arg_index >= argc)
    {
        puts("No executable specified");
        return 1;
    }

    if (set_scheduler_policy)
    {
        struct sched_param sp;
        sp.sched_priority = sched_get_priority_min(scheduler_policy & ~(SCHED_RESET_ON_FORK));
        if (sched_setscheduler(0, scheduler_policy, &sp) < 0)
        {
            perror("Failed to set scheduling policy");
            return 1;
        }
    }

    if (set_niceness)
    {
        if (setpriority(PRIO_PROCESS, 0, niceness) < 0)
        {
            perror("Failed to set process niceness");
            return 1;
        }
    }

    if (execve(argv[arg_index], argv + arg_index, environ) < 0)
    {
        perror("Failed to run executable");
        return 1;
    }

    return 0;
}
