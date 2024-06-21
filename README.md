# sched_run

A simple wrapper to run processes with real-time scheduling policy.

Usage:

```
sched_run <executable> [options]
```

Sets process scheduling policy to `SCHED_RR` with the lowest priority and runs the `executable` with any `options` specified in the command line.
Useful for running timing-critical processes such as a window manager or an audio server.

In order for the wrapper to not require `sudo` for setting the scheduler policy, the executable can be granted `CAP_SYS_NICE` capability:

```
sudo setcap 'cap_sys_nice=ep' <path-to-sched_run>
```
