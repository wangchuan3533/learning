#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int main()
{
    struct sockaddr_in sin;
    struct hostent *h;
    int fd, random_fd;
    const char *hostname = "localhost";
    char buf[1024];
    int ret;

    random_fd = open("/dev/urandom", O_RDONLY);
    if (!random_fd) {
        fprintf(stderr, "could open /dev/urandom");
        return 1;
    }

    h = gethostbyname(hostname);
    if (!h) {
        fprintf(stderr, "could not lookup %s : %s", hostname, hstrerror(h_errno));
        return 1;
    }

    if (h->h_addrtype != AF_INET) {
        fprintf(stderr, "No ipv6 support, sorry.");
        return 1;
    }

    /* Allocate a new socket */
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket");
        return 1;
    }

    /* Connect to the remote host. */
    sin.sin_family = AF_INET;
    sin.sin_port = htons(40713);
    sin.sin_addr = *(struct in_addr*)h->h_addr;
    if (connect(fd, (struct sockaddr*)&sin, sizeof(sin))) {
        perror("connect");
        close(fd);
        return 1;
    }

    while (1) {
        ret = read(random_fd, buf, sizeof(buf));
        if (ret > 0) {
            write(fd, buf, ret);
        }
    }
}

