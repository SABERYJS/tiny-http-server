//
// Created by Administrator on 2019/7/2.
//

#include "../src/core/net.h"
#include <sys/select.h>
#include "../src/algorithm/rb-tree.h"
#include "../src/core/string-tool.h"
#include "../src/core/config.h"
#include "../src/core/log.h"


int main() {
    char *filename = "E:\\code\\algorithm\\app.conf";
    struct Config *config = ConfigCreate(filename);
    struct Log *log = LogCreate(0, STDOUT_FILENO, NULL, LOG_LEVEL_DEBUG);
    if (!config) {
        LogInfo(log, "create config failed\n");
        exit(0);
    } else {
        ConfigFileParse(config);
    }
}