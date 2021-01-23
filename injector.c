#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <stdarg.h>
#include <dlfcn.h>
#include <stdint.h>

#define ERR_EXIT(errmsg)    \
    {                       \
        perror(errmsg);     \
        exit(EXIT_FAILURE); \
    }

pid_t injected_proc;
struct user_regs_struct state;

void usage() {}

void attach_injected_proc()
{
    if (ptrace(PTRACE_ATTACH, injected_proc))
        ERR_EXIT("PTRACE_ATTACH");
    waitpid(injected_proc, NULL, 0);
}

void save_state()
{
    if (ptrace(PTRACE_GETREGS, injected_proc, NULL, &state))
        ERR_EXIT("PTRACE_GETREGS");
}

void *search_maps(int count, ...)
{
    int ret, idx;
    char *substr, *line = NULL;
    char addr_str[16] = {};
    char maps_path[255];
    FILE *maps;
    size_t len = 0;
    va_list substrs;

    sprintf(maps_path, "/proc/%u/maps", injected_proc);
    if (!(maps = fopen(maps_path, "r")))
        ERR_EXIT("fopen");

    while (getline(&line, &len, maps) != -1)
    {
        va_start(substrs, count);
        idx = 0;

        while (idx != count)
        {
            substr = va_arg(substrs, char *);
            if (!strstr(line, substr))
                break;
            idx++;
        }
        if (idx == count)
        {
            ret = strstr(line, "-") - line;
            strncpy(addr_str, line, ret);
            break;
        }
    }

    fclose(maps);
    free(line);
    return (void *)strtol(addr_str, NULL, 16);
}

void *get_rw_mem()
{
    return search_maps(1, "rw-p");
}

void *get_dlopen()
{
    Dl_info ldso;
    void *libdl;
    unsigned int dlopen_offset;

    libdl = search_maps(2, "r-xp", "libdl");
    dladdr(dlopen, &ldso);
    dlopen_offset = (uintptr_t)dlopen - (uintptr_t)ldso.dli_fbase;
    return libdl + dlopen_offset;
}
void inject_so_path()
{
    printf("Injecting .so path to %p\n", get_rw_mem());
    printf("dlopen() @ %p\n", get_dlopen());
}

int main()
{
    injected_proc = 1;
    // attach_injected_proc();
    // save_state();
    // inject_so_path();
    // get_dlopen();
}
