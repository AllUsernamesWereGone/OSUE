#include "http.h"





int main(int argc, char *argv[]) {

    int opt;
    int port = 80; //standard port
    char* url = NULL;
    char* file_name = NULL;
    char* dir_name = NULL;
    char *path = NULL;
    char host[256];
    


    while ((opt = getopt(argc, argv, "p:o:d:")) != -1) {
        switch (opt) {
        case 'p':
            //port handling
            port = atoi(optarg);
            break;
        case 'o':
            //FILE name
            file_name = optarg;
            break;
        case 'd':
            //DIRectory name
            dir_name = optarg;
            break;
        default: /* '?' */
            usage();
        }
    }

    if(file_name && dir_name){
        usage();
    }

    if (optind >= argc) {
        usage();
    }else{
        url = argv[optind];
    }
    

    if (strlen(url) <= 7 || strncmp(url, "http://", 7) != 0) {
        error("Invalid url");
        usage();
    }
    
    // split URL into the path and host
    // http://www.example.com/test.html
    // path: /test.html
    // host: www.example.com
    path = strpbrk(url + 7, ";/?:@=&");
    sprintf(host, "%.*s", (int) (path ? (path - (url + 7)) : strlen(url + 7)), url + 7);
    if (!path) path = "/";


    //tries to open the file or the directoy which is specified
    FILE *file = NULL;
    if (file_name) {
        if ((file = fopen(file_name, "wb+")) == NULL) {
            error("Unable to open file");
            return EXIT_FAILURE;
        }
    } else if (dir_name) {
        char buf[256];
        char *pos = strrchr(path, '/');
        char *qpos = strchr(pos, '?');
        char *name = (pos[1] == 0 || pos[1] == '?') ? "index.html" : pos + 1;
        //constructs the path
        sprintf(buf, "%s/%.*s", dir_name, (int) ((name != pos + 1 || qpos == NULL) ? strlen(name) : qpos - pos), name);
        if ((file = fopen(buf, "wb+")) == NULL) {
            error("Unable to open file");
            return EXIT_FAILURE;
        }
    }

    



















    
    printf("Client success\n");
    return EXIT_SUCCESS;
}


void usage(void){
    fprintf(stderr, "usage: client [-p PORT] [ -o FILE | -d DIR ] URL\n");
    exit(EXIT_FAILURE);
}


void error(const char *msg) {
    if (errno == 0) {
        fprintf(stderr, "%s\n",msg);
    } else {
        fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    }
}