#include <stdio.h>
#include "csapp.h"

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400

/* You won't lose style points for including this long line in your code */
static const char *user_agent_hdr =
    "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";

void read_response(rio_t *rp, char *content_length, char *res_header);
void do_proxy(int fd);
void read_requesthdrs(int fd, rio_t *rp, char *header, char *host);
void serve_static(int fd, char *filename, int filesize, char *method);
void get_filetype(char *filename, char *filetype);
// void serve_dynamic(int fd, char *filename, char *cgiargs, char *method);
void clienterror(int fd, char *cause, char *errnum, char *shortmsg,
                 char *longmsg);

int main(int argc, char **argv) {
  int listenfd, connfd;
  char hostname[MAXLINE], port[MAXLINE];
  socklen_t clientlen;
  struct sockaddr_storage clientaddr;

  /* Check command line args */
  if (argc != 2) {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(1);
  }

  listenfd = Open_listenfd(argv[1]);
  while (1) {
    clientlen = sizeof(clientaddr);
    connfd = Accept(listenfd, (SA *)&clientaddr,
                    &clientlen);  // line:netp:tiny:accept
    Getnameinfo((SA *)&clientaddr, clientlen, hostname, MAXLINE, port, MAXLINE,
                0);
    printf("Accepted connection from (%s, %s)\n", hostname, port);
    do_proxy(connfd);   // line:netp:tiny:doit
    Close(connfd);  // line:netp:tiny:close
  }
  
  return 0;
}

void do_proxy(int fd)
{
  char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE], header[MAXLINE];
  char filename[MAXLINE], host[MAXLINE], *port = NULL;

  rio_t rio, rio_client;

  Rio_readinitb(&rio, fd);

  // Request Header
  Rio_readlineb(&rio, buf, MAXLINE);
  printf("Request headers:\n");
  printf("%s", buf);
  sscanf(buf, "%s %s %s", method, uri, version);

  if (!strcmp(version, "HTTP/1.1"))
    strcpy(version, "HTTP/1.0");

  read_requesthdrs(fd, &rio, header, host);

  // Port 
  {
    char *p = index(host, ':');
    if (p == NULL) {
      strcpy(port, "80");
    } else {
      *p = '\0';
      port = p + 1;
    }
  }

  int clientfd = open_clientfd(host, port);
  if (clientfd < 0) {   // ERROR
    // ERROR MSG
    clienterror(fd, filename, "404", "Not found", "Tiny couldn't find this file");
    return;
  }
  
  // Connection Established
  char str_conn[]  = "Connection: close\r\n";
  char str_proxyconn[] = "Proxy-Connection: close\r\n";
  char contents_length[MAXLINE];

  // Method
  sprintf(buf, "%s / %s\r\n", method, version);

  // Headers
  strcat(buf, user_agent_hdr);
  strcat(buf, header);
  strcat(buf, str_conn);
  strcat(buf, str_proxyconn);
  strcat(buf, "\r\n");

  // Write Order to the Server
  Rio_writen(clientfd, buf, strlen(buf));
  
  // Get Response from server
  char response_header[MAXLINE];
  Rio_readinitb(&rio_client, clientfd);
  
  // Response Header
  printf("\nResponse headers:\n");
  read_response(&rio_client, contents_length, response_header);
  printf("%s", response_header);

  size_t i_contents_length = (size_t)atol(contents_length);

  // Get Response Body from server
  void *srcp = Malloc(i_contents_length);
  Rio_readnb(&rio_client, srcp, i_contents_length);
  Rio_writen(fd, response_header, strlen(response_header));
  Rio_writen(fd, srcp, i_contents_length);
  Free(srcp);
}

void clienterror(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg)
{
  char buf[MAXLINE], body[MAXLINE];

  sprintf(body, "<html><title>Tiny Error</title>");
  sprintf(body, "%s<body bgcolor=""ffffff"">\r\n", body);
  sprintf(body, "%s%s: %s\r\n", body, errnum, shortmsg);
  sprintf(body, "%s<p>%s: %s\r\n", body, longmsg, cause);
  sprintf(body, "%s<hr><em>The Tiny Web server</em>\r\n", body);

  sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "Content-type: text/html\r\n");
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "Content-length: %d\r\n\r\n", (int)strlen(body));
  Rio_writen(fd, buf, strlen(buf));
  Rio_writen(fd, body, strlen(body));
}

void read_response(rio_t *rp, char *content_length, char *res_header)
{
  char buf[MAXLINE];
  char *p;
  buf[0] = '\0';
  *res_header = '\0';

  do {
    Rio_readlineb(rp, buf, MAXLINE);
    buf[MAXLINE - 1] = '\0';
    strcat(res_header, buf);
    printf("%s", buf);
    if ((p = strstr(buf, "Content-length:")) != NULL) {
      strcpy(content_length, p + 16);
    }
  } while (strcmp(buf, "\r\n"));

  return;
}

void read_requesthdrs(int fd, rio_t *rp, char *header, char *host)
{
  char buf[MAXLINE];
  char *p;

  do {
    Rio_readlineb(rp, buf, MAXLINE);

    if ((p = strstr(buf, "Host:")) != NULL) {
      strcpy(host, p + 6);
      char *temp = strchr(host, '\r');
      *temp = '\0';
    }
    if (strstr(buf, "Connection:"))
      continue;

    if (strcmp(buf, "\r\n"))
      strcat(header, buf);

  } while (strcmp(buf, "\r\n"));

  return;
}
