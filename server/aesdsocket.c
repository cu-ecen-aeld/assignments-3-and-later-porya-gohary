#define _POSIX_C_SOURCE 200809L
#include <sys/types.h>
#include <sys/socket.h>
#include <syslog.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <poll.h>


#define PORT "9000"
#define CONNECTION_QUEUE_SIZE 10
#define BUFFER_SIZE 256

#define WRITE_TEMP_FILE "/var/tmp/aesdsocketdata"


int sock_fd = -1;
int connection_fd = -1;
int file_fd = -1;
struct addrinfo* addr_list;
struct addrinfo hints;


static void signal_handler(int sig) {

    if (sock_fd != -1) close(sock_fd);
    if (connection_fd != -1) close(connection_fd);
    if (file_fd != -1) close(file_fd);
    
    // remove the temporary file
    remove(WRITE_TEMP_FILE);

    syslog(LOG_INFO, "Caught signal, exiting now");
    closelog();
    exit(0);
}

int main(int argc, char** argv) {
    
    openlog("aesdsocket", 0, LOG_USER);
    struct sigaction sa;
    memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_handler = signal_handler;

    if (sigaction(SIGINT | SIGTERM, &sa, NULL) != 0) {
        perror("sigaction");
        syslog(LOG_ERR, "Failed to set signal handler");
        closelog();
        return -1;
    }

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;


    getaddrinfo(NULL, PORT, &hints, &addr_list);


    sock_fd = socket(addr_list->ai_family, addr_list->ai_socktype, addr_list->ai_protocol);
    if (sock_fd == -1) {
        perror("Error creating socket");
        syslog(LOG_ERR, "Failed to open socket");
        closelog();
        return -1;
    }


    if(setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) == -1) {
        perror("Error on setsockopt");
        syslog(LOG_ERR, "Failed to set socket options");
        closelog();
        close(sock_fd);
        return -1;
    }


    if(bind(sock_fd, addr_list->ai_addr, addr_list->ai_addrlen) == -1) {
        perror("Error on bind");
        syslog(LOG_ERR, "Failed to bind socket");
        closelog();
        close(sock_fd);
        return -1;
    }

    if(listen(sock_fd, CONNECTION_QUEUE_SIZE) == -1) {
        perror("Error on listen");
        syslog(LOG_ERR, "Failed to listen on socket");
        closelog();
        close(sock_fd);
        return -1;
    }


    freeaddrinfo(addr_list);


    printf("Waiting for connection on port %s\n", PORT);

    // if user passes -d as argument, daemonize the process
    if(argc > 1 && strcmp(argv[1], "-d") == 0) {
        printf("Daemonizing the process\n");
        
        pid_t pid = fork();

        if (pid < 0) {
            perror("Error on fork");
            syslog(LOG_ERR, "Failed to fork");
            closelog();
            close(sock_fd);
            return -1;
        }

        if(pid != 0) {
            closelog();
            close(sock_fd);
            return 0;
        }


        if(setsid() == -1) {
            perror("Error on setsid");
            syslog(LOG_ERR, "Failed to create new session");
            closelog();
            close(sock_fd);
            return -1;
        }


        if(chdir("/") == -1) {
            perror("Error on chdir");
            syslog(LOG_ERR, "Failed to change directory");
            closelog();
            close(sock_fd);
            return -1;
        }


        int null_fd = open("/dev/null", O_RDWR);
        if(null_fd == -1) {
            perror("Error on open");
            syslog(LOG_ERR, "Failed to open /dev/null");
            closelog();
            close(sock_fd);
            return -1;
        }

        dup2(null_fd, STDIN_FILENO);
        dup2(null_fd, STDOUT_FILENO);
        dup2(null_fd, STDERR_FILENO);

        
    }


    // open the file to write the data
    file_fd = open(WRITE_TEMP_FILE, O_RDWR | O_CREAT | O_APPEND | O_SYNC, S_IRWXU | S_IRGRP | S_IROTH);
    if(file_fd == -1) {
        perror("Error on open");
        syslog(LOG_ERR, "Failed to open file");
        closelog();
        close(sock_fd);
        return -1;
    }


    struct sockaddr_storage client_addr;
    socklen_t client_addr_len = sizeof(struct sockaddr_storage);


    // receiving the data
    while (1) {
        connection_fd = accept(sock_fd, (struct sockaddr*)&client_addr, &client_addr_len);
        if (connection_fd == -1) {
            perror("Error on accept");
            syslog(LOG_ERR, "Failed to accept connection");
            closelog();
            close(sock_fd);
            close(file_fd);
            return -1;
        }


        char addr[INET6_ADDRSTRLEN];

        inet_ntop(client_addr.ss_family, 
            &((struct sockaddr_in*)&client_addr)->sin_addr, 
            addr, sizeof(addr));

        printf("Connection from %s\n", addr);
        syslog(LOG_INFO, "Connection from %s", addr);


        char buffer[BUFFER_SIZE];
        int total_bytes_read;
        int bytes_per_read;

        while ((bytes_per_read = recv(connection_fd, buffer, BUFFER_SIZE, 0)) != 0) {
            total_bytes_read += bytes_per_read;
            write(file_fd, buffer, bytes_per_read);

            // check if we reached the end of the message
            if (buffer[bytes_per_read - 1] == '\n') {
                break;
            }
        }


        // seek to the beginning of the file
        lseek(file_fd, 0, SEEK_SET);


        // read the data from the file and send it back to the client
        while ((bytes_per_read = read(file_fd, buffer, BUFFER_SIZE)) != 0) {
            send(connection_fd, buffer, bytes_per_read, 0);
        }

        
        

        close(connection_fd);
        syslog(LOG_INFO, "Connection from %s closed", addr);
        

    }

    closelog();
    close(sock_fd);
    close(file_fd);
    return 0;

}