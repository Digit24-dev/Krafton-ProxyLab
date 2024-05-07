/*
 * adder.c - a minimal CGI program that adds two numbers together
 */
/* $begin adder */
#include "csapp.h"

int main(void) {
  char *buf, *p, *method;
  char arg1[MAXLINE], arg2[MAXLINE], content[MAXLINE];
  int n1 = 0, n2 = 0;
  
  if ((buf = getenv("QUERY_STRING")) != NULL) {
    p = strchr(buf, '&');
    *p = '\0';
    strcpy(arg1,   buf);
    strcpy(arg2, p + 1);

    char *a1 = strchr(arg1, '=');
    char *a2 = strchr(arg2, '=');

    n1 = atoi(a1 + 1);
    n2 = atoi(a2 + 1);
  }

  method = getenv("REQUEST_METHOD");

  sprintf(content, "QUERY_STRING=%s", buf);
  sprintf(content, "Welcome to add.com: ");
  sprintf(content, "%sThe Internet addition portal.\r\n<p>", content);
  sprintf(content, "%sThe answer is: %d + %d = %d\r\n<p>", content, n1, n2, n1 + n2);
  sprintf(content, "%sThanks for visiting!\r\n", content);

  printf("Connection: close\r\n");
  printf("Content-length: %d\r\n", (int)strlen(content));
  printf("Content-type: text/html\r\n\r\n");

  if (strcasecmp(method, "GET") == 0)
    printf("%s", content);

  fflush(stdout);
  
  exit(0);
}
/* $end adder */
