# Tema 4 PCOM

**Author:** Giuglan Catalin-Ionut  
**Group:** 325CB

## Introduction
For solving this assignment, I started with the skeleton provided in Lab 9 ([Lab 9 Skeleton](https://gitlab.cs.pub.ro/pcom/pcom-laboratoare-public/-/tree/master/lab9)), from which I used the files `buffer.c`, `buffer.h`, `helpers.c`, `helpers.h`, `requests.c`, and `requests.h`. Additionally, I used the files `parson.c` and `parson.h` from one of the links mentioned in the assignment ([Parson Library](https://github.com/kgabis/parson)).

## Usage of Parson Library
I used the `parson.c` and `parson.h` files to create JSON objects when sending data to the server and to extract information from the responses received from the server.

## Functions in `requests.c`
I implemented the functions already present in the lab skeleton and added the `char *compute_delete_request` function to enable DELETE commands.

## Functions in `client.c`
All functions used for the assignment commands are implemented here. Each function is described below:

- **is_logged_in()**: Checks if the user is logged in. Returns 0 if the "cookie" string is empty (user not logged in), otherwise returns 1.
- **has_access_to_library()**: Checks if the user has access to the library in a similar manner to `is_logged_in()`. Returns 0 if the "token" string is empty, otherwise returns 1.
- **get_response_code(char *response)**: Extracts the HTTP response code from a given string. Searches for the substring "HTTP/1.1" and returns the response code as an integer, or -1 if the substring is not found.
- **get_cookie(char *response)**: Searches for a cookie in the response and saves it in the global "cookie" variable if found.
- **remove_underscores(char *string)**: Removes all underscore ("_") characters from a given string.
- **get_token(char *response)**: Similar to `get_cookie`, extracts the token from the response and saves it in the global "token" variable.

## Command Handling Functions
- **handle_register(int sockfd, char *route, char *content_type)**: Handles the "register" command. The user enters a username and password, which are sent via a POST request to the server. Depending on the response, an appropriate message is displayed.
- **handle_login(int sockfd, char *route, char *content_type)**: Handles the "login" command. Similar to `handle_register`, the user enters a username and password, which are sent via a POST request to the server. The user is logged in or not based on the response, and the received cookie is saved if logged in.
- **handle_enter_library(int sockfd, char *route)**: Handles the "enter_library" command. Checks if the user is logged in and sends a GET request to the server to gain access to the library if logged in. The received token is saved, and an appropriate message is displayed.
- **handle_get_books(int sockfd, char *route)**: Requests the list of books from the library by sending a GET request to the server. If access is granted, the list of books is displayed, otherwise, an error message is shown.
- **handle_get_book(int sockfd, char *route, char *id)**: Similar to `handle_get_books`, but requests information about a specific book using the provided ID.
- **handle_add_book(int sockfd, char *route, char *content_type)**: The user enters book information, which is sent via a POST request to the server. Depending on the response, a corresponding message is displayed indicating whether the book was added or not.
- **handle_delete_book(int sockfd, char *route, char *id)**: Handles the deletion of a specific book from the library using the provided ID. A DELETE request is sent to the server, and the response indicates whether the book was deleted or not.
- **handle_logout(int sockfd, char *route)**: Sends a GET request to the server to log out the user. If successful, the cookie and token are reset for the next session, and the user is logged out with a corresponding message displayed.

## Main Function
In the `main` function, user commands are handled using an "if/else" structure. Depending on the entered command, the corresponding function is called. For commands requiring server interaction, the connection is opened and closed before and after calling the respective function. The program terminates when the user enters the "exit" command, closing the connection and breaking out of the "while" loop. If an unknown command is entered, an error message is displayed.
