#include <iostream>
#include "LZcode.h"

using namespace std;

int main() {

    LZ pressFile;
    string path="image001.jpg";//修改文件名
    pressFile.compress(path);
    pressFile.decode("result.jpg");//修改解压后文件名
    getchar();
    return 0;
}


