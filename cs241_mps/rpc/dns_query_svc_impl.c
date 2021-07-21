/**
* Resplendent RPCs Lab
* CS 241 - Fall 2018
*/
//co-work with chennxie3

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <netdb.h>

#include "common.h"
#include "dns_query.h"
#include "dns_query_svc_impl.h"

#define CACHE_FILE "cache_files/rpc_server_cache"

char *contact_nameserver(query *argp, char *host, int port) {
    // Your code here
    // Look in the header file for a few more comments

  int s;

  struct addrinfo hints, *res;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET; // INET for IPv4
  hints.ai_socktype =  SOCK_DGRAM;
  hints.ai_flags =  AI_PASSIVE;

  //int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  //char buffer[10];
  //snprintf(buffer, MAX_BYTES_PORT, "%d", port);
  //fprintf(stderr, "port is %s\n", buffer);
  
  s = getaddrinfo(NULL, "9001", &hints, &res);
  int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  int optval = 1;
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
  if(s != 0){
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
    exit(1);
  }
  //fprintf(stderr,"start bind\n");
  if (bind(sockfd, res->ai_addr, res->ai_addrlen) != 0) {
    perror("bind()");
    exit(1);

  }

  //fprintf(stderr,"struct sock addr in\n");
  struct sockaddr_in sa;
  memset(&sa, 0, sizeof(sa));
  //sa.sin_family = AF_INET6;
  inet_pton(AF_INET, host, &(sa.sin_addr));
  sa.sin_port = htons(port);

  //fprintf(stderr,"sedn message\n");
  char*msg = malloc(strlen(argp -> host));
  strcpy(msg, argp -> host);
  //fprintf(stderr, "msg is %s\n", argp -> host);
  int s_ret = sendto(sockfd, msg, strlen(argp -> host), 0, (struct sockaddr *)&sa, sizeof(sa));
  if(s_ret < 0)
    {
      fprintf(stderr, "sendto failed\n");
    }
  int err_save_s = errno;
  while(s_ret == -1 && err_save_s == EAGAIN)
    {
      s_ret = sendto(sockfd, msg, strlen(argp ->host), 0, (struct sockaddr *)&sa, sizeof(sa));
      err_save_s = errno;
    }

  //fprintf(stderr, "receive ip_addr\n");

  struct sockaddr_storage addr;
  socklen_t addrlen = sizeof(addr);
  //memset(&addr, 0, sizeof(addr));
  //char* ret_ip = malloc(MAX_BYTES_IPV4);
  char ret_ip[MAX_BYTES_IPV4];
  //socklen_t addrlen = sizeof(sa);
  //fprintf(stderr, "receive\n");
  ssize_t byte_count = recvfrom(sockfd, ret_ip, 15/*sizeof(ret_ip) - 1*/, 0,(struct sockaddr *)&addr, &addrlen);
  int err_save = errno;
  //fprintf(stderr, "finish receive\n");
  
  while(byte_count == -1 && err_save == EAGAIN)
    {
      byte_count = recvfrom(sockfd, ret_ip, sizeof(ret_ip), 0, (struct sockaddr *)&sa, &addrlen);
      err_save = errno;
      
    }
  
  ret_ip[byte_count] = '\0';
  //free(msg);
  //free(ret_ip);
  if(strcmp(ret_ip, "-1.-1.-1.-1") == 0)
    {
      free(msg);
      //free(ret_ip);
      return NULL;
    }
  free(msg);
  //free(ret_ip);
  char* ret = malloc(16);
  ret[15] = '\0';
  strcpy(ret, ret_ip);
  return ret;
}

void create_response(query *argp, char *ipv4_address, response *res) {
    // Your code here
    // As before there are comments in the header file
  if(!ipv4_address)
    {
      res -> success = 0;
      res -> address = malloc(sizeof(host_address));
      res -> address -> host = malloc(strlen(argp -> host) + 1);
      res -> address -> host[strlen(argp -> host)] = 0;
      strcpy(res -> address -> host, argp -> host);
      res -> address -> host_ipv4_address = malloc(sizeof(char));
      res -> address -> host_ipv4_address[0] = '\0';
    }
  else
    {
      res -> success = 1;
      res -> address = malloc(sizeof(host_address));
      res -> address -> host = malloc(strlen(argp -> host) + 1);
      res -> address -> host[strlen(argp -> host)] = '\0';
      strcpy(res -> address -> host, argp -> host);
      //fprintf(stderr, "%s\n", res -> address -> host);
      res -> address -> host_ipv4_address = malloc(strlen(ipv4_address) + 1);
      res -> address -> host_ipv4_address[strlen(ipv4_address)] = '\0';
      strcpy(res -> address -> host_ipv4_address, ipv4_address);
    }
}

// Stub code

response *dns_query_1_svc(query *argp, struct svc_req *rqstp) {
    printf("Resolving query...\n");
    // check its cache, 'rpc_server_cache'
    // if it's in cache, return with response object with the ip address
    char *ipv4_address = check_cache_for_address(argp->host, CACHE_FILE);
    if (ipv4_address == NULL) {
        // not in the cache. contact authoritative servers like a recursive dns
        // server
        printf("Domain not found in server's cache. Contacting authoritative "
               "servers...\n");
        char *host = getenv("NAMESERVER_HOST");
        int port = strtol(getenv("NAMESERVER_PORT"), NULL, 10);
        ipv4_address = contact_nameserver(argp, host, port);
    } else {
        // it is in the server's cache; no need to ask the authoritative
        // servers.
        printf("Domain found in server's cache!\n");
    }

    static response res;
    xdr_free(xdr_response, &res); // Frees old memory in the response struct
    create_response(argp, ipv4_address, &res);

    free(ipv4_address);

    return &res;
}
