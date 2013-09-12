#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

/* globals */
int client_port = 0;
int rpc_port = 0;
const char *log_file = NULL;
const char *pid_file = NULL;
int daemon_mode = 0;

int main(int argc, char *argv[])
{
    int i, ret, opt;
    const char *opts_short = "c:r:l:p:d";
    struct option opts_long[] = {
        {"client-port", 1, NULL, 'c'},
        {"rpc-port", 1, NULL, 'r'},
        {"log-file", 1, NULL, 'l'},
        {"pid-file", 1, NULL, 'p'},
        {"daemon-mode", 0, NULL, 'd'},
        {0, 0, 0, 0}};

    while ((opt = getopt_long(argc, argv, opts_short, opts_long, NULL)) != -1) {
        switch (opt) {
        case 'c':
            client_port = atoi(optarg);
            break;
        case 'r':
            rpc_port = atoi(optarg);
            printf("rpc port is %s\n", optarg);
            break;
        case 'l':
            log_file = optarg;
            printf("log file is %s\n", optarg);
            break;
        case 'p':
            pid_file = optarg;
            printf("pid file is %s\n", optarg);
            break;
        case 'd':
            printf("daemon enabled\n");
            daemon_mode = 1;
            break;
        default:
            printf("unkown option\n");
            break;
        }
    }

    for (i = 0; i < argc; i++) {
        printf("arg[%d] = 0x%lx\n", i, (unsigned long)argv[i]);
    }
    printf("log_file=0x%lx\n", (unsigned long)log_file);
    printf("pid_file=0x%lx\n", (unsigned long)pid_file);
    /*
    printf("client_port is %d\n", client_port);
    printf("rpc_port is %d\n", rpc_port);
    printf("log_file is %s\n", log_file);
    printf("pid_file is %s\n", pid_file);
    printf("daemon_mode is %d\n", daemon_mode);
    */


    while (optind < argc) {
        printf("%s\n", argv[optind++]);
    }

    if (daemon_mode) {
        daemon(0, 0);
    }

    while (1) {
        sleep(1);
    }
    exit(0);
}

