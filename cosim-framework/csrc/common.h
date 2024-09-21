#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdlib.h>

#define str(x) #x
#define concat(x, y) x ## y
#define todo(msg) (fprintf(stderr, "warning: %s:%d [TODO] %s\n", __FILE__, __LINE__, msg))
#define fatalf(fmt, ...) (fprintf(stderr, "fatal: %s:%d " fmt "\n", __FILE__, __LINE__, __VA_ARGS__), exit(1))
#define fatal(msg) fatalf("%s", msg)







#endif