#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>

#define FALSE 0
#define TRUE 1

#define EOL	0
#define AMPERSAND 1
#define PIPE 2
#define ARG	10
#define IREDIRECT 11
#define OREDIRECT 12

#define STDIO 0
#define FILEIO 1
#define PIPEIO 2

union IoMethodInner {
    char *filename;
    int pipefd;
};

struct IoMethod {
    char tag;
    union IoMethodInner inner;
};

int get_token(char **bufptr, char **argptr, char **ifile, char **ofile) {
    if (**bufptr == ' ' || **bufptr == '\t') {
        **bufptr = '\0';
        do {
            *bufptr += 1;
        } while (**bufptr == ' ' || **bufptr == '\t');
    }

    switch (**bufptr) {
        case '\0':
        case '\n':
            **bufptr = '\0';
            return EOL;

        case '&':
            **bufptr = '\0';
            *bufptr += 1;
            return AMPERSAND;
        
        case '<':
            **bufptr = '\0';
            *bufptr += 1;
            while (**bufptr == ' ' || **bufptr == '\t') {
                *bufptr += 1;
            }
            *ifile = *bufptr;
            while ((**bufptr != ' ')
                && (**bufptr != '&')
                && (**bufptr != '\t')
                && (**bufptr != '\n')
                && (**bufptr != '\0')
                && (**bufptr != '<')
                && (**bufptr != '>')
                && (**bufptr != '|')) {
                *bufptr += 1;
            }
            return IREDIRECT;
        
        case '>':
            **bufptr = '\0';
            *bufptr += 1;
            while (**bufptr == ' ' || **bufptr == '\t') {
                *bufptr += 1;
            }
            *ofile = *bufptr;
            while ((**bufptr != ' ')
                && (**bufptr != '&')
                && (**bufptr != '\t')
                && (**bufptr != '\n')
                && (**bufptr != '\0')
                && (**bufptr != '<')
                && (**bufptr != '>')
                && (**bufptr != '|')) {
                *bufptr += 1;
            }
            return OREDIRECT;
        
        case '|':
            **bufptr = '\0';
            *bufptr += 1;
            return PIPE;

        default:
            *argptr = *bufptr;
            while ((**bufptr != ' ')
                && (**bufptr != '&')
                && (**bufptr != '\t')
                && (**bufptr != '\n')
                && (**bufptr != '\0')
                && (**bufptr != '<')
                && (**bufptr != '>')
                && (**bufptr != '|')) {
                *bufptr += 1;
            }
            return ARG;
    }
}

int execute(char **argvec, int type, const struct IoMethod *in, const struct IoMethod *out) {
    int	pid = fork();

    if (pid < 0) {
        fprintf(stderr, "minish : fork error\n");
        return -1;
    } else if (pid == 0) {
        // Child process
        if (in->tag != STDIO) {
            int fd;
            if (in->tag == FILEIO) {
                fd = open(in->inner.filename, O_RDONLY);
                if (fd == -1) {
                    fprintf(stderr, "minish : Could not open input file\n");
                    exit(127);
                }
            } else {
                fd = in->inner.pipefd;
            }
            dup2(fd, STDIN_FILENO);
            close(fd);
        }

        if (out->tag != STDIO) {
            int fd;
            if (out->tag == FILEIO) {
                fd = open(out->inner.filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd == -1) {
                    fprintf(stderr, "minish : Could not open output file\n");
                    exit(127);
                }
            } else {
                fd = out->inner.pipefd;
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }
        execvp(argvec[0], argvec);
        fprintf(stderr, "minish : command not found\n");
        exit(127);
    }

    // Parent process
    if (in->tag == PIPEIO) {
        close(in->inner.pipefd);
    }
    if (out->tag == PIPEIO) {
        close(out->inner.pipefd);
    }
    if (type == AMPERSAND) {   // Print child pid and return immediately
        printf("[%d]\n", pid);
    } else if (type == EOL) {  // Wait for child process to finish
        while (waitpid(pid, NULL, 0) < 0) {
            if (errno != EINTR) {
                return -1;
            }
        }
    }
    // On type == PIPE, we don't print child pid and we don't wait for the
    // child process.
    return 0;
}

int parse_and_execute(char *input) {
    char *argvec[2048];
    int	type;
    int how;

    int imethod = STDIO;
    int omethod = STDIO;
    char *ifile;
    char *ofile;
    int pipefd[2];

    int	quit = FALSE;
    int	argcount = 0;
    int	finished = FALSE;
    
    while (!finished) {
        type = get_token(&input, &argvec[argcount], &ifile, &ofile);
        switch (type) {
        case ARG:
            argcount += 1;
            break;
        case IREDIRECT:
            if (imethod = PIPEIO) {
                close(pipefd[0]);
            }
            imethod = FILEIO;
            break;
        case OREDIRECT:
            omethod = FILEIO;
            break;
        case EOL:
        case AMPERSAND:
        case PIPE:
            if (argcount != 0) {
                if (strcmp(argvec[0], "exit") == 0) {
                    quit = TRUE;
                } else if (strcmp(argvec[0], "cd") == 0 && argcount > 1) {
                    if (chdir(argvec[1]) == -1) {
                        printf("cd: %s: ", argvec[1]);
                        switch (errno) {
                        case EACCES:
                            puts("Access denied");
                            break;
                        case EFAULT:
                            puts("Invalid address space");
                            break;
                        case EIO:
                            puts("IO Error");
                            break;
                        case ELOOP:
                            puts("Too many symbolic links");
                            break;
                        case ENAMETOOLONG:
                            puts("Name too long");
                            break;
                        case ENOENT:
                            puts("Directory  does not exist");
                            break;
                        case ENOMEM:
                            puts("Out of kernel memory");
                            break;
                        case ENOTDIR:
                            puts("Not a directory");
                            break;
                        default:
                            // unreachable?
                            break;
                        }
                    }
                } else {
                    struct IoMethod in;
                    struct IoMethod out;
                    if (imethod == PIPEIO) {
                        in.tag = PIPEIO;
                        in.inner.pipefd = pipefd[0];
                    } else if (imethod == FILEIO) {
                        in.tag = FILEIO;
                        in.inner.filename = ifile;
                    } else {
                        in.tag = STDIO;
                    }
                    imethod = STDIO;

                    if (omethod == FILEIO) {
                        out.tag = FILEIO;
                        out.inner.filename = ofile;
                    } else if (type == PIPE) {
                        out.tag = PIPEIO;
                        pipe(pipefd);
                        out.inner.pipefd = pipefd[1];
                        imethod = PIPEIO;
                    } else {
                        out.tag = STDIO;
                    }
                    omethod = STDIO;

                    if (argcount != 0) {
                        argvec[argcount] = NULL;
                        execute(argvec, type, &in, &out);
                    }
                }
                argcount = 0;
            }
            if (type == EOL) {
                finished = TRUE;
            }
            break; 
        }
    }
    return quit;
}

int main() {
    char buf[4096];
    int	quit = FALSE;

    while (quit != TRUE) {
        printf("msh # ");
        if (fgets(buf, 4096, stdin) == NULL) {
            break;
        }
        // Here I assume 4096 bytes are enough for a single line.
        // That means, when given a line longer than that, this program will do
        // something bad like halting and catching fire, or continuing silently
        // even though something is wrong and doing weird thing.
        // if one were to handle that case properly, I think I can do this:
        // if (strlen(buf) == 4095) {
        //     /* Do something */
        //     continue;
        // }
        // But then we iterate through the input twice, and I don't like that.
        // So I'm not doing that.
        quit = parse_and_execute(buf);
    }
    return 0;
}