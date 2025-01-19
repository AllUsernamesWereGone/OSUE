#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>

#define BUFFER_SIZE 1024

int running = 1;

void handle_signal(int signal) {
    running = 0;
}

void print_usage() {
    printf("Usage: server [-p PORT] [-i INDEX] DOC_ROOT\n");
    exit(1);
}

int main(int argc, char *argv[]) {
    int port = 8080; // Default port
    char *doc_root = NULL, *index_file = "index.html";
    int opt;

    while ((opt = getopt(argc, argv, "p:i:")) != -1) {
        switch (opt) {
            case 'p': port = atoi(optarg); break;
            case 'i': index_file = optarg; break;
            default: print_usage();
        }
    }

    if (optind < argc) {
        doc_root = argv[optind];
    } else {
        print_usage();
    }

    printf("Serving %s on port %d\n", doc_root, port);

    // Handle SIGINT and SIGTERM
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    // Create server socket
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("Socket creation failed");
        return 1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        return 1;
    }

    if (listen(server_sock, 5) < 0) {
        perror("Listen failed");
        return 1;
    }

    printf("Server is running...\n");

    while (running) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len);
        if (client_sock < 0) continue;

        // Read request
        char buffer[BUFFER_SIZE];
        read(client_sock, buffer, sizeof(buffer));
        printf("Request: %s\n", buffer);

        // Send response
        char response[BUFFER_SIZE];
        snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello World!\n");
        write(client_sock, response, strlen(response));
        close(client_sock);
    }

    close(server_sock);
    printf("Server stopped.\n");
    return 0;
}