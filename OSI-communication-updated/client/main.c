/*
Client program:
- Connects to server at localhost:10000.
- Listens for one monitor (Python) connection on port 10001.
- Receives commands from monitor (text lines, e.g. "SEND:Hello").
- When a SEND command is received:
    - Encapsulate layers (simulate) and after every 2 layers, send progress JSON to monitor.
    - Send full JSON packet to server, wait for reply.
    - On receiving reply, simulate decapsulation and after every 2 layers, send progress JSON to monitor.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <errno.h>

#define SERVER_HOST "127.0.0.1"
#define SERVER_PORT 10000
#define MONITOR_PORT 10001
#define BUFSIZE 8192

/* Layer list */
static const char *LAYERS[] = {
    "Application","Presentation","Session","Transport","Network","DataLink","Physical"
};
static const int LAYERS_COUNT = 7;

/* monitor socket (single connection) */
static int monitor_fd = -1;
static int server_fd = -1;

/* send newline-terminated JSON to monitor */
static void monitor_send_json(const char *json) {
    if (monitor_fd < 0) return;
    size_t len = strlen(json);
    char *line = malloc(len + 2);
    if (!line) return;
    memcpy(line, json, len);
    line[len] = '\n';
    line[len+1] = '\0';
    ssize_t w = write(monitor_fd, line, len+1);
    (void)w;
    free(line);
}

/* helper: send progress to monitor in JSON */
static void progress_report(const char *direction, const char *layer1, const char *layer2) {
    char buf[512];
    if (layer2) {
        snprintf(buf, sizeof(buf),
                 "{\"type\":\"progress\",\"direction\":\"%s\",\"completed\":[\"%s\",\"%s\"]}",
                 direction, layer1, layer2);
    } else {
        snprintf(buf, sizeof(buf),
                 "{\"type\":\"progress\",\"direction\":\"%s\",\"completed\":[\"%s\"]}",
                 direction, layer1);
    }
    monitor_send_json(buf);
}

/* send length-prefixed JSON to server */
static int send_lp(int s, const char *data, size_t len) {
    uint32_t L = htonl((uint32_t)len);
    if (write(s, &L, 4) != 4) return -1;
    ssize_t w = write(s, data, len);
    return w == (ssize_t)len ? 0 : -1;
}

/* recv lp */
static int recv_lp(int s, char *buf, size_t bufsz) {
    uint32_t L;
    ssize_t r = read(s, &L, 4);
    if (r == 0) return 0;
    if (r != 4) return -1;
    uint32_t len = ntohl(L);
    if (len >= bufsz) return -1;
    size_t got = 0;
    while (got < len) {
        ssize_t rr = read(s, buf + got, len - got);
        if (rr <= 0) return -1;
        got += rr;
    }
    buf[got] = '\0';
    return (int)got;
}

/* build JSON packet with layers array and payload */
static void build_packet_json(const char *payload, char *out, size_t outsz) {
    /* naive building */
    char tmp[4096];
    int pos = 0;
    pos += snprintf(tmp + pos, sizeof(tmp) - pos, "{\"type\":\"packet\",\"layers\":[");
    for (int i = 0; i < LAYERS_COUNT; ++i) {
        pos += snprintf(tmp + pos, sizeof(tmp) - pos, "\"%s\"%s", LAYERS[i], (i == LAYERS_COUNT-1) ? "" : ",");
    }
    pos += snprintf(tmp + pos, sizeof(tmp) - pos, "],\"payload\":\"%s\"}", payload);
    strncpy(out, tmp, outsz-1);
    out[outsz-1] = '\0';
}

