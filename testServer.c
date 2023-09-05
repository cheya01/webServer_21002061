#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PORT 2726
#define ROOT_DIR "htdocs" // The directory where your PHP files and images are stored

// Function to serve static files (e.g., HTML, CSS, JavaScript, images)
void serve_static(int client_socket, const char *filename)
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

// Function to determine the file type based on the file extension
const char *get_content_type(const char *filename)
{
    const char *dot = strrchr(filename, '.');
    if (!dot || dot == filename)
        return "application/octet-stream";
    if (strcasecmp(dot, ".html") == 0)
        return "text/html";
    if (strcasecmp(dot, ".jpg") == 0 || strcasecmp(dot, ".jpeg") == 0)
        return "image/jpeg";
    if (strcasecmp(dot, ".png") == 0)
        return "image/png";
    if (strcasecmp(dot, ".gif") == 0)
        return "image/gif";
    if (strcasecmp(dot, ".css") == 0)
        return "text/css";
    if (strcasecmp(dot, ".js") == 0)
        return "application/javascript";
    return "application/octet-stream";
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
            // Determine the content type based on the file extension
            const char *content_type = get_content_type(full_path);

            // Serve static files (images, HTML, CSS, JavaScript)
            serve_static(client_socket, full_path);

            // Send the appropriate content type header
            snprintf(request, sizeof(request), "Content-Type: %s\r\n\r\n", content_type);
            send(client_socket, request, strlen(request), 0);
        }

        // Close the client socket
        close(client_socket);
    }

    // Close the server socket
    close(server_socket);

    return 0;
}
