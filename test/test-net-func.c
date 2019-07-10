//
// Created by Administrator on 2019/7/2.
//


/*#include <w32api/inaddr.h>
#include <cygwin/in.h>*/
#include "../src/core/net.h"
#include <sys/select.h>
#include "../src/algorithm/rb-tree.h"
#include "../src/core/string-tool.h"


int main() {
    char *src = "  123456789  ";//len=14
    src = trim(src, 14, ' ');
    printf("%d\n", strlen(src));
    printf("%s\n", src);
}