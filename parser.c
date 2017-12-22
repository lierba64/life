/**************************************
 * file:        parser.c
 * description: parse flow config file
 * author:      kari.zhang
 * date:        2017-12-22
 *************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#include "parser.h"

typedef struct Param {
    char* name;     // parameter name
    char* type;     // parameter type
    bool optional;  // optional parameter
} Param;

typedef struct Node {
    int id;             // node id 
    char* name;         // node name defined by user, may be NULL
    char* description;  // node description
    /* 
     * define host name, if NULL means use that deinfed in Flow
     */
    char* devHost;      // develop host
    char* demoHost;     // demo host
    char* productHost;  // product host
    char* apiUrl;       // api path exclude host
    Param *params;      // api params
    int paramCnt;       // params count

    char* response;
    char* keypoint;

    // child defined use for logic jumping
    int childCnt;
    int* childMap;
    Node** children;
    Flow *flow;
} Node;

typedef struct Flow {
    int id;
    char* name;
    char* description;
    char* devHost;
    char* demoHost;
    char* productHost;
    Node* rootNode;
} Flow;


typedef struct Profile {
    char* name;       
    char* package;
    char* description;
} Profile;

#define STATIC_MODE 0
#define DYNAMIC_MODE 1
typedef struct Context {
    Profile* profile;
    Node** nodes;
    int nodeCnt;
    Flow* flow;     
    int mode;       // static mode or dynamic mode
} Context;

extern int takePlatform(const char *option)
{
    int platform = PLATFORM_UNKNOWN; 
    if (!strncmp("-i", option, 2)) {
        platform = PLATFORM_IOS;
    } else if (!strncmp("-a", option, 2)) {
        platform = PLATFORM_ANDROID;
    } 
    return platform;

}

extern bool checkPlatform(int platform)
{
    if (platform == PLATFORM_ANDROID || platform == PLATFORM_IOS) {
        return TRUE;
    }
    return FALSE;
}

extern bool validatePath(const char *path)
{
    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        return FALSE;
    }
    fclose(fp);
    return TRUE;

}

static char* readFileText(const char* path) {
    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        return NULL;
    }

    fseek(fp, 0, SEEK_END);    
    long size = ftell(fp);
    rewind(fp);

    char *text = (char *)calloc(1, size + 1);
    do {
        if (text == NULL) {
            break;
        }

        if (fread(text, size, 1, fp) != 1) {
            free(text);
            text = NULL;
            break;
        }

    } while (0);

    fclose(fp);
    return text;
}

extern Context* createContext(const char* path) {
    return NULL;
}

static void analyzeProfile(const char* path)
{
    char* text = readFileText(path);
    if (text == NULL) {
        printf("Failed read %s \n", path);
        return;
    }

    cJSON *root = cJSON_Parse(text);
    free(text);

    char* name = cJSON_GetObjectItem(root, "name")->valuestring;
    printf("name:%s\n", name);

    char* package = cJSON_GetObjectItem(root, "package")->valuestring;
    printf("package:%s\n", package);

}

extern void performLife(Context* ctx, int platform)
{

}
