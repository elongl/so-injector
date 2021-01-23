#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/user.h>

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
}

void save_state()
{
    if (ptrace(PTRACE_GETREGS, injected_proc, NULL, &state))
        ERR_EXIT("PTRACE_GETREGS");
}

void *get_rw_mem()
{
    int ret;
    char *line = NULL;
    char addr_str[16] = {};
    char maps_path[255];
    FILE *maps;
    size_t len = 0;
    ssize_t read;

    sprintf(maps_path, "/proc/%u/maps", injected_proc);
    if (!(maps = fopen(maps_path, "r")))
        ERR_EXIT("fopen");

    while ((ret = getline(&line, &len, maps)) != -1)
    {
        if (strstr(line, "rw-p"))
        {
            puts(line);
            ret = strstr(line, "-") - line;
            strncpy(addr_str, line, ret);
            break;
        }
    }
    fclose(maps);
    free(line);
    return (void*)strtol(addr_str, NULL, 16);
}
void inject_so_path()
{
    void* rw_mem = get_rw_mem();
    printf("Injecting .so path to %p\n", rw_mem);
}

int main()
{
    injected_proc = 1;
    inject_so_path();
}
