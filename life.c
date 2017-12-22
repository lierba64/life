/*****************************************
 * file:        life.c
 * description: lender invest flow engine
 * author:      kari.zhang
 * date:        2017-12-22
 ****************************************/

#include <stdio.h>
#include "parser.h"

static void usage();

void main(int argc, char* argv[]) {
    if (argc != 2 && argc != 3) {
        usage();
        return;
    } 

    if(!validatePath(argv[1])) {
        usage();
        return;
    }

    int platform = PLATFORM_ANDROID;
    if (argc == 3) {
        platform = takePlatform(argv[2]);
        if (!checkPlatform(platform)) {
            usage();
            return;
        }
    }
    Context *ctx = createContext(argv[1]);
    performLife(ctx, platform);
}

static void usage() {
    printf("Usage:\n");
    printf("life <profile-path> [platform] \n");
    printf("  profile-path should be valid\n");
    printf("  platform should be -a or -i, default is -a\n");
    printf("  -a: for Android \n");
    printf("  -i: for iOS \n");
}
