#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>

#define BUFFER_SIZE 1024

void print_usage() {
    printf("Usage: client [-p PORT] [-o FILE | -d DIR] URL\n");
    exit(1);
}

int main(int argc, char *argv[]) {
    int port = 80; // Default HTTP port
    char *url = NULL, *output_file = NULL, *output_dir = NULL;
    char host[BUFFER_SIZE], path[BUFFER_SIZE];
    
    int opt;
    while ((opt = getopt(argc, argv, "p:o:d:")) != -1) {
        switch (opt) {
            case 'p': port = atoi(optarg); break;
            case 'o': output_file = optarg; break;
            case 'd': output_dir = optarg; break;
            default: print_usage();
        }
    }

    if (optind < argc) {
        url = argv[optind];
    } else {
        print_usage();
    }

    if (sscanf(url, "http://%[^/]%s", host, path) != 2) {
        strcpy(path, "/");
    }

    printf("Host: %s\n", host);
    printf("Path: %s\n", path);
    printf("Port: %d\n", port);

    // Create socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // Resolve hostname
    struct hostent *server = gethostbyname(host);
    if (!server) {
        fprintf(stderr, "Host not found: %s\n", host);
        return 1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);

    // Connect to server
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        return 1;
    }

    // Build HTTP GET request
    char request[BUFFER_SIZE];
    snprintf(request, sizeof(request), "GET %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", path, host);
    send(sock, request, strlen(request), 0);

    // Receive response
    char buffer[BUFFER_SIZE];
    FILE *output = stdout;
    if (output_file) output = fopen(output_file, "w");
    else if (output_dir) {
        char file_path[BUFFER_SIZE];
        snprintf(file_path, sizeof(file_path), "%s/%s", output_dir, "index.html");
        output = fopen(file_path, "w");
    }

    if (!output) {
        perror("File open failed");
        close(sock);
        return 1;
    }

    int bytes_received;
    while ((bytes_received = recv(sock, buffer, sizeof(buffer), 0)) > 0) {
        fwrite(buffer, 1, bytes_received, output);
    }

    if (output != stdout) fclose(output);
    close(sock);
    return 0;
}