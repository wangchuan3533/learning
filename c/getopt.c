#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
int main(int argc, char *argv[])
{
    int ret, opt, daemon_mode = 0;

    while ((opt = getopt(argc, argv, "c:r:l:d")) != -1) {
        switch (opt) {
        case 'c':
            printf("client port is %s\n", optarg);
            break;
        case 'r':
            printf("rpc port is %s\n", optarg);
            break;
        case 'l':
            printf("log file 0s %s\n", optarg);
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
    if (daemon_mode) {
        daemon(0, 0);
    }
    while (1) {
        sleep(1);
    }
    exit(0);
}

