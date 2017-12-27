/**************************************
 * file:        parser.h
 * description: parse flow config file
 * author:      kari.zhang
 * date:        2017-12-22
 *************************************/

#ifndef __PARSER__H__
#define __PARSER__H__

#include <stdio.h>

typedef unsigned char bool;
#define TRUE 1
#define FALSE 0

#define PLATFORM_UNKNOWN 0
#define PLATFORM_ANDROID 1
#define PLATFORM_IOS 2

typedef struct Node Node;
typedef struct Flow Flow;
typedef struct Context Context;

extern int takePlatform(const char *platform);
extern bool checkPlatform(int platform);
extern bool validatePath(const char *path);
extern Context* createContext(const char* path);
extern void performLife(Context* cxt, int platform);
extern void freeContext(Context* cxt);

#endif
