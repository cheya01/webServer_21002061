#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctype.h>

// Function to URL decode data (replace "+" with " " and decode %xx sequences)
int main()
{
    int server_socket;
    struct sockaddr_in server_address;

    // Create a socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    // Configure the server address
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(2728);
    server_address.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket to the address
    bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address));

    // Listen for incoming connections
    listen(server_socket, 5);

    printf("Server listening on port 2728...\n");

    // Accept and handle incoming connections
    int client_socket;
    while (1)
    {
        client_socket = accept(server_socket, NULL, NULL);

        // Handle the client request
        char request[1024];
        recv(client_socket, request, sizeof(request), 0);

        // Parse the HTTP request (simplified)
        char method[32], path[256];
        sscanf(request, "%s %s", method, path);

        if (strcmp(method, "GET") == 0)
        {
            // Check if the requested path is a PHP script
            if (strstr(path, ".html") != NULL)
            {

                // Execute the PHP script and capture output (simplified)
                char command[256];
                sprintf(command, "php-cgi %s", path + 1);
                FILE *php_output = popen(command, "r");

                // Send HTTP response header
                char response_header[] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
                send(client_socket, response_header, strlen(response_header), 0);

                // Send PHP script output to the client
                char buffer[1024];
                while (fgets(buffer, sizeof(buffer), php_output) != NULL)
                {
                    send(client_socket, buffer, strlen(buffer), 0);
                }

                // Close the PHP output
                pclose(php_output);
            }
        }
        else if (strcmp(method, "POST") == 0)
        {
            // Handle POST request
            char *post_data = strstr(request, "\r\n\r\n"); // Locate the POST data in the request
            if (post_data != NULL)
            {
                post_data += 4; // Move past the "\r\n\r\n" separator
                // Now, post_data contains the raw POST data

                // Generate a unique file name for the PHP script
                char php_script_name[64];
                snprintf(php_script_name, sizeof(php_script_name), "temp_script_%d.php", getpid());
                FILE *php_script_file = fopen(php_script_name, "w");
                if (php_script_file)
                {
                    // Append the POST data to the PHP script
                    fprintf(php_script_file, "<?php\n");

                    fprintf(php_script_file, "$_POST = array();\n");
                    fprintf(php_script_file, "parse_str(\"%s\", $_POST);\n", post_data);

                    // Include your existing PHP script
                    fprintf(php_script_file, "include '%s';\n", path + 1);

                    fprintf(php_script_file, "?>\n");
                    fclose(php_script_file);

                    // print newly created php file
                    FILE *php_script_file_read = fopen(php_script_name, "r");
                    if (php_script_file_read)
                    {
                        char script_buffer[1024];
                        while (fgets(script_buffer, sizeof(script_buffer), php_script_file_read) != NULL)
                        {
                            printf("%s", script_buffer);
                        }
                        fclose(php_script_file_read);
                    }

                    // Execute the PHP script and capture output
                    char command[256];

                    // Send HTTP response header
                    char response_header[] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
                    send(client_socket, response_header, strlen(response_header), 0);
                    sprintf(command, "php %s", php_script_name);

                    // Execute the PHP script
                    FILE *php_output = popen(command, "r");

                    // Send PHP script output to the client
                    char buffer[1024];
                    while (fgets(buffer, sizeof(buffer), php_output) != NULL)
                    {
                        send(client_socket, buffer, strlen(buffer), 0);
                    }

                    // Close the PHP output and remove the temporary script file
                    pclose(php_output);
                    remove(php_script_name);
                }
            }
        }
        close(client_socket);
        shutdown(client_socket, SHUT_RDWR);
    }
}