/* simulate encapsulation: after every two layers call progress_report */
static void simulate_encapsulation_and_send(const char *payload) {
    /* perform encapsulation top-down: Application -> ... -> Physical */
    for (int i = 0; i < LAYERS_COUNT; i += 2) {
        const char *l1 = LAYERS[i];
        const char *l2 = (i+1 < LAYERS_COUNT) ? LAYERS[i+1] : NULL;
        /* simulate small delay */
        usleep(80000); /* 80ms */
        progress_report("TX", l1, l2);
    }
    /* build full packet JSON and send to server */
    char pkt[4096];
    build_packet_json(payload, pkt, sizeof(pkt));
    if (send_lp(server_fd, pkt, strlen(pkt)) != 0) {
        fprintf(stderr, "Failed to send to server\n");
        return;
    }
    /* wait for reply */
    char buf[BUFSIZE];
    int n = recv_lp(server_fd, buf, sizeof(buf));
    if (n <= 0) {
        fprintf(stderr, "Failed to receive from server\n");
        return;
    }
    /* notify monitor about full packet received (optional) */
    // monitor_send_json(buf); // could be sent, but we'll report decapsulation instead

    /* simulate decapsulation bottom-up: Physical -> ... -> Application */
    for (int i = LAYERS_COUNT-1; i >= 0; i -= 2) {
        /* pair current and previous for reporting as they get removed in pairs bottom-up */
        const char *l1 = (i-1 >= 0) ? LAYERS[i-1] : NULL;
        const char *l2 = LAYERS[i];
        /* In bottom-up order, we want to report physical+datalink, then network+transport, etc.
           To maintain the "two layers" grouping consistent with the user's requirement, we will
           group l2 (current) with l1 (previous) where present. */
        if (l1) {
            progress_report("RX", l1, l2);
        } else {
            progress_report("RX", l2, NULL);
        }
        usleep(80000);
    }
}

/* handle monitor connection (single threaded) */
static void *monitor_thread_fn(void *arg) {
    int mfd = *(int*)arg;
    char line[2048];
    FILE *mf = fdopen(mfd, "r");
    if (!mf) {
        close(mfd);
        return NULL;
    }
    monitor_fd = mfd;
    /* send initial info */
    monitor_send_json("{\"type\":\"info\",\"text\":\"Client monitor connected\"}");
    while (fgets(line, sizeof(line), mf)) {
        /* strip newline */
        size_t L = strlen(line);
        while (L > 0 && (line[L-1]=='\n' || line[L-1]=='\r')) { line[--L] = '\0'; }
        if (L == 0) continue;
        /* expect SEND:<payload> */
        if (strncmp(line, "SEND:", 5) == 0) {
            const char *payload = line + 5;
            simulate_encapsulation_and_send(payload);
        } else {
            /* unknown command */
            char buf[256];
            snprintf(buf, sizeof(buf), "{\"type\":\"info\",\"text\":\"Unknown command: %s\"}", line);
            monitor_send_json(buf);
        }
    }
    fclose(mf);
    monitor_fd = -1;
    return NULL;
}

static int start_monitor_listener() {
    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) { perror("socket"); return -1; }
    int opt = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(MONITOR_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(s, (struct sockaddr*)&addr, sizeof(addr)) < 0) { perror("bind"); close(s); return -1; }
    if (listen(s, 1) < 0) { perror("listen"); close(s); return -1; }
    return s;
}

static int connect_to_server() {
    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) { perror("socket"); return -1; }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_HOST, &addr.sin_addr);
    if (connect(s, (struct sockaddr*)&addr, sizeof(addr)) < 0) { perror("connect"); close(s); return -1; }
    return s;
}

int main() {
    /* connect to server */
    server_fd = connect_to_server();
    if (server_fd < 0) {
        fprintf(stderr, "Cannot connect to server at %s:%d\n", SERVER_HOST, SERVER_PORT);
        return 1;
    }
    printf("Connected to server %s:%d\n", SERVER_HOST, SERVER_PORT);

    /* start monitor listener */
    int mlisten = start_monitor_listener();
    if (mlisten < 0) {
        fprintf(stderr, "Failed to start monitor listener\n");
        return 1;
    }
    printf("Monitor listening on port %d\n", MONITOR_PORT);

    /* accept one monitor connection and spawn thread to handle */
    struct sockaddr_in peer;
    socklen_t plen = sizeof(peer);
    int mfd = accept(mlisten, (struct sockaddr*)&peer, &plen);
    if (mfd < 0) {
        perror("accept");
        close(mlisten);
        return 1;
    }
    printf("Monitor connected\n");
    pthread_t thr;
    pthread_create(&thr, NULL, monitor_thread_fn, &mfd);

    /* main thread can just wait for monitor thread to exit */
    pthread_join(thr, NULL);

    close(server_fd);
    close(mlisten);
    return 0;
}

