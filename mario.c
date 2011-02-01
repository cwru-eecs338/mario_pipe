#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

const int  READ_END = 0;
const int WRITE_END = 1;

void child(int);
void parent(int);

int main() {

    // Holds file descriptors
    // for read and write
    // ends of the pipe
    int pipefd[2];

    // Create the pipe
    if (pipe(pipefd)) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Fork a child
    pid_t childpid = fork();
    if (childpid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);

    } else if (childpid == 0) {
        // Child closes write
        // end and uses read end
        close(pipefd[WRITE_END]);
        child(pipefd[READ_END]);

    } else {
        // Parent closes read
        // end and uses write end
        close(pipefd[READ_END]);
        parent(pipefd[WRITE_END]);
    }

    // Wait for child
    wait(NULL);
    exit(EXIT_SUCCESS);
}

void parent(int pipe_write_fd) {
    char* mario = "MARIO";
    printf("Parent: \"%s\" enters the pipe...\n", mario);
    // Write to the pipe
    write(pipe_write_fd, mario, strlen(mario));
    // Close the pipe
    // (Sends 'end of file' to reader)
    close(pipe_write_fd);
}

void child(int pipe_read_fd) {
    // Allocate buffer to store
    // result of read
    int buffer_size = 1024;
    char buffer[buffer_size];

    // Keep reading until we exhaust
    // buffer or reach end of file
    int i = 0;
    while (i < buffer_size
           && read(pipe_read_fd, &buffer[i], 1) > 0)
    { i++; }

    if (i < buffer_size) {
        // Add null termination
        buffer[i] = '\0';
    } else {
        // We exhausted buffer
        fprintf(stderr, "Warning: buffer full.\n");
        buffer[buffer_size-1] = '\0';
    }

    printf("Child:      ...and \"%s\" exits the pipe.\n", buffer);

    // Close the pipe
    close(pipe_read_fd);
}
