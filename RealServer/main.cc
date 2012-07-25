/**
  * @brief project RealServer, 主程序入口
  * @file main.cc
  * @author Liu Shuo
  * @date 2011.04.18
  */

#include "realserver.h"

int main(int argc, char **argv)
{
    char defaultFile[] = "default.conf";

    if (argc > 2) {
        cout << "usage: TopNode [config filepath]" << endl;
        return 0;
    }
#ifdef DEBUG
    cout << "DEBUG" << endl;
#endif
    RealServer rs;
    if (argc == 1) {
        rs.readConf(defaultFile);
    }
    else {
        rs.readConf(argv[1]);
    }
    rs.init();
    return 0;
}
