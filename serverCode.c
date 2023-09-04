#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PORT 2728
#define ROOT_DIR "public" // The directory where your PHP files are stored

// Function to serve the requested PHP file
void serve_php(int client_socket, const char *filename)
{
    char response[2048];
    char buffer[1024];
    FILE *file = fopen(filename, "r");

    if (file == NULL)
    {
        snprintf(response, sizeof(response), "HTTP/1.1 404 Not Found\r\n\r\nFile not found");
        send(client_socket, response, strlen(response), 0);
        return;
    }

    snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\n\r\n");

    while (fgets(buffer, sizeof(buffer), file) != NULL)
    {
        strcat(response, buffer);
    }

    send(client_socket, response, strlen(response), 0);
    fclose(file);
}

int main()
{
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        perror("Socket creation failed");
        return 1;
    }

    // Configure server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Bind socket to address
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("Socket binding failed");
        return 1;
    }

    // Listen for incoming connections
    if (listen(server_socket, 10) == -1)
    {
        perror("Listen failed");
        return 1;
    }

    printf("Server is listening on port %d...\n", PORT);

    // Accept and handle connections
    while (1)
    {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket == -1)
        {
            perror("Accept failed");
            continue;
        }

        // Read request from client
        char request[1024];
        recv(client_socket, request, sizeof(request), 0);

        // Parse the request to get the requested file
        char method[10], filename[50];
        sscanf(request, "%s %s", method, filename);

        // Handle requests for the root path ("/") by serving the index.php file
        if (strcmp(filename, "/") == 0)
        {
            snprintf(filename, sizeof(filename), "index.php");
        }

        // Construct the full path of the requested file
        char full_path[100];
        snprintf(full_path, sizeof(full_path), "%s/%s", ROOT_DIR, filename);

        // Check if the requested file is a PHP file
        if (strstr(full_path, ".php") != NULL)
        {
            serve_php(client_socket, full_path);
        }
        else
        {
            // ... (previous non-PHP file serving code)
        }

        // Close the client socket
        close(client_socket);
    }

    // Close the server socket
    close(server_socket);

    return 0;
}
