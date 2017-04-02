#include <stdio.h>
#include <stdlib.h>
#include "sockets/server.h"

int main(void) {
	char* port = "6667";
	socket_server_select(port);
}
