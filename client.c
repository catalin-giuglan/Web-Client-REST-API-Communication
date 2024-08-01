#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include "parson.h"

#define HOST "34.246.184.49"
#define PORT 8080

char command[LINELEN];
char cookie[LINELEN];
char token[LINELEN];

int is_logged_in() {
	if (strlen(cookie) == 0) {
		return 0;
	}
	return 1;
}

int has_access_to_library() {
	if (strlen(token) == 0) {
		return 0;
	}
	return 1;
}

int get_response_code(char *response) {
	char *start = NULL;
	char *end = NULL;
	char code[4];

	start = strstr(response, "HTTP/1.1 ");
	if (start == NULL) {
		return -1;
	}
	start = start + strlen("HTTP/1.1 ");

	end = strstr(start, " ");
	if (end == NULL) {
		return -1;
	}

	strncpy(code, start, end - start);
	code[end - start] = '\0';
	return atoi(code);
}

void get_cookie(char *response) {
	char *start = NULL;
	char *end = NULL;

	start = strstr(response, "Set-Cookie: ");
	if (start == NULL) {
		return;
	}
	start = start + strlen("Set-Cookie: ");

	end = strstr(start, "\r\n");
	if (end == NULL) {
		return;
	}

	memset(cookie, 0, LINELEN);
	strncpy(cookie, start, end - start);
	cookie[end - start] = '\0';
}

void remove_underscores(char *string) {
	char *read = string;
	char *write = string;
	while (*read) {
		if (*read != '_') {
			*write++ = *read;
		}
		read++;
	}
	*write = '\0';
}

void get_token(char *response) {
	char *start = NULL;
	char *end = NULL;

	start = strstr(response, "{\"token\":\"");
	if (start == NULL) {
		return;
	}
	start = start + strlen("{\"token\":\"");

	end = strstr(start, "\"}");
	if (end == NULL) {
		return;
	}

	memset(token, 0, LINELEN);
	strncpy(token, start, end - start);
	token[end - start] = '\0';
}

void handle_register(int sockfd, char *route, char *content_type) {
	char username[LINELEN];
	printf("username=");
	fgets(username, LINELEN, stdin);
	username[strcspn(username, "\n")] = 0;
	char password[LINELEN];
	printf("password=");
	fgets(password, LINELEN, stdin);
	password[strcspn(password, "\n")] = 0;

	if (strchr(username, ' ') != NULL) {
		printf("[Error] Username cannot contain spaces!\n");
		return;
	}
	if (strchr(password, ' ') != NULL) {
		printf("[Error] Password cannot contain spaces!\n");
		return;
	}

	JSON_Value *json_value = json_value_init_object();
	JSON_Object *json_object = json_value_get_object(json_value);
	json_object_set_string(json_object, "username", username);
	json_object_set_string(json_object, "password", password);
	char *data = (char *) calloc(BUFLEN, sizeof(char));
	json_serialize_to_buffer(json_value, data, BUFLEN);
	char *message = compute_post_request(HOST, route, content_type, &data, 1, NULL, 0, NULL);
	send_to_server(sockfd, message);
	char *response = receive_from_server(sockfd);

	if (get_response_code(response) > 199 && get_response_code(response) < 300) {
		printf("[Success] Registered successfully!\n");
	} else {
		printf("[Error] Username is taken!\n");
	}

	json_value_free(json_value);
	json_free_serialized_string(data);
	free(message);
	free(response);
}

void handle_login(int sockfd, char *route, char *content_type) {
	char username[LINELEN];
	printf("username=");
	fgets(username, LINELEN, stdin);
	username[strcspn(username, "\n")] = 0;
	char password[LINELEN];
	printf("password=");
	fgets(password, LINELEN, stdin);
	password[strcspn(password, "\n")] = 0;

	if (strchr(username, ' ') != NULL) {
		printf("[Error] Username cannot contain spaces!\n");
		return;
	}
	if (strchr(password, ' ') != NULL) {
		printf("[Error] Password cannot contain spaces!\n");
		return;
	}

	JSON_Value *json_value = json_value_init_object();
	JSON_Object *json_object = json_value_get_object(json_value);
	json_object_set_string(json_object, "username", username);
	json_object_set_string(json_object, "password", password);
	char *data = (char *) calloc(BUFLEN, sizeof(char));
	json_serialize_to_buffer(json_value, data, BUFLEN);
	char *message = compute_post_request(HOST, route, content_type, &data, 1, NULL, 0, NULL);
	send_to_server(sockfd, message);
	char *response = receive_from_server(sockfd);

	if (get_response_code(response) > 199 && get_response_code(response) < 300) {
		get_cookie(response);
		printf("[Success] Logged in successfully!\n");
	} else {
		printf("[Error] Invalid credentials!\n");
	}

	json_value_free(json_value);
	json_free_serialized_string(data);
	free(message);
	free(response);
}

