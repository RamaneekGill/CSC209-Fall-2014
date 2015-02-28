//include goes here with fxn prototypes & port number
static int listenfd;
struct client {
    int fd; struct in_addr ipaddr; struct client *next;
} *top = NULL;
int howmany = 1; 
static char greeting[] = "Do you know the muffin man,\r\n"
static char happyreply[] = "%d of us know the muffin man,\r\n"

int main(int argc, char **argv) {
    int c;
    struct client *p;
    bindandlisten();  /* aborts on error */
    while (1) { //server only exits when its killed
        fd_set fdlist;
        int maxfd = listenfd;
        FD_ZERO(&fdlist);
        FD_SET(listenfd, &fdlist);
        for (p = top; p; p = p->next) {
            FD_SET(p->fd, &fdlist);
            if (p->fd > maxfd)
                maxfd = p->fd;
        }
        if (select(maxfd + 1, &fdlist, NULL, NULL, NULL) < 0) {
            perror("select");
        } else {
            for (p = top; p; p = p->next)
                if (FD_ISSET(p->fd, &fdlist))
                    //this client is ready to be read
                    break;
            if (p)
                whatsup(p);  /* might remove p from list, so can't be in the loop */
            if (FD_ISSET(listenfd, &fdlist))
                newconnection();
        }
    } return(0);
}

void bindandlisten(void) {
    struct sockaddr_in r;
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }
    memset(&r, '\0', sizeof r);
    r.sin_family = AF_INET;
    r.sin_addr.s_addr = INADDR_ANY;
    r.sin_port = htons(port);
    if (bind(listenfd, (struct sockaddr *)&r, sizeof r)) {
        perror("bind"); exit(1);
    }
    if (listen(listenfd, 5)) {
        perror("listen"); exit(1);
    }
}

void newconnection(void) {
    int fd;
    struct sockaddr_in r;
    socklen_t socklen = sizeof r;
    int len, i, c;
    char buf[sizeof happyreply + 40];
    if ((fd = accept(listenfd, (struct sockaddr *)&r, &socklen)) < 0) {
        perror("accept");
    } else {
        printf("connection from %s\n", inet_ntoa(r.sin_addr));
        addclient(fd, r.sin_addr);
        sprintf(buf, "Dear %s,\r\n", inet_ntoa(r.sin_addr));
        broadcast(buf, strlen(buf));
        broadcast(greeting, sizeof greeting - 1);
    }
}

void whatsup(struct client *p) {
    char garbage[80];
    int len = read(p->fd, garbage, sizeof garbage);
    if (len > 0) {
        
    } else if (len == 0) {
        char buf[80];
        printf("Disconnect from %s\n", inet_ntoa(p->ipaddr));
        removeclient(p->fd);
    } else {
        perror("read");
    }
}

static void addclient(int fd, struct in_addr addr) {
    struct client *p = malloc(sizeof(struct client));
    if (!p) {
        fprintf(stderr, "out of memory!\n");  /* highly unlikely to happen */
        exit(1);
    }
    printf("Adding client %s\n", inet_ntoa(addr));
    p->fd = fd;
    p->ipaddr = addr;
    p->next = top;
    top = p;
    howmany++;
}

static void removeclient(int fd) {
    struct client **p;
    for (p = &top; *p && (*p)->fd != fd; p = &(*p)->next)
        ;
    if (*p) {
        struct client *t = (*p)->next;
        printf("Removing client %s\n", inet_ntoa((*p)->ipaddr));
        free(*p);
        *p = t;
        howmany--;
    } else {
        fprintf(stderr, "Trying to remove fd %d, but I don't know about it\n", fd);
    }
}

static void broadcast(char *s, int size) {
    struct client *p;
    for (p = top; p; p = p->next)
        write(p->fd, s, size); //should check w perror
}
