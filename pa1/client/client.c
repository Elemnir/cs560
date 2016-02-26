#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>

#define MAXDATASIZE 100

int main(int argc, char **argv)
{
  struct addrinfo hints, *info, *p;
  int retval, portnum, sock, received;
  char ip4[INET_ADDRSTRLEN];
  char *msg = (char *) malloc(sizeof(char) * 1000);
  fd_set readfds, masterfds;

  /* Handle arguments */
  if(argc != 2) {
    fprintf(stderr, "USAGE: ./fsclient PORTNUM\n");
    exit(1);
  }

  /* Make sure the portnum is valid */
  portnum = strtol(argv[1], NULL, 10);
  if((portnum <= 0) || (portnum > 65535)) {
    fprintf(stderr, "ERROR: Your port number is invalid.\n");
    exit(1);
  }
  
  /* Fill in the information that we do know */
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;     /* IPv4 or IPv6 */
  hints.ai_socktype = SOCK_STREAM; /* TCP */

  /* Get information about the host (DNS, etc) */
  retval = getaddrinfo("localhost",
                       argv[1],
                       &hints,
                       &info);
  if(retval != 0) {
    fprintf(stderr, "ERROR: %s\n", gai_strerror(retval));
    exit(1);
  }
  
  /* Find the addrinfo that we can connect to */
  for(p = info; p != NULL; p = p->ai_next) {

    inet_ntop(AF_INET, &(((struct sockaddr_in *)(p->ai_addr))->sin_addr),
              ip4, INET_ADDRSTRLEN);
    /* printf("Trying to connect to: %s\n", ip4); */

    /* Get a socket */
    sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if(sock == -1) {
      perror("");
      continue;
    }


    /* Connect to the port */
    if(connect(sock, p->ai_addr, p->ai_addrlen) == -1) {
      close(sock);
      perror("");
      continue;
    }

    /* Success, so continue */
    break;
  }
  
  if(p == NULL) {
    fprintf(stderr, "ERROR: Failed to connect.\n");
    exit(1);
  }

  /* Set the file descriptors */
  FD_ZERO(&masterfds);
  FD_SET(0, &masterfds);
  FD_SET(sock, &masterfds);

  while(1) {
    memcpy(&readfds, &masterfds, sizeof(fd_set));
    
    if(select(sock + 1, &readfds, NULL, NULL, NULL) < 0) {
      perror("Select failed");
      exit(1);
    }

    if(FD_ISSET(0, &readfds)) {
      received = read(0, msg, 999);
      if(received == -1) {
        perror("");
        close(0);
        close(sock);
        exit(1);
      } else {
        write(sock, msg, received);
      }
    }
    if(FD_ISSET(sock, &readfds)) {
      received = recv(sock, msg, 999, 0);
      if(received == -1) {
        close(0);
        close(sock);
        exit(1);
      } else {
        write(1, msg, received);
      }
    }
  }

  /* Clean up */
  free(msg);
  close(sock);
  freeaddrinfo(info);
}
