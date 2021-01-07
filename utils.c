#include <stdlib.h>
#include <string.h>

#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

#define arraylen(x)  (sizeof(x) / sizeof((x)[0]))

int strisblank(char * s) {
    if(!s || !strlen(s)) {
        return 1;
    }
    int i = 0;
    for (i = 0; i < strlen(s); ++i) {
        if(s[i] == ' ' || s[i] == '\t' || s[i] == '\n') {
        } else {
            return 0;
        }
    }
    return 1;
}

