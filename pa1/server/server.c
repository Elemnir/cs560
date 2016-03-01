#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>

/* Execute the shell and dup its stdout */
void exec_shell(int client_fd)
{
  char cmd[] = "fakefs";
  size_t size = 7;
  char *argv[2];

  argv[0] = (char *) malloc(size);
  strncpy(argv[0], cmd, size);
  argv[1] = NULL;
  
  dup2(client_fd, 0);
  dup2(client_fd, 1);
  dup2(client_fd, 2);
  
  execvp(cmd, argv);

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

  /* The socket that we're binding to */
  int sock;

  /* Maximum numbers of connections in the queue */
  int backlog = 20;

  /* Information about the client */
  struct sockaddr_storage client_addr;
  socklen_t client_addr_size;
  int client_fd;
	char client_addrstr[INET6_ADDRSTRLEN];

  /* To wait for the forked process */
  int status;

  /* Handle arguments */
  if(argc != 2) {
    fprintf(stderr, "USAGE: ./server PORTNUM\n");
    exit(1);
  }

  /* Fill in the information that we do know */
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;     /* IPv4 or IPv6 */
  hints.ai_socktype = SOCK_STREAM; /* TCP */
  hints.ai_flags = AI_PASSIVE;

  /* Get information about localhost's sockets */
  if(getaddrinfo(NULL, argv[1], &hints, &info) != 0) {
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
    exit(1);
  }

  client_addr_size = sizeof(client_addr);

  /* Accept a connection */
  client_fd = accept(sock, (struct sockaddr *)&client_addr, &client_addr_size);
  if(client_fd == -1) {
    perror("");
    exit(1);
  }
  
  /* Print the connecting IP address */
  inet_ntop(client_addr.ss_family, get_in_addr((struct sockaddr *) &client_addr),
            client_addrstr, sizeof(client_addrstr));

  /* Fork the shell */
  if(!fork()) {
    exec_shell(client_fd);
  }

  /* Wait for the shell to exit */
  wait(&status);
  
  /* Clean up */
  close(sock);
  freeaddrinfo(info);
}
