#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>
#include <commons/config.h>
#include <commons/string.h>
#include "commons/structures.h"
#include "commons/declarations.h"
#include <commons/bitarray.h>
#include <commons/log.h>
#include "functions/principales.h"
#include "functions/config.h"

int main(void) {

	remove("logGeneradorArchivos");
	logs = log_create("logGeneradorArchivos", "GeneradorDeArchivos", 0, LOG_LEVEL_TRACE);

	configMetadata = malloc(sizeof(t_metadata));
	metadataFS_read("../../FileSystem/Debug/mnt/SADICA_FS/Metadata/Metadata.bin");

	remove("../../FileSystem/Debug/mnt/SADICA_FS/Metadata/Bitmap.bin");
	crearBitmapVacio();

	truncarBloquesBitmap();

	free(configMetadata);
	return EXIT_SUCCESS;
}
