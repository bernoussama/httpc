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

#define BUFFER_SIZE 1024

typedef struct http_request
{
  char *method;
  char *path;
  char *version;
  char *headers[10]; // max 10 headers
  char *body;
} http_request;

http_request *parse_request(char *request);
char *gen_response(http_request *request);
void *handle_client(void *arg);
char *read_file(char *file_path);

char *directory = NULL;
int main(int argc, char *argv[])
{
  // Disable output buffering
  setbuf(stdout, NULL);
  setbuf(stderr, NULL);

  // You can use print statements as follows for debugging, they'll be visible
  // when running tests.

  if (argc >= 2)
  {
    // Check if the argument is "--directory"
    if (strcmp(argv[1], "--directory") == 0)
    {
      if (argc < 3)
      {
        fprintf(stderr, "Error: Missing directory path\n");
        return 1;
      }
      directory = argv[2];
    }
  }

  int server_fd, client_addr_len;
  struct sockaddr_in client_addr;
  char request_buffer[BUFFER_SIZE];
  request_buffer[BUFFER_SIZE - 1] = '\0'; // Ensure null-termination

  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd == -1)
  {

    return 1;
  }

  // Since the tester restarts your program quite often, setting SO_REUSEADDR
  // ensures that we don't run into 'Address already in use' errors
  int reuse = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) <
      0)
  {

    return 1;
  }

  struct sockaddr_in serv_addr = {
      .sin_family = AF_INET,
      .sin_port = htons(4221),
      .sin_addr = {htonl(INADDR_ANY)},
  };

  if (bind(server_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) != 0)
  {

    return 1;
  }

  int connection_backlog = 10;
  if (listen(server_fd, connection_backlog) != 0)
  {

    return 1;
  }

  client_addr_len = sizeof(client_addr);
  while (1)
  {
    int fd =
        accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);

    if (fd == -1)
      continue;

    pthread_t thread_id;

    pthread_create(&thread_id, NULL, handle_client, (void *)&fd);
    pthread_detach(thread_id);
  }
  close(server_fd);

  return 0;
}

void *handle_client(void *arg)
{

  int fd = *(int *)arg;

  char request_buffer[BUFFER_SIZE];
  request_buffer[BUFFER_SIZE - 1] = '\0'; // Ensure null-termination
  // Receive the HTTP request

  int bytes_received = recv(fd, request_buffer, sizeof(request_buffer) - 1, 0);
  if (bytes_received == -1)
  {
    perror("recv failed");
    // exit(1);
  }
  request_buffer[bytes_received] = '\0';
  char *request_string = request_buffer;
  http_request *request = parse_request(request_string);
  if (request == NULL)
  {
  }

  char *response;
  response = gen_response(request);
  if (response != NULL)
  {
    int bytes_sent = send(fd, response, strlen(response), 0);
  }
  else
  {
  }
  // server response

  free(request->method);
  free(request->path);
  free(request->version);
  // free(request->headers);
  free(request->body);
  free(request);
}

http_request *parse_request(char *request)
{
  char *headers = strchr(request, '\n') + 1;
  http_request *req = malloc(sizeof(http_request));
  if (request == NULL)
  {

    return (NULL); // Error handling: memory allocation failed
  }

  // parse method

  char *method = strtok(request, " ");
  if (method == NULL)
  {

    // Error handling: invalid request format
    free(req);
    return (NULL);
  }
  req->method = malloc(strlen(method) + 1);
  strcpy(req->method, method);

  // parse path

  char *path = strtok(NULL, " ");
  if (path == NULL)
  {

    // Error handling: invalid request format
    free(req->method);
    free(req);
    return (NULL);
  }
  req->path = malloc(strlen(path) + 1);
  strcpy(req->path, path);

  // parse version

  char *version = strtok(NULL, "\r\n");
  if (version == NULL)
  {

    // Error handling: invalid request format
    free(req->method);
    free(req->path);
    free(req);
    return (NULL);
  }
  req->version = malloc(strlen(version) + 1);
  strcpy(req->version, version);

  // char *headers = strtok(NULL, "\r\n\r\n");
  // printf("headers: %s\n", headers);
  // parse_headers(headers, req->headers);

  char *header = strtok(NULL, "\r");
  for (int i = 0; strcmp(header, "\n") != 0 && i < 10; i++)
  {
    header = header + 1;
    req->headers[i] = malloc(strlen(header) + 1);

    strcpy(req->headers[i], header);
    header = strtok(NULL, "\r");
  }

  char *body = strtok(NULL, "\r\n");
  if (body != NULL)
  {
    req->body = malloc(strlen(body) + 1);
    strcpy(req->body, body);
  }
  else
  {
    req->body = NULL;
  }

  return req;
}