void handle_enter_library(int sockfd, char *route) {
	char *temp_cookie = (char *) calloc(LINELEN, sizeof(char));
	strcpy(temp_cookie, cookie);
	char *message = compute_get_request(HOST, route, NULL, &temp_cookie, 1, token);
	send_to_server(sockfd, message);
	char *response = receive_from_server(sockfd);

	if (get_response_code(response) > 199 && get_response_code(response) < 300) {
		get_token(response);
		printf("[Success] Entered library successfully!\n");
	} else {
		printf("[Error] Could not enter library!\n");
	}

	free(temp_cookie);
	free(message);
	free(response);
}

void handle_get_books(int sockfd, char *route) {
	char *temp_cookie = (char *) calloc(LINELEN, sizeof(char));
	strcpy(temp_cookie, cookie);
	char *message = compute_get_request(HOST, route, NULL, &temp_cookie, 1, token);
	send_to_server(sockfd, message);
	char *response = receive_from_server(sockfd);

	if (get_response_code(response) > 199 && get_response_code(response) < 300) {
		if (strstr(response, "[]") != NULL) {
			printf("[Success] No books found!\n");
		} else {
			JSON_Value *json_value = json_parse_string(strstr(response, "[{\""));
			char *books = json_serialize_to_string_pretty(json_value);
			printf("%s\n", books);
		}
	} else {
		printf("[Error] Could not get books!\n");
	}

	free(temp_cookie);
	free(message);
	free(response);
}

void handle_get_book(int sockfd, char *route) {
	char id[LINELEN];
	printf("id=");
	fgets(id, LINELEN, stdin);
	id[strcspn(id, "\n")] = 0;

	if (strspn(id, "0123456789") != strlen(id)) {
		printf("[Error] Invalid id!\n");
		return;
	}

	char *temp_cookie = (char *) calloc(LINELEN, sizeof(char));
	strcpy(temp_cookie, cookie);
	char *temp_route = (char *) calloc(LINELEN, sizeof(char));
	strcpy(temp_route, route);
	strcat(temp_route, "/");
	strcat(temp_route, id);
	char *message = compute_get_request(HOST, temp_route, NULL, &temp_cookie, 1, token);
	send_to_server(sockfd, message);
	char *response = receive_from_server(sockfd);

	if (get_response_code(response) > 199 && get_response_code(response) < 300) {
		JSON_Value *json_value = json_parse_string(basic_extract_json_response(response));
		char *book = json_serialize_to_string_pretty(json_value);
		printf("%s\n", book);
	} else {
		printf("[Error] Could not get book!\n");
	}

	free(temp_cookie);
	free(temp_route);
	free(message);
	free(response);
}

void handle_add_book(int sockfd, char *route, char *content_type) {
	char title[LINELEN];
	printf("title=");
	fgets(title, LINELEN, stdin);
	title[strcspn(title, "\n")] = 0;
	char author[LINELEN];
	printf("author=");
	fgets(author, LINELEN, stdin);
	author[strcspn(author, "\n")] = 0;
	char genre[LINELEN];
	printf("genre=");
	fgets(genre, LINELEN, stdin);
	genre[strcspn(genre, "\n")] = 0;
	char publisher[LINELEN];
	printf("publisher=");
	fgets(publisher, LINELEN, stdin);
	publisher[strcspn(publisher, "\n")] = 0;
	char page_count[LINELEN];
	printf("page_count=");
	fgets(page_count, LINELEN, stdin);
	page_count[strcspn(page_count, "\n")] = 0;

	remove_underscores(title);
	remove_underscores(author);
	remove_underscores(genre);
	remove_underscores(publisher);

	if (strspn(page_count, "0123456789") != strlen(page_count)) {
		printf("[Error] Invalid page_count!\n");
		return;
	}

	JSON_Value *json_value = json_value_init_object();
	JSON_Object *json_object = json_value_get_object(json_value);
	json_object_set_string(json_object, "title", title);
	json_object_set_string(json_object, "author", author);
	json_object_set_string(json_object, "genre", genre);
	json_object_set_string(json_object, "publisher", publisher);
	json_object_set_number(json_object, "page_count", atoi(page_count));
	char *data = (char *) calloc(BUFLEN, sizeof(char));
	json_serialize_to_buffer(json_value, data, BUFLEN);
	char *temp_cookie = (char *) calloc(LINELEN, sizeof(char));
	strcpy(temp_cookie, cookie);
	char *message = compute_post_request(HOST, route, content_type, &data, 1, &temp_cookie, 1, token);
	send_to_server(sockfd, message);
	char *response = receive_from_server(sockfd);

	if (get_response_code(response) > 199 && get_response_code(response) < 300) {
		printf("[Success] Added book successfully!\n");
	} else {
		printf("[Error] Could not add book!\n");
	}

	json_value_free(json_value);
	json_free_serialized_string(data);
	free(temp_cookie);
	free(message);
	free(response);
}

