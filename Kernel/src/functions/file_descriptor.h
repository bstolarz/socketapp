/*
 * file_descriptor.h
 *
 *  Created on: 30/5/2017
 *      Author: utnso
 */

#ifndef FUNCTIONS_FILE_DESCRIPTOR_H_
#define FUNCTIONS_FILE_DESCRIPTOR_H_

t_fd* file_descriptor_get_by_path(t_program* program, char* path);
t_fd* file_descriptor_get_by_number(t_program* program, t_descriptor_archivo nFD);
int file_descriptor_check_permission(t_fd* fd, char* permission);

int get_cursor_of_file(t_cpu* cpu, char* path);
t_gobal_fd* existeArchivoEnTablaGlobalDeArchivos(t_list * l, char* path);
int delete_file_from_global_file_table(t_descriptor_archivo d, t_cpu* cpu);
int process_had_opened_file(t_cpu* cpu,t_descriptor_archivo d);
void update_cursor_of_file(t_cpu* cpu, t_descriptor_archivo f, int c);

#endif /* FUNCTIONS_FILE_DESCRIPTOR_H_ */
