#include <unistd.h>
#include <stdio.h>

#include <errno.h>

#include <string.h>
#include <fcntl.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <signal.h>

//one more than what can come in
#define IN_BUFFER_SIZE 1024+1
#define OUT_BUFFER_SIZE 8192

#define WKP "birdo"

void catch_error(int er);
static void sighandler(int signo);

char handshake();

void process(char *s);

char in_buff[IN_BUFFER_SIZE];
char out_buff[OUT_BUFFER_SIZE];
int sucssess;

int main() {
    signal(SIGINT, sighandler);

    while(1) {
        if(handshake()) exit(1);


        //make pipes
        sucssess = mkfifo("waluigi",0644);
        if(errno != EEXIST) catch_error(sucssess);

        sucssess = mkfifo("wario",0644);
        if(errno != EEXIST) catch_error(sucssess);

        //open pipe in
        int in_pipe = open("waluigi",O_RDONLY);
        catch_error(in_pipe);
        //open pipe out
        int out_pipe = open("wario",O_WRONLY);
        catch_error(out_pipe);

        //remove pipes after connection made
        sucssess = remove("wario");
        catch_error(sucssess);
        sucssess = remove("waluigi");
        catch_error(sucssess);


        //pipe open
        //read into buffer
        while(read(in_pipe,in_buff, sizeof(in_buff)-1)) {
            process(in_buff);
            //write output buffer
            write(out_pipe,out_buff,sizeof(out_buff));
            //catch_error(sucssess);
        }
        //error on read
        //catch_error(sucssess);


        close(in_pipe), close(out_pipe); 
    }
    return 0;
}

char handshake() {
    sucssess = mkfifo(WKP, 0644);
    if(errno != EEXIST) catch_error(sucssess);

    int wkp = open(WKP, O_RDONLY);
    catch_error(wkp);

    sucssess = remove(WKP);
    catch_error(sucssess);


    read(wkp, in_buff, sizeof(in_buff)-1);

    int out_pipe = open(in_buff, O_WRONLY);
    catch_error(out_pipe);

    //0 is acknoledgement code
    write(out_pipe, "0", sizeof("0"));

    return 0;
}

static void sighandler(int signo) {
    if(signo == SIGINT) {
        remove(WKP);
        exit(0);
    }
}

void catch_error(int er) {
    if(er == -1) {
        printf("error %d: %s\n",errno ,strerror(errno));
        exit(-1);
    }
}

//stuffs subsets (or as much as will fit) into the output buffer
void process(char *s) {
    //remove newline
    char *p = strchr(s,'\n');
    if(p) {
        *p = '\0';
    }
    int n = strlen(s);

    char tmp[n];
    int i,j;
    int overal_place = 0;;
    for(i = 1; i < 1 << n; i++) {
        for(j = 0; j < n; j++) {
            if(i & (1<<j)) {
                if(overal_place == OUT_BUFFER_SIZE-2){
                    out_buff[overal_place++] = '\n';
                    out_buff[overal_place] = '\0';
                    return;
                }
                out_buff[overal_place++] = s[j];

            }
        }
        if(i != (1 << n) -1 ) {
            if(overal_place == OUT_BUFFER_SIZE-2) {
                out_buff[overal_place++] = '\n';
                out_buff[overal_place] = '\0';
                return;
            }
            out_buff[overal_place++] = ',' ;
        }
    }
    //we know overal_place <= OUT_BUFFER_SIZE-2 as otherwise it would have returned by now
    out_buff[overal_place++] = '\n';
    out_buff[overal_place] = '\0';
}
