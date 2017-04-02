#include <stdio.h>
#include <stdlib.h>
#include "libSockets/server.h"
#include "select.h"

int main(void) {
	char* port = "6667";
	socket_server_select(port,1024, *socket_select_connection_lost, *socket_select_recive_package);

	return 0;
}
