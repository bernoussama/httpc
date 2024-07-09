#include <errno.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

typedef struct http_request {
  char *method;
  char *path;
  char *version;
  char **headers; // Maximum 10 headers
  char *body;
} http_request;

http_request *parse_request(char *request);

int main() {
  // Disable output buffering
  setbuf(stdout, NULL);
  setbuf(stderr, NULL);

  // You can use print statements as follows for debugging, they'll be visible
  // when running tests.
  printf("Logs from your program will appear here!\n");

  // Uncomment this block to pass the first stage
  //
  int server_fd, client_addr_len;
  struct sockaddr_in client_addr;
  char request_buffer[1024];

  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd == -1) {
    printf("Socket creation failed: %s...\n", strerror(errno));
    return 1;
  }

  // Since the tester restarts your program quite often, setting SO_REUSEADDR
  // ensures that we don't run into 'Address already in use' errors
  int reuse = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) <
      0) {
    printf("SO_REUSEADDR failed: %s \n", strerror(errno));
    return 1;
  }

  struct sockaddr_in serv_addr = {
      .sin_family = AF_INET,
      .sin_port = htons(4221),
      .sin_addr = {htonl(INADDR_ANY)},
  };

  if (bind(server_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) != 0) {
    printf("Bind failed: %s \n", strerror(errno));
    return 1;
  }

  int connection_backlog = 5;
  if (listen(server_fd, connection_backlog) != 0) {
    printf("Listen failed: %s \n", strerror(errno));
    return 1;
  }

  printf("Waiting for a client to connect...\n");
  client_addr_len = sizeof(client_addr);

  int fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
  printf("Client connected\n");

  // Receive the HTTP request
  int bytes_received = recv(fd, request_buffer, sizeof(request_buffer) - 1, 0);
  if (bytes_received == -1) {
    perror("recv failed");
    exit(1);
  }
  request_buffer[bytes_received] = '\0';
  char *request_string = request_buffer;
  http_request *request = parse_request(request_string);
  if (request == NULL) {
    printf("Failed to parse request\n");
    return 1;
  }

  char *response;
  // if path is "/"
  if (strcmp(request->path, "/") == 0) {
    response = "HTTP/1.1 200 OK\r\n\r\n";
  } else {
    response = "HTTP/1.1 404 Not Found\r\n\r\n";
  }

  // server response
  int bytes_sent = send(fd, response, strlen(response), 0);

  close(server_fd);

  return 0;
}

http_request *parse_request(char *request) {
  http_request *req = malloc(sizeof(http_request));
  if (request == NULL) {
    return (NULL); // Error handling: memory allocation failed
  }

  // parse method
  char *method = strtok(request, " ");
  if (method == NULL) {
    // Error handling: invalid request format
    free(req);
    return (NULL);
  }
  req->method = malloc(strlen(method) + 1);
  strcpy(req->method, method);

  // parse path
  char *path = strtok(NULL, " ");
  if (path == NULL) {
    // Error handling: invalid request format
    free(req->method);
    free(req);
    return (NULL);
  }
  req->path = malloc(strlen(path) + 1);
  strcpy(req->path, path);

  // parse version
  char *version = strtok(NULL, "\r\n");
  if (version == NULL) {
    // Error handling: invalid request format
    free(req->method);
    free(req->path);
    free(req);
    return (NULL);
  }
  req->version = malloc(strlen(version) + 1);
  strcpy(req->version, version);

  return req;
}