void handle_delete_book(int sockfd, char *route) {
	char id[LINELEN];
	printf("id=");
	fgets(id, LINELEN, stdin);
	id[strcspn(id, "\n")] = 0;

	if (strspn(id, "0123456789") != strlen(id)) {
		printf("[Error] Invalid id!\n");
		return;
	}

	char *temp_cookie = (char *) calloc(LINELEN, sizeof(char));
	strcpy(temp_cookie, cookie);
	char *temp_route = (char *) calloc(LINELEN, sizeof(char));
	strcpy(temp_route, route);
	strcat(temp_route, "/");
	strcat(temp_route, id);
	char *message = compute_delete_request(HOST, temp_route, NULL, &temp_cookie, 1, token);
	send_to_server(sockfd, message);
	char *response = receive_from_server(sockfd);

	if (get_response_code(response) > 199 && get_response_code(response) < 300) {
		printf("[Success] Deleted book successfully!\n");
	} else {
		printf("[Error] Could not delete book!\n");
	}

	free(temp_cookie);
	free(temp_route);
	free(message);
	free(response);
}

void handle_logout(int sockfd, char *route) {
	char *temp_cookie = (char *) calloc(LINELEN, sizeof(char));
	strcpy(temp_cookie, cookie);
	char *message = compute_get_request(HOST, route, NULL, &temp_cookie, 1, token);
	send_to_server(sockfd, message);
	char *response = receive_from_server(sockfd);

	if (get_response_code(response) > 199 && get_response_code(response) < 300) {
		memset(cookie, 0, LINELEN);
		memset(token, 0, LINELEN);
		printf("[Success] Logged out successfully!\n");
	} else {
		printf("[Error] Could not log out!\n");
	}

	free(temp_cookie);
	free(message);
	free(response);
}

int main(int argc, char *argv[]) {
	int sockfd = -1;
	while (1) {
		fgets(command, LINELEN, stdin);
		command[strcspn(command, "\n")] = 0;
		if (strcmp(command, "register") == 0) {
			if (is_logged_in()) {
				printf("[Error] You are already logged in!\n");
				continue;
			}
			sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
			handle_register(sockfd, "/api/v1/tema/auth/register", "application/json");
			close_connection(sockfd);
		} else if (strcmp(command, "login") == 0) {
			if (is_logged_in()) {
				printf("[Error] You are already logged in!\n");
				continue;
			}
			sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
			handle_login(sockfd, "/api/v1/tema/auth/login", "application/json");
			close_connection(sockfd);
		} else if (strcmp(command, "enter_library") == 0) {
			if (!is_logged_in()) {
				printf("[Error] You are not logged in!\n");
				continue;
			}
			if (has_access_to_library()) {
				printf("[Error] You already have access to the library!\n");
				continue;
			}
			sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
			handle_enter_library(sockfd, "/api/v1/tema/library/access");
			close_connection(sockfd);
		} else if (strcmp(command, "get_books") == 0) {
			if (!has_access_to_library()) {
				printf("[Error] You do not have access to the library!\n");
				continue;
			}
			sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
			handle_get_books(sockfd, "/api/v1/tema/library/books");
			close_connection(sockfd);
		} else if (strcmp(command, "get_book") == 0) {
			if (!has_access_to_library()) {
				printf("[Error] You do not have access to the library!\n");
				continue;
			}
			sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
			handle_get_book(sockfd, "/api/v1/tema/library/books");
			close_connection(sockfd);
		} else if (strcmp(command, "add_book") == 0) {
			if (!has_access_to_library()) {
				printf("[Error] You do not have access to the library!\n");
				continue;
			}
			sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
			handle_add_book(sockfd, "/api/v1/tema/library/books", "application/json");
			close_connection(sockfd);
		} else if (strcmp(command, "delete_book") == 0) {
			if (!has_access_to_library()) {
				printf("[Error] You do not have access to the library!\n");
				continue;
			}
			sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
			handle_delete_book(sockfd, "/api/v1/tema/library/books");
			close_connection(sockfd);
		} else if (strcmp(command, "logout") == 0) {
			if (!is_logged_in()) {
				printf("[Error] You are not logged in!\n");
				continue;
			}
			sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);
			handle_logout(sockfd, "/api/v1/tema/auth/logout");
			close_connection(sockfd);
		} else if (strcmp(command, "exit") == 0) {
			close_connection(sockfd);
			break;
		} else {
			printf("[Error] Command not found!\n");
		}
	}
	return 0;
}
