#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <event2/event.h>

void cb_func(evutil_socket_t fd, short what, void *arg)
{
    const char *data = arg;
    printf("Got an event on socket %d:%s%s%s%s [%s]\n",
           (int) fd,
           (what&EV_TIMEOUT) ? " timeout" : "",
           (what&EV_READ) ? " read" : "",
           (what&EV_WRITE) ? " write" : "",
           (what&EV_SIGNAL) ? " signal" : "",
           data);

}

void main_loop(evutil_socket_t fd1, evutil_socket_t fd2)
{
    struct event *ev1, *ev2;
    struct timeval five_seconds = {5,0};
    struct event_base *base = event_base_new();
    /* The caller has already set up fd1, fd2 somehow, and m
     * ake them
     * nonblocking. */
    ev1 = event_new(base, fd1, EV_TIMEOUT|EV_READ|EV_PERSIST
                    , cb_func,
                    (char*)"Reading event");
   // ev2 = event_new(base, fd2, EV_WRITE|EV_PERSIST, cb_func,
    //                (char*)"Writing event");
    ev2 = event_new(base, fd2, EV_WRITE, cb_func,
                    (char*)"Writing event");
    event_add(ev1, &five_seconds);
    event_add(ev2, NULL);
    event_base_dispatch(base);

}

int main(int argc,char *argv[])
{
    pid_t fd[2];
    pipe(fd);
    main_loop(fd[0],fd[1]);
    close(fd[0]);
    close(fd[1]);
    return 0;
}
