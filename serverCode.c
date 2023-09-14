// standard C libraries needed for socket programming and file operations.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctype.h>

int main()
{
    int server_socket;                 // to represent the server's socket
    struct sockaddr_in server_address; // to store information about the server's address.

    server_socket = socket(AF_INET, SOCK_STREAM, 0); // create a socket using the socket function
    // AF_INET-address family, SOCK_STREAM-sockect type(in this case for TCP socket), 0-default protocol

    // this section initializes the server_address structure
    server_address.sin_family = AF_INET;         // address family
    server_address.sin_port = htons(2728);       // port number
    server_address.sin_addr.s_addr = INADDR_ANY; // binds the server to all available network interfaces

    bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address));
    // binds the socket to the server's address. It associates the socket with the IP address and port specified in 'server_address'.

    listen(server_socket, 5); // server socket listen for incoming connections. It allows up to 5 pending connections in the server's queue.

    printf("Server listening on port 2728...\n");

    // An infinite loop is started to continuously handle incoming client connections.
    int client_socket;
    while (1)
    {
        client_socket = accept(server_socket, NULL, NULL);
        /* accepts an incoming client connection and returns a new socket for communication with the client.
        The NULL arguments are used to ignore client address information. */

        char request[1024];
        recv(client_socket, request, sizeof(request), 0); // receives the client's HTTP request and stores it in the request buffer.

        char method[32], path[256];
        sscanf(request, "%s %s", method, path);
        // the request is parsed to extract the HTTP method (GET/POST) and the requested path ("/index.html")

        if (strcmp(method, "GET") == 0) // checks if the HTTP method is GET. If it is, it proceeds to handle the GET request
        {

            if (strstr(path, ".php") != NULL) // checks if the requested path contains ".html," indicating that it's an HTML file request.
            {
                char command[256];
                sprintf(command, "php %s", path + 1);
                FILE *php_output = popen(command, "r");
                /* If the requested path is an HTML file, the server prepares to execute it as PHP. It constructs a command to run
                php on the requested file (excluding the initial "/"). It then uses popen to execute this command and opens a
                pipe to read the PHP script's output. */

                char response_header[] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
                send(client_socket, response_header, strlen(response_header), 0);
                // The server sends an HTTP response header back to the client, indicating that the response is OK and that it's in HTML format.

                char buffer[1024];
                while (fgets(buffer, sizeof(buffer), php_output) != NULL)
                {
                    send(client_socket, buffer, strlen(buffer), 0); // server reads the PHP script's output from the pipe and sends it to the client.
                }

                pclose(php_output); // server closes the PHP output pipe
            }
        } // end of handling a GET request.

        else if (strcmp(method, "POST") == 0) // server checks if the HTTP method is POST. If it is, it proceeds to handle the POST request.
        {

            char *post_data = strstr(request, "\r\n\r\n"); // locates the POST data in the request
            if (post_data != NULL)
            // If POST data is found, the server advances the pointer to start reading the actual data
            {
                post_data += 4;

                char php_script_name[64];
                snprintf(php_script_name, sizeof(php_script_name), "temp_script_%d.php", getpid());
                FILE *php_script_file = fopen(php_script_name, "w");
                // server generates a unique PHP script name based on the process ID and creates a new PHP script file with that name.
                if (php_script_file)
                {

                    fprintf(php_script_file, "<?php\n");

                    fprintf(php_script_file, "$_POST = array();\n");
                    fprintf(php_script_file, "parse_str(\"%s\", $_GET);\n", post_data);

                    fprintf(php_script_file, "include '%s';\n", path + 1);

                    fprintf(php_script_file, "?>\n");
                    fclose(php_script_file);
                    /*writes PHP code to the script file. It first writes the PHP opening tag (<?php), creates an $_POST array and
                    populates it with the parsed POST data, includes the requested PHP file (excluding the initial "/"), and closes
                    the PHP script.*/

                    FILE *php_script_file_read = fopen(php_script_name, "r");
                    if (php_script_file_read)
                    {
                        char script_buffer[1024];
                        while (fgets(script_buffer, sizeof(script_buffer), php_script_file_read) != NULL)
                        {
                            printf("%s\n", script_buffer);
                        }
                        fclose(php_script_file_read);
                    }
                    // The server opens the PHP script file for reading and prints its content to the console for debugging purposes.

                    char command[256];

                    char response_header[] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
                    send(client_socket, response_header, strlen(response_header), 0); // HTTP response header indicating a successful response
                    sprintf(command, "php %s", php_script_name);

                    FILE *php_output = popen(command, "r");
                    // constructs a command to execute the PHP script using 'php', opens a pipe to read the script's output, and stores it in 'php_output'

                    char buffer[1024];
                    while (fgets(buffer, sizeof(buffer), php_output) != NULL)
                    {
                        send(client_socket, buffer, strlen(buffer), 0);
                    }

                    pclose(php_output);
                    remove(php_script_name);
                    /*reads the PHP script's output from the pipe, sends it to the client, closes the PHP output pipe, and removes
                    the temporary PHP script file*/
                }
            } // end of handling a POST request.
        }
        close(client_socket);
        shutdown(client_socket, SHUT_RDWR);
        // closes the client socket and shuts it down. continues to listen for new connections in an infinite loop
    }
}