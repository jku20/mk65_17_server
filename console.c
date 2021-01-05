#include <unistd.h>
#include <stdio.h>

#include <errno.h>

#include <string.h>
#include <fcntl.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>

#define OUT_BUFFER_SIZE 1024
//one more than what could come in
#define IN_BUFFER_SIZE 8192+1

#define WKP "birdo"

void catch_error(int er);

char handshake();

char out_buff[OUT_BUFFER_SIZE];
char in_buff[IN_BUFFER_SIZE];
int sucssess;

void parse(char *b);

int main() {
    if(handshake()) exit(1);

    //open pipe out
    int out_pipe = open("waluigi",O_WRONLY);
    catch_error(out_pipe);
    //open pipe in
    int in_pipe = open("wario",O_RDONLY);
    catch_error(out_pipe);

    //pipe open, write stuff
    while(fgets(out_buff, sizeof(out_buff), stdin)) {

        parse(out_buff);

        sucssess = write(out_pipe,out_buff,sizeof(out_buff));
        //catch_error(sucssess);

        //read and print response
        sucssess = read(in_pipe,in_buff, sizeof(in_buff)-1);
        //catch_error(sucssess);

        //in_buff is expected to be a null terminated string
        printf("%s",in_buff);
    }


    close(in_pipe), close(out_pipe);

    return 0;
}

//parses for special commands, currently the only one is EXIT
void parse(char *b) {
    if(!strcmp(b, "EXIT\n")) {
        exit(0);
    }
}

//return 0 on sucess
char handshake() {
    int out_pipe = open(WKP, O_WRONLY);
    catch_error(out_pipe);

    sprintf(out_buff, "%d", getpid());

    sucssess = mkfifo(out_buff, 0644);
    if(errno != EEXIST) catch_error(sucssess);


    write(out_pipe, out_buff, sizeof(out_buff));


    int in_pipe = open(out_buff, O_RDONLY);
    catch_error(in_pipe);

    sucssess = remove(out_buff);
    catch_error(sucssess);


    read(in_pipe, in_buff, sizeof(in_buff)-1);

    //0 is acknoledgement code
    if((in_buff[0] - '0')) return 1;

    return 0;
}

void catch_error(int er) {
    if(er == -1) {
        printf("error %d: %s\n",errno ,strerror(errno));
        exit(-1);
    }
}
