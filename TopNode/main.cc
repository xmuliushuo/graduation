/**
  * @brief project TopNode, 主程序入口
  * @file main.cc
  * @author Liu Shuo
  * @date 2011.04.18
  */

#include "main.h"
#include "topnode.h"

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
    TopNode topNode;
    if (argc == 1) {
        topNode.readConf(defaultFile);
    }
    else {
        topNode.readConf(argv[1]);
    }
    topNode.init();
    return 0;
}
