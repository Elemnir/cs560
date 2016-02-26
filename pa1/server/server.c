#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>

/* Execute the shell and dup its stdout */
void exec_shell(int client_fd)
{
  char *argv[2];

  argv[0] = "fakefs";
  argv[1] = NULL;
  
  dup2(client_fd, 0);
  dup2(client_fd, 1);
  dup2(client_fd, 2);
  
  printf("Running filesystem shell...\n");
  execvp("fakefs", argv);
  perror("Exec failed.\n");
  exit(1);
}

/* Get an IPv4 or IPv6 address */
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in *)sa)->sin_addr);
	} else {
		return &(((struct sockaddr_in6 *)sa)->sin6_addr);
	}
}


int main(int argc, char **argv)
{
  /* Used to get information about the socket */
  struct addrinfo hints, *info, *p;
  char ip[INET6_ADDRSTRLEN];

  /* The port number and the socket that we're binding to */
  int portnum, sock;

  /* Maximum numbers of connections in the queue */
  int backlog = 20;

  /* Information about the client */
  struct sockaddr_storage client_addr;
  socklen_t client_addr_size;
  int client_fd;
	char client_addrstr[INET6_ADDRSTRLEN];

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

  /* Get information about localhost's sockets */
  if(getaddrinfo("localhost", argv[1], &hints, &info) != 0) {
    exit(1);
  }
  
  /* Find the addrinfo that we can connect to */
  for(p = info; p != NULL; p = p->ai_next) {

    inet_ntop(AF_INET, &(((struct sockaddr_in *)(p->ai_addr))->sin_addr),
              ip, INET_ADDRSTRLEN);

    /* Get a socket */
    sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if(sock == -1) {
      perror("");
      continue;
    }

    /* Connect to the port */
    if(bind(sock, p->ai_addr, p->ai_addrlen) == -1) {
      close(sock);
      perror("");
      continue;
    }

    break;
  }
  
  /* Print if successful; die otherwise */
  if(p == NULL) {
    fprintf(stderr, "ERROR: Failed to bind.\n");
    exit(1);
  }
  
  /* Start listening for clients */
  if(listen(sock, backlog) == -1) {
    perror("");
    exit(1);
  }

  client_addr_size = sizeof(client_addr);
  while(1) {
    /* Accept a connection */
    client_fd = accept(sock, (struct sockaddr *)&client_addr, &client_addr_size);
    if(client_fd == -1) {
      perror("");
      continue;
    }
    
    /* Print the connecting IP address */
    inet_ntop(client_addr.ss_family, get_in_addr((struct sockaddr *) &client_addr),
							client_addrstr, sizeof(client_addrstr));

    /* Fork the shell */
    if(!fork()) {
      exec_shell(client_fd);
      close(sock);
    }
  }
  
  /* Clean up */
  close(sock);
  freeaddrinfo(info);
}
