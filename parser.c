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
    char* prodHost;     // product host
    char* apiUrl;       // api path exclude host
    Param **params;     // api params
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

static bool analyzeProfile(const char* path, Context *ctx)
{
    char* text = readFileText(path);
    if (text == NULL) {
        printf("Failed read %s \n", path);
        return FALSE;
    }

    cJSON *root = cJSON_Parse(text);
    free(text);

    Profile *profile = (Profile *)calloc(1, sizeof(Profile));
    if (NULL == profile) {
        return FALSE;
    }

    char* name = cJSON_GetObjectItem(root, "name")->valuestring;
    profile->name = name;
    char* package = cJSON_GetObjectItem(root, "package")->valuestring;
    profile->package = package;
    char* description = cJSON_GetObjectItem(root, "description")->valuestring;
    profile->description = description;
    ctx->profile = profile;

    cJSON* apiSet =cJSON_GetObjectItem(root, "apiSet");
    char* devHost = cJSON_GetObjectItem(apiSet, "dev-host")->valuestring;   
    char* demoHost = cJSON_GetObjectItem(apiSet, "demo-host")->valuestring;
    char* prodHost = cJSON_GetObjectItem(apiSet, "prod-host")->valuestring;

    cJSON* api = cJSON_GetObjectItem(apiSet, "api");
    int size = cJSON_GetArraySize(api);
    ctx->nodeCnt = size;
    ctx->nodes = (Node **)calloc(size, sizeof(Node*));
    if (NULL == ctx->nodes) {
        printf("Failed calloc for nodes \n");
        exit(10);
    }

    int i;
    for (i = 0; i < size; i++) {
        Node* node = (Node *)calloc(1, sizeof(Node));
        if (NULL == node) {
            printf("Failed calloc for node \n");
            exit(11);
        }

        cJSON* item = cJSON_GetArrayItem(api, i);
        int id = cJSON_GetObjectItem(item, "id")->valueint;
        char* apiUrl = cJSON_GetObjectItem(item, "apiUrl")->valuestring;
        node->id = id;
        node->apiUrl = apiUrl;

        cJSON* devSrvr = cJSON_GetObjectItem(item, "dev-host");
        if (devSrvr != NULL) {
            char* dev = devSrvr->valuestring;
            node->devHost = dev;
        } else {
            node->devHost = devHost;
        }

        cJSON* demoSrvr = cJSON_GetObjectItem(item, "demo-host");
        if (demoSrvr != NULL) {
            char* demo = demoSrvr->valuestring;
            node->demoHost = demo;
        } else {
            node->demoHost = demoHost;
        }

        cJSON* prodSrvr = cJSON_GetObjectItem(item, "prod-host");
        if (prodSrvr != NULL) {
            char* prod = cJSON_GetObjectItem(item, "prod-host")->valuestring;
            node->prodHost = prod;
        } else {
            node->prodHost = prodHost;
        }

        cJSON* params = cJSON_GetObjectItem(item, "param");
        int paramCnt = cJSON_GetArraySize(params);
        node->paramCnt = paramCnt;
        node->params = (Param **)calloc(paramCnt, sizeof(Param *));
        if (NULL == node->params) {
            printf("Failed calloc for params \n");
            exit(12);
        }

        int j;
        for(j = 0; j < paramCnt; j++) {
            cJSON* cp = cJSON_GetArrayItem(params, j);
            char* name = cJSON_GetObjectItem(cp, "name")->valuestring;
            char* type = cJSON_GetObjectItem(cp, "type")->valuestring;
            printf("name:%s type:%s\n", name, type);

            Param* param = (Param*)calloc(1, sizeof(Param));
            param->optional = FALSE;
            param->name = name;
            param->type = type;
            node->params[j] = param;
        }

        ctx->nodes[i] = node;
    }

    return TRUE;
}

extern Context* createContext(const char* path) {
    Context *ctx = (Context *)calloc(1, sizeof(Context));
    if (NULL == ctx) {
        return NULL;
    }

    if(!analyzeProfile(path, ctx)) {
        free(ctx);
        return NULL;
    }
    return ctx;
}

extern void performLife(Context* ctx, int platform)
{
    printf("perform life \n");
}

extern void freeContext(Context* ctx)
{
    if (NULL == ctx) {
        return;
    }

    Profile *profile = ctx->profile;
    if (NULL != profile) {
        if (NULL != profile->name) {
            free(profile->name);
        }
        if (NULL != profile->package) {
            free(profile->package);
        }
        if (NULL != profile->description) {
            free(profile->description);
        }
    }
    free(ctx);
}
