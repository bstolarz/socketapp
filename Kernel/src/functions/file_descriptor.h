/*
 * file_descriptor.h
 *
 *  Created on: 30/5/2017
 *      Author: utnso
 */

#ifndef FUNCTIONS_FILE_DESCRIPTOR_H_
#define FUNCTIONS_FILE_DESCRIPTOR_H_

void get_filename_with_filedescriptor(t_cpu* cpu, t_descriptor_archivo _fd, char* path);
int get_cursor_of_file(t_cpu* cpu, char* path);
t_gobal_fd* existeArchivoEnTablaGlobalDeArchivos(t_list * l, char* path);
int delete_file_from_global_file_table(t_descriptor_archivo d, t_cpu* cpu);
int program_has_permission_to_write(t_cpu* cpu,t_descriptor_archivo d);
int program_has_permission_to_delete(t_cpu* cpu,t_descriptor_archivo d);
int process_had_opened_file(t_cpu* cpu,t_descriptor_archivo d);
void update_cursor_of_file(t_cpu* cpu, t_descriptor_archivo f, int c);
int get_permission_on_file(t_descriptor_archivo d, t_cpu* cpu, char* path);

#endif /* FUNCTIONS_FILE_DESCRIPTOR_H_ */
