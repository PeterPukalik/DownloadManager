#include <stdio.h>
#include "header/server.h"
#include "header/client.h"
#include "header/http.h"
#include <cstring>

int main(int argc, char* argv[]) {

    char* noveArgumenty[argc - 1];
    noveArgumenty[0] = argv[0];
    for (int i = 2; i <= argc - 1; ++i) {
        noveArgumenty[i - 1] = argv[i];
    }

    if (strcmp(argv[1], "http") == 0) {
        return http(argc - 1, noveArgumenty);
    }
    else if (strcmp(argv[1], "client") == 0) {
        return client(argc - 1, noveArgumenty);
    }
    else {
        fprintf(stderr,"wrong arguments\n", argv[0]);
        return 1;
    }

    //./server 10000
    //./client frios2.fri.uniza.sk 10000

    //./sockety server 10000
    //./sockety client frios2.fri.uniza.sk 10000

    return 0;
}
