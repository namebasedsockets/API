/**
 * @file namebased.h
 * @brief Name-based API - experimental API extensions for name-based sockets.
 * @author Javier Ubillos
 */

#include <stdio.h>
#include "namebased.h"

void usage(char *arg){
	printf("%s <src name> <dst name>\n", arg);
}

int main (int argc, char* argv[]) {
	int i;

	if(argc != 3) {
		usage(argv[0]);
		return -1;
	}

	CONNECTION *my_conn = ALLOC_SOCKET();
	DBG_P("Socket allocated\n");

	SET_LOCAL_NAME(my_conn, argv[1]);
	SET_DEST_NAME(my_conn, argv[2]);

	printf("Dest name: %s\n", GET_DEST_NAME(my_conn));

	SET_TRANSPORT(my_conn, SOCK_STREAM);
	
	SET_SERVICE(my_conn, "http");

	if( 0 > CONNECT( my_conn )  )
		return -1;

	char* msg = "Hello remote node!";
	SEND( my_conn, msg, strlen(msg) );

	CLOSE( my_conn );
}
