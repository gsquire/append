#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define BUF 4096
#define PATH 255
#define MAX_FDS 1024

// the cwd, global so we can free it in error situation
char *cwd; 

void error_and_exit()
{
    fprintf(stderr, "Usage: app [SRC] [DST]\n");
    fprintf(stderr, "Append SRC to DST\n");
    exit(EXIT_FAILURE);
}

void free_resources(int fds[], int num_fds)
{
    // for the global cwd buffer
    free(cwd);
    int i;
    for (i = 0; i < num_fds; i++)
        close(fds[i]);
}

void open_error_exit()
{
    perror("Opening file");
    free(cwd);
    exit(EXIT_FAILURE);
}

void append_file(char *src, char *dst)
{
    int fds[MAX_FDS];
    int ind = 0;

    int rfd = open(src, O_RDONLY);
    if (rfd < 0)
        open_error_exit();

    int wfd = open(dst, O_WRONLY|O_APPEND);
    if (wfd < 0)
    {
        close(rfd);
        open_error_exit();
    }

    fds[ind++] = rfd;
    fds[ind++] = wfd;

    char buffer[BUF];
    ssize_t bytes;

    while((bytes = read(rfd, &buffer, BUF)) > 0)
    {
        ssize_t written = write(wfd, &buffer, bytes);
        if (bytes != written)
        {
            free_resources(fds, ind-1);
            perror("Writing to file");
            exit(EXIT_FAILURE);
        }
    }

    free_resources(fds, ind-1);
}

int main(int argc, char *argv[])
{
    if (argc < 3)
        error_and_exit();

    cwd = malloc(PATH);
    if (!getcwd(cwd, PATH))
    {
        free(cwd);
        perror("Getting cwd");
        exit(EXIT_FAILURE);
    }

    // cwd is freed regardless of success or failure in append_file
    append_file(argv[1], argv[2]);

    return 0;
}
