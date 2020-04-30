/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.c
 * Author: cakers
 *
 * Created on 3 March 2020, 12:12
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <getopt.h>
#include "../../cakelog/cakelog.h"

#ifndef BUF_SIZE
#define BUF_SIZE 1024
#endif

#define true 1
#define false 0

int main(int argc, char** argv) {
	
	if (cakelog_initialise("tlpi-tee") != 0) {
		printf("main() : error initialising logging. Fast fail.\n");
		exit(EXIT_FAILURE);
	}

	void* buf[BUF_SIZE];
	int file_append = false;

	// Parse options
	cakelog("Parsing options");
	
	int opt;
	while ((opt = getopt(argc, argv, "a")) != -1) {
		if ((unsigned char)opt == 'a') {
			cakelog("Option [a] found (APPEND)");
			file_append = true;
		}
		else {
			cakelog("Invalid option found, printing usage message");
			printf("Usage: caketee [-a] <outfile>\n");
			exit(EXIT_FAILURE);
		}
	}

	if (optind >= argc) {
		printf("Missing filename (Usage: caketee [-a] <outfile>)\n");
		exit(EXIT_FAILURE);
	}

	const char* out_file = argv[optind];
	cakelog("Outfile to open is %s", out_file);

	int file_open_flags = (O_CREAT | O_WRONLY);
	if (file_append == true){
		cakelog("append = TRUE, so adding 0_APPEND to file_open_flags");
		file_open_flags = (file_open_flags | O_APPEND);
	}
	else {
		cakelog("append = FALSE, so adding o_EXCL to file_open_flags");
		file_open_flags = (file_open_flags | O_EXCL);
	}

	int outfile_fd = open(out_file, file_open_flags, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	cakelog("Opened outfile [%s] successfully with file descriptor [%ld]",out_file, outfile_fd);

	if (outfile_fd == -1) {
		perror("caketee (outfile)");
		exit(EXIT_FAILURE);
	}

	ssize_t bytes_read;
	ssize_t bytes_written_stdout;
	ssize_t bytes_written_outfile;
	while ((bytes_read = read(STDIN_FILENO, buf, BUF_SIZE+1)) > 0) {
		cakelog("Read [%ld] bytes from STDIN", bytes_read);
		bytes_written_stdout = write(STDOUT_FILENO, buf, bytes_read);
		cakelog("Written [%ld] bytes to STDOUT", bytes_written_stdout);
		bytes_written_outfile = write(outfile_fd, buf, bytes_read);
		cakelog("Written [%ld] bytes to outfile", bytes_written_outfile);
	}

	close(outfile_fd);
	cakelog_stop();
	return (EXIT_SUCCESS);
}

