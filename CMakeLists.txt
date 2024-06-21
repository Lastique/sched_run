cmake_minimum_required(VERSION 3.13)

project(sched_run)

add_definitions(-D_REENTRANT=1 -D_GNU_SOURCE=1)
add_executable(sched_run sched_run.c)
