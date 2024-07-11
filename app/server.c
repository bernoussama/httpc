#include <errno.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

typedef struct http_request {
  char *method;
  char *path;
  char *version;
  char *headers[10]; // max 10 headers
  char *body;
} http_request;

http_request *parse_request(char *request);
char *gen_response(http_request *request);
void *handle_client(void *arg);
char *read_file(char *filename);

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

  int connection_backlog = 10;
  if (listen(server_fd, connection_backlog) != 0) {
    printf("Listen failed: %s \n", strerror(errno));
    return 1;
  }

  printf("Waiting for a client to connect...\n");
  client_addr_len = sizeof(client_addr);
  while (1) {
    int fd =
        accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
    printf("Client connected\n");
    if (fd == -1)
      continue;

    // // Allocate memory to hold both arguments for the thread (file descriptor
    // // and request buffer)
    // void *thread_args = malloc(sizeof(int) + sizeof(char *) * 2);
    // if (thread_args == NULL) {
    //   perror("malloc failed");
    //   exit(1);
    // }
    // // Set the first argument (file descriptor) in the allocated memory
    // *((int *)thread_args) = fd;
    // // Allocate memory for the request buffer copy within the thread
    // arguments char *request_buffer_copy = malloc(1024); if
    // (request_buffer_copy == NULL) {
    //   perror("malloc failed");
    //   free(thread_args); // Free previously allocated memory
    //   exit(1);
    // }
    // // Copy the request buffer into the thread arguments' buffer
    // memcpy(request_buffer_copy, request_buffer, 1024);
    // // Set the second argument (request buffer pointer) in the allocated
    // memory
    // ((char **)thread_args)[1] = request_buffer_copy;

    pthread_t thread_id;
    pthread_create(&thread_id, NULL, handle_client, (void *)&fd);
    pthread_detach(thread_id);
  }
  close(server_fd);

  return 0;
}

void *handle_client(void *arg) {

  int fd = *((int *)arg);
  char request_buffer[1024];
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
    return NULL;
  }

  char *response;
  response = gen_response(request);
  if (response != NULL) {
    int bytes_sent = send(fd, response, strlen(response), 0);
  } else {
    printf("Failed to generate response\n");
  }
  // server response

  free(request->method);
  free(request->path);
  free(request->version);
  // free(request->headers);
  free(request->body);
  free(request);
}

http_request *parse_request(char *request) {
  http_request *req = malloc(sizeof(http_request));
  if (request == NULL) {
    printf("failed to malloc\n");
    return (NULL); // Error handling: memory allocation failed
  }

  // parse method
  char *method = strtok(request, " ");
  if (method == NULL) {
    printf("method null\n");
    // Error handling: invalid request format
    free(req);
    return (NULL);
  }
  req->method = malloc(strlen(method) + 1);
  strcpy(req->method, method);

  // parse path
  char *path = strtok(NULL, " ");
  if (path == NULL) {
    printf("path null\n");
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
    printf("version null\n");
    // Error handling: invalid request format
    free(req->method);
    free(req->path);
    free(req);
    return (NULL);
  }
  req->version = malloc(strlen(version) + 1);
  strcpy(req->version, version);

  char *header = strtok(NULL, "\r\n");
  if (header == NULL) {
    printf("header null\n");
    // Error handling: invalid request format
    // free(req->version);
    // free(req->method);
    // free(req->path);
    // free(req);
    // return (NULL);
  }
  for (int i = 0; header != NULL; i++) {
    req->headers[i] = malloc(strlen(header) + 1);
    strcpy(req->headers[i], header);
    header = strtok(NULL, "\r\n");
  }

  return req;
}

char *gen_response(http_request *request) {

  char *response = NULL;
  char *res = NULL;

  if (strcmp(request->path, "/") == 0) {
    res = "HTTP/1.1 200 OK\r\n\r\n";
  } else {
    char *endpoint = strtok(request->path, "/");
    // char *endpoint = request->path;

    if (strcmp(endpoint, "user-agent") == 0) {
      char *str = NULL;
      for (int i = 0; i < 10; i++) {
        str = strtok(request->headers[i], ":");
        if (strcasecmp(str, "User-Agent") == 0) {
          break;
        }
      }
      // char *str = strtok(request->headers[1], ":");
      str = strtok(NULL, ": ");
      printf("User-Agent: %s\n", str);
      // if (str != NULL) {
      response = "HTTP/1.1 200 OK";

      char *tmp = "Content-Type: text/plain\r\nContent-Length:";
      char *headers = malloc(strlen(tmp) + sizeof(unsigned long) + 1);
      sprintf(headers, "%s %lu", tmp, strlen(str));

      res = malloc(strlen(response) + strlen(headers) + strlen(str) + 1);

      sprintf(res, "%s\r\n%s\r\n\r\n%s", response, headers, str);
      // maybe use snprintf instead
      // } else {
      //   res = "HTTP/1.1 400 Bad Request\r\n\r\n";
      // }
      free(headers);
      free(str);
    } else if (strcmp(endpoint, "echo") == 0) {
      char *str = strtok(NULL, "/");
      if (str != NULL) {
        response = "HTTP/1.1 200 OK";

        char *tmp = "Content-Type: text/plain\r\nContent-Length:";
        char *headers = malloc(strlen(tmp) + sizeof(unsigned long) + 1);
        sprintf(headers, "%s %lu", tmp, strlen(str));

        res = malloc(strlen(response) + strlen(headers) + strlen(str) + 1);

        sprintf(res, "%s\r\n%s\r\n\r\n%s", response, headers, str);
        // maybe use snprintf instead

        free(headers);
        free(str);
      } else {
        res = "HTTP/1.1 400 Bad Request\r\n\r\n";
      }
    } else if (strcmp(endpoint, "file") == 0) {
      char *str = strtok(NULL, "/");
      if (str != NULL) {
        response = "HTTP/1.1 200 OK";
        str = read_file(str);

        // char *tmp = "Content-Type: text/plain\r\nContent-Length:";
        char *tmp = "Content-Type: application/octet-stream\r\nContent-Length:";
        char *headers = malloc(strlen(tmp) + sizeof(unsigned long) + 1);
        sprintf(headers, "%s %lu", tmp, strlen(str));

        res = malloc(strlen(response) + strlen(headers) + strlen(str) + 1);

        sprintf(res, "%s\r\n%s\r\n\r\n%s", response, headers, str);
        // maybe use snprintf instead

        if (headers != NULL)
          free(headers);
        free(str);
      } else {
        res = "HTTP/1.1 400 Bad Request\r\n\r\n";
      }

    } else {
      res = "HTTP/1.1 404 Not Found\r\n\r\n";
    }
  }
  return res;
}

char *read_file(char *filename) {
  FILE *fp;
  // char filename[] = "example.txt"; // replace with your file name

  fp = fopen(filename, "r"); // open file in read-only mode
  if (fp == NULL) {
    printf("Error opening file\n");
    return NULL;
  }

  size_t fileSize;        // size of the file
  fseek(fp, 0, SEEK_END); // move to the end of the file
  fileSize = ftell(fp);   // get the current position (end of the file)
  rewind(fp);             // move back to the beginning of the file

  char *fileContent = malloc(fileSize + 1); // allocate memory for the content
  fread(fileContent, sizeof(char), fileSize,
        fp);                    // read the contents into the buffer
  fileContent[fileSize] = '\0'; // null-terminate the string

  printf("File contents: %s\n", fileContent);

  // free(fileContent); // don't forget to free the memory!
  fclose(fp);
  return fileContent;
}