char *gen_response(http_request *request)
{

  char *response = NULL;
  char *res = NULL;
  int gzip_encoding = 0;

  // Check for "Accept-Encoding: gzip" header
  for (int i = 0; i < 10 && request->headers[i] != NULL; i++)
  {
    if (strstr(request->headers[i], "Accept-Encoding: gzip") != NULL)
    {
      gzip_encoding = 1;
      break;
    }
  }
  if (strcmp(request->method, "GET") == 0)
  {
    if (strcmp(request->path, "/") == 0)
    {
      response = "HTTP/1.1 200 OK";
      if (gzip_encoding)
      {
        char *tmp = "Content-Type: text/plain";
        char *headers = malloc(strlen(tmp) + sizeof(unsigned long) + 1);
        char *encoding_header = "Content-Encoding: gzip\r\n";
        res = malloc(strlen(response) + strlen(headers) + strlen(encoding_header) + 1);
        sprintf(res, "%s\r\n%s%s\r\n\r\n", response, headers, encoding_header);
        free(headers);
      }
      else
      {
        res = "HTTP/1.1 200 OK\r\n\r\n";
      }
    }
    else
    {
      char *endpoint = strtok(request->path, "/");
      // char *endpoint = request->path;

      if (strcmp(endpoint, "user-agent") == 0)
      {
        char *str = NULL;
        for (int i = 0; i < 10; i++)
        {
          str = strtok(request->headers[i], ":");
          if (strcasecmp(str, "User-Agent") == 0)
          {
            break;
          }
        }
        // char *str = strtok(request->headers[1], ":");
        str = strtok(NULL, ": ");

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
        // if (str != NULL)
        //   free(str);
      }
      else if (strcmp(endpoint, "echo") == 0)
      {
        char *str = strtok(NULL, "/");
        if (str != NULL)
        {
          response = "HTTP/1.1 200 OK";

          char *tmp = "Content-Type: text/plain\r\nContent-Length:";
          char *headers = malloc(strlen(tmp) + sizeof(unsigned long) + 1);
          sprintf(headers, "%s %lu", tmp, strlen(str));
          if (gzip_encoding)
          {
            char *encoding_header = "Content-Encoding: gzip\r\n";
            res = malloc(strlen(response) + strlen(headers) + strlen(encoding_header) + strlen(str) + 5);
            sprintf(res, "%s\r\n%s\r\n%s\r\n\r\n%s", response, headers, encoding_header, str);
          }
          else
          {
            res = malloc(strlen(response) + strlen(headers) + strlen(str) + 5);
            sprintf(res, "%s\r\n%s\r\n\r\n%s", response, headers, str);
          }
          // res = malloc(strlen(response) + strlen(headers) + strlen(str) + 1);

          // sprintf(res, "%s\r\n%s\r\n\r\n%s", response, headers, str);
          // maybe use snprintf instead

          free(headers);
          // free(str);
        }
        else
        {
          res = "HTTP/1.1 400 Bad Request\r\n\r\n";
        }
      }
      else if (strcmp(endpoint, "files") == 0)
      {
        char *str = strtok(NULL, "/");
        if (str != NULL)
        {
          response = "HTTP/1.1 200 OK";

          char *file_path = malloc(strlen(directory) + strlen(str) + 2);

          sprintf(file_path, "%s/%s", directory, str);

          str = read_file(file_path);
          if (str != NULL)
          {
            free(file_path);

            char *tmp =
                "Content-Type: application/octet-stream\r\nContent-Length:";
            char *headers = malloc(strlen(tmp) + sizeof(unsigned long) + 1);
            sprintf(headers, "%s %lu", tmp, strlen(str));

            res = malloc(strlen(response) + strlen(headers) + strlen(str) + 1);

            sprintf(res, "%s\r\n%s\r\n\r\n%s", response, headers, str);
            // maybe use snprintf instead

            if (headers != NULL)
              free(headers);

            // free(str);
          }
          else
          {
            res = "HTTP/1.1 404 Not Found\r\n\r\n";
          }
        }
        else
        {
          res = "HTTP/1.1 400 Bad Request\r\n\r\n";
        }
      }
      else
      {
        res = "HTTP/1.1 404 Not Found\r\n\r\n";
      }
    }
  }
  else if (strcmp(request->method, "POST") == 0)
  {
    printf("POST method\n");
    char *endpoint = strtok(request->path, "/");
    if (strcmp(endpoint, "files") == 0)
    {
      char *str = strtok(NULL, "/");
      if (str != NULL)
      {
        response = "HTTP/1.1 201 Created\r\n\r\n";
        // filname to file path
        char *file_path = malloc(strlen(directory) + strlen(str) + 2);
        sprintf(file_path, "%s/%s", directory, str);

        /* File pointer to hold reference to our file */
        FILE *fPtr;
        /* Open file in w (write) mode. Specify the path and filename */
        fPtr = fopen(file_path, "w");
        /* Check if file opening was successful */
        if (fPtr == NULL)
        {
          printf("Unable to create file.\n");
          free(fPtr);
        }
        // fputs(request->body, fPtr);
        fprintf(fPtr, "%s", request->body);
        fclose(fPtr);
        printf("File created and saved successfully.\n");
        res = response;
      }
    }
  }
  return res;
}

char *read_file(char *file_path)
{
  FILE *fp;
  // char filename[] = "example.txt"; // replace with your file name

  fp = fopen(file_path, "r"); // open file in read-only mode
  if (fp == NULL)
  {
    // printf("Error opening file\n");
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

  // free(fileContent); // don't forget to free the memory!
  fclose(fp);
  return fileContent;
}
