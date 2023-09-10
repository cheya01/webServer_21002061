This code essentially creates a simple web server that can handle both GET and POST requests. It can serve static
HTML files and execute PHP scripts when requested. It's worth noting that this code is quite basic and doesn't
include advanced features or security measures commonly found in production-ready web servers.

Make sure you have a C compiler installed on your system. GCC is a commonly used compiler and works well for
this purpose.

Create a folder on your computer to hold these web server files.

For this program to work as intended, all web-page files (index.html and add.php) must be in the same level as server.exe
That means, server.c, server.exe, index.html and add.php must be in the same level.

Open your terminal or command prompt and navigate to the folder containing the server.C file

Run the following command to compile the code:
gcc server.c -o server

After compilation, run the web server by executing the following command:
./server

Open a web browser and enter the following URL:
http://localhost:2728/index.html

You should see the "Addition Form" displayed on the web page. Enter two numbers and click "submit"
to see the result.

To stop the web server, press Ctrl+C in the terminal where the server is running