#include <Rinternals.h>
#include <string.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
const char *inet_ntop(int af, const void *src, char *dst, socklen_t size);
#else
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#ifdef __FreeBSD__
#include <netinet/in.h>
#endif
#endif

SEXP R_nslookup(SEXP hostname) {
  /* Because gethostbyname() is deprecated */
  struct addrinfo *addr;
  if(getaddrinfo(CHAR(STRING_ELT(hostname, 0)), NULL, NULL, &addr))
    return R_NilValue;

  /* For debugging */
  struct sockaddr *sa = addr->ai_addr;

  /* IPv4 vs v6 */
  char ip[INET6_ADDRSTRLEN];
  if (sa->sa_family == AF_INET) {
    struct sockaddr_in *sa_in = (struct sockaddr_in*) sa;
    inet_ntop(AF_INET, &(sa_in->sin_addr), ip, INET_ADDRSTRLEN);
  } else {
    struct sockaddr_in6 *sa_in = (struct sockaddr_in6*) sa;
    inet_ntop(AF_INET6, &(sa_in->sin6_addr), ip, INET6_ADDRSTRLEN);
  }
  freeaddrinfo(addr);
  return mkString(ip);
}

/* Fallback implementation for inet_ntop in Win32 */

#if defined(_WIN32) && !defined(_WIN64)
const char *inet_ntop(int af, const void *src, char *dst, socklen_t size)
{
  struct sockaddr_storage ss;
  unsigned long s = size;

  ZeroMemory(&ss, sizeof(ss));
  ss.ss_family = af;

  switch(af) {
  case AF_INET:
    ((struct sockaddr_in *)&ss)->sin_addr = *(struct in_addr *)src;
    break;
  case AF_INET6:
    ((struct sockaddr_in6 *)&ss)->sin6_addr = *(struct in6_addr *)src;
    break;
  default:
    return NULL;
  }
  /* cannot direclty use &size because of strict aliasing rules */
  return (WSAAddressToString((struct sockaddr *)&ss, sizeof(ss), NULL, dst, &s) == 0)?
  dst : NULL;
}
#endif
