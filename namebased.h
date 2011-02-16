/**
 * @file namebased.h
 * @brief Name-based API - experimental API extensions for name-based sockets.
 * @author Javier Ubillos
 */

#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <linux/net.h>
#include <linux/socket.h>


#ifdef DEBUG

#define DBG_P( ... ) (printf("%s:%d:%s() > " , __FILE__, __LINE__, __FUNCTION__), printf(__VA_ARGS__) )
#else

#define DBG_P //
#endif


#ifndef _NAMEBASED_H
#define _NAMEBASED_H

typedef struct _nameorientedcon {
	int fd;
	char* local_name;
	char* dest_name;
	u_int16_t port;
	int transport;
	char* service;
	struct sockaddr *saddr;
	struct sockaddr *daddr;
  
} nameorientedcon;

#define CONNECTION nameorientedcon

/**
 * Allocates a "nameorientedcon" struct, all zero:ed out.
 * @returns pointer to struct.
 */
#define ALLOC_SOCKET() (nameorientedcon*) calloc(1,sizeof(nameorientedcon)); 

/**
 * Defines a local name (sets names to 1 and removes all other names)
 */
#define SET_LOCAL_NAME(conn, name)					\
	conn->local_name = (char*) calloc(1, strlen( name ));		\
	strcpy(	conn->local_name, name)

#define GET_LOCAL_NAME(conn) conn->local_name

#define SET_DEST_NAME(conn, name)					\
	conn->dest_name = (char*) calloc(1, strlen( name ));		\
	strcpy(	conn->dest_name, name)

#define GET_DEST_NAME(conn) conn->dest_name

#define GET_ADDRESSES(conn) conn->saddr

#define SET_TRANSPORT(conn, tp) conn->transport = tp

#define GET_SERVICE(conn) conn->service
#define SET_SERVICE(conn, srv) namebased_set_service(conn, srv)
int namebased_set_service(nameorientedcon *conn, char* srv)
{								
	struct servent *ret;					
	char* tp = 0;						
	setservent(0);						
	if(SOCK_STREAM == conn->transport) tp = "tcp";		
	if(SOCK_DGRAM == conn->transport) tp = "udp";		
	ret = getservbyname( srv, tp );				
	endservent();						
	if(ret) {						
		conn->service = ret->s_name;			
		conn->port = ret->s_port;			
	} else {						
		conn->service = 0;				
		conn->port = 0;					
	}							
}								

#define CONNECT(conn) namebased_connect(conn)
int namebased_connect(nameorientedcon *conn) 
{
	if( 0 < namestack_module_loaded() )
		return namebased_connect_af_name(conn);
	else 
		return namebased_connect_legacy(conn);
}

int namebased_connect_legacy(nameorientedcon *conn) {
	char inetbuf[INET6_ADDRSTRLEN];
	struct addrinfo *ainf, hints;
	int fd=0, ret=0;
	
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	if( conn->transport )
		hints.ai_socktype = conn->transport;
	
	memset(&inetbuf, 0, sizeof(inetbuf));
	getaddrinfo( conn->dest_name, conn->service, &hints,  &ainf );
	
	while( ainf ) {
		
		DBG_P("socket( %d, %d, %d);\n", ainf->ai_family, conn->transport, 0);
		fd = socket( ainf->ai_family, conn->transport, 0);
		if(0 <= fd) {
			DBG_P("socket returned fd: %d\n", fd);
			inet_ntop(AF_INET, ainf->ai_addr->sa_data +2, inetbuf);
			
			ret = connect(fd, ainf->ai_addr, ainf->ai_addrlen);
			DBG_P("connect returned: %d\n", ret);	
			if(0 <= ret) {
				conn->daddr = ainf->ai_addr;
				conn->fd = fd;
				break;
			}
		}

		ainf = ainf->ai_next;
		DBG_P("Next ainf\n");
	}
}

int namebased_connect_af_name() {
	perror("AF_NAME not yet implemented");
}


#define SEND(conn, data, size) write(conn->fd, data, size)
	
#define CLOSE(conn) close(conn->fd); conn->fd = 0;

#define NAMESTACK_MODULE_NAME "namestack"

int namestack_module_loaded(void) {
	
	int i;
	FILE* f;
	char line[256], mod_name[256];
	
	if ((f = fopen("/proc/modules", "r")) >= 0 ) {
		while (( fgets(line, 256, f)) > 0) {
			sscanf(line, "%s", mod_name );
			if ( 0 == strcmp(NAMESTACK_MODULE_NAME, mod_name) )
				return 1;
		}
		fclose(f);
		return 0;
	}
	return -1; // Could not open /proc/modules
}
#endif
