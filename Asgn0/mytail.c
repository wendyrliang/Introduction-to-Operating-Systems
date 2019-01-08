/*
 * Ruiwen Liang
 * rliang4
 * CS111-asgn0
 * mytail.c
 */

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2
#define BUFFER_SIZE 4096

int main(int argc, char** argv) {

	// If file is not provided
	if (argc < 2){
        perror("Error: ");
        return(1);
    }

    // Process each file in the command line
	for (int i = 1; i < argc; i++) {
		char* filename = argv[i];
        char buffer[BUFFER_SIZE] = {0};
		//char* buffer = (char*)calloc(BUFFER_SIZE, sizeof(char));
		int lines = 0;

		// Open file, stop and print error message if file cannot be opened
		int fp = open(filename, O_RDONLY);
		if (fp < 0) {
			perror("Error: ");
			return(1);
		}

		// Read file
        int sz;
        char last;
		if ((sz = read(fp, &buffer, BUFFER_SIZE)) != 0) {
			// If error in reading file
			if (sz < 0) {
				perror("Error: ");
				return(1);
			}

            // Count total line number
            for (int c = 0; c < sz; c++){
                if (buffer[c] == '\n') lines++;
                last = buffer[c];
            }
        }
        // Count last line when it does not end with newline character
        if (last != '\n') lines++;

        // Print the whole file when it has less or equal to 10 lines
        if (lines <= 10) {
            write(STDOUT, &buffer, BUFFER_SIZE);
        } else {
            int x = lines/10;
            int r = lines%10;
            int l = (x-1)*10+r;
            int j, counter = 0;
            char temp[BUFFER_SIZE] = {0};
            for(j = 0; counter < l; j++){
                if(buffer[j] == '\n') counter++;
            }
            strncpy(temp, buffer+j, sz);
            write(STDOUT, &temp, BUFFER_SIZE);
        }

		// Close file, check for errors
		if (close(fp) < 0) {
			perror("Error: ");
			return(1);
		} 
	}
	return(0);
}