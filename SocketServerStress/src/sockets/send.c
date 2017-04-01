#include <sys/types.h>
 #include <sys/socket.h>

int socket_send_all(int s, char *buf, int *len){
	int total = 0; // cuántos bytes hemos enviado
	int bytesleft = *len; // cuántos se han quedado pendientes
	int n;

	while(total < *len) {
		n = send(s, buf+total, bytesleft, 0);
		if (n == -1) { break; }
		total += n;
		bytesleft -= n;
	}

	*len = total; // devuelve aquí la cantidad enviada en realidad
	return n==-1?-1:0; // devuelve -1 si hay fallo, 0 en otro caso
}
