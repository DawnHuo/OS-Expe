#include <stdio.h>
#include <unistd.h>
#include <memory.h>
#include <sys/wait.h>
#include <stdlib.h>

void execute(char *cmd)
{
    int pid;
    pid = fork();
    int error;
    if(pid == 0)
    {
        error = execl("/bin/sh", "sh", "-c", cmd, NULL);
        if(error == -1)
        {
            printf("'%s' command not fount.\n", cmd);
            exit(0);
        }
    }
    waitpid(pid, NULL, 0);
}

int buildin_command(char **argv)
{
    if(strcmp(argv[0], "exit") == 0)
        exit(0);
    if(strcmp(argv[0], "cd") == 0)
    {
        if(chdir(argv[1] < 0))
        {
            perror("chdir failed.\n");
            return 0;
        }
        return 1;
    }
    if(strcmp(argv[0], "pwd") == 0)
    {
        char buff[512];
        printf("%s\n", getcwd(buff, sizeof(buff)));
        return 1;
    }
    return 0;
}

int parseline(char *cmd, char **argv)
{
    char *pch;
    pch = strtok(cmd, " ");
    int i = 0;
    while(pch != NULL)
    {
        argv[i++] = pch;
        pch = strtok(NULL, " ");
    }
    argvp[i] = NULL;
    return 0;
}

void eval(cahr *cmd)
{
    char *argv[512];
    char buf[512];

    strcpy(buf, cmd);
    paraseline(buf, argv);
    if(argv[0] == NULL) return;
    if(buildin_command(argv))   return;
    execute(cmd);
}

int main()
{
    char buf[512];
    char p[512];
    char c;
    int n;

    while(1)
    {
        printf("**");
        printf("mysystem:%s > ", getcwd(buf, sizeof(buf)));
        n = 0;
        while(1)
        {
            scanf("%c", &c);
            if(c == '\n')   break;
            p[n] = c;
            n++;
        }

        memset(buf, 0, strlen(buf));
        eval(p);
        memset(p, '\0', strlen(p));
    }
    return 0;
}