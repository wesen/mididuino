#include <stdio.h>
#include <stdbool.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>

void setup_port(int fd) {
    struct termios options;
    fcntl(fd, F_SETFL, 0);
    tcgetattr(fd, &options);
    cfsetispeed(&options, B9600);
    cfsetospeed(&options, B9600);
    options.c_cflag |= (CLOCAL | CREAD);
    tcsetattr(fd, TCSANOW, &options);
}

int main(int argc, char *argv[]) {
    int port;
    if( argc != 2 ) { 
        printf("%s commport\n", argv[0]); 
        return -1;
    }
    port = open(argv[1], O_RDWR | O_NOCTTY | O_NONBLOCK);
    if(port < 0) {
        printf("Port could not be opened.\n");
        return -3;
    }
    setup_port(port);
    
    write(port, "hi", 2);
    while(1) {
        fd_set monitored_files;
        struct timeval timeout;
        
        FD_ZERO(     &monitored_files);
        FD_SET(port, &monitored_files);
        timeout.tv_sec = 0;
        timeout.tv_usec = 290;
        //printf("Waiting\n");
        if( select(port+1, &monitored_files, NULL, NULL, &timeout) <= 0 ) {
            continue;
        }
        printf("Got something 1!\n");
        if( FD_ISSET(port, &monitored_files) ) {
            unsigned char inch;
            printf("Performing read.\n");
            read(port, &inch, 1);
            printf("Got %02x\n", inch);
        }
    }
    close(port);
}
