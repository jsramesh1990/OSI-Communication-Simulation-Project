/* Simple server that simulates decapsulation and replies with ACK.
 * Listens on port 10000 and expects JSON packets:
 * {"type":"packet","layers":["Application","Presentation",...],"payload":"Hello"}
 *
 * Reply with {"type":"packet","layers":[...],"payload":"ACK: Hello"}
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 10000
#define BACKLOG 5
#define BUFSIZE 8192

static int start_server_socket() {
    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) { perror("socket"); exit(1); }
    int opt = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(s, (struct sockaddr*)&addr, sizeof(addr)) < 0) { perror("bind"); exit(1); }
    if (listen(s, BACKLOG) < 0) { perror("listen"); exit(1); }
    return s;
}

/* very small helper to extract payload value from JSON - naive but works for project */
static void extract_payload(const char *buf, char *out, size_t maxlen) {
    const char *p = strstr(buf, "\"payload\"");
    if (!p) { out[0]='\0'; return; }
    p = strchr(p, ':');
    if (!p) { out[0]='\0'; return; }
    p++; // skip :
    while (*p==' '||*p=='\"' || *p=='\t') p++;
    // now read until closing quote or end
    const char *end = p;
    // if starts with quote
    if (*(p-1) == '"') {
        while (*end && *end != '"') end++;
    } else {
        while (*end && *end != ',' && *end != '}' && *end != '\n') end++;
    }
    size_t n = end - p;
    if (n >= maxlen) n = maxlen - 1;
    strncpy(out, p, n);
    out[n] = '\0';
}

/* send length-prefixed message (4 bytes big-endian) */
static int send_lp(int s, const char *data, size_t len) {
    uint32_t L = htonl((uint32_t)len);
    if (write(s, &L, 4) != 4) return -1;
    ssize_t w = write(s, data, len);
    return w == (ssize_t)len ? 0 : -1;
}

/* read length-prefixed */
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

int main() {
    int srv = start_server_socket();
    printf("Server listening on port %d\n", PORT);
    while (1) {
        struct sockaddr_in peer;
        socklen_t plen = sizeof(peer);
        int c = accept(srv, (struct sockaddr*)&peer, &plen);
        if (c < 0) { perror("accept"); continue; }
        printf("Client connected\n");
        char buf[BUFSIZE];
        int n = recv_lp(c, buf, sizeof(buf));
        if (n <= 0) { close(c); continue; }
        /* naive: print received packet */
        printf("Received packet JSON: %s\n", buf);
        /* extract payload */
        char payload[1024];
        extract_payload(buf, payload, sizeof(payload));
        printf("Payload: %s\n", payload);
        /* form reply JSON */
        char reply[2048];
        snprintf(reply, sizeof(reply),
                 "{\"type\":\"packet\",\"layers\":[\"Application\",\"Presentation\",\"Session\",\"Transport\",\"Network\",\"DataLink\",\"Physical\"],\"payload\":\"ACK: %s\"}",
                 payload);
        send_lp(c, reply, strlen(reply));
        printf("Reply sent.\n");
        close(c);
    }
    close(srv);
    return 0;
}

