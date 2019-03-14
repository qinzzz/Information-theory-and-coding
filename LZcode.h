//
// Created by qinzzz on 2018/11/10.
//

#ifndef FILETEST_LZ_H
#define FILETEST_LZ_H

#include <iostream>
#include <fstream>
#include <time.h>
#include <map>

using namespace std;


class LZ{
public:
    string file;
    map<string,int> inDic;  //压缩字典
    map<int,string> outDic; //解压字典

    struct outData{
        int index;
        char lastChar;
    };     //二进制输出结构体

    LZ(){};
    void compress(string readfile);
    void decode(string file);
};

void LZ::compress(string readfile){

    file=readfile;
    outData *output=new outData;
    clock_t start,end;
    start = clock();//开始计时
    fstream infile,outfile;//打开文件
    infile.open(file,ios::in|ios::binary);
    outfile.open(file+".lz",ios::out|ios::binary);
    if (!infile){
        cout<<"infile open error!";
        return;
    }
    if(!outfile){
        cout<<"outfile open error!";
        return;
    }
    cout<<"file opened.\n";
    cout<<"compressing file......\n";

    char *data=new char;    //读取二进制文件的指针
    int i=1; //字典中索引
    int preIndex;

    while(!infile.eof()){
        infile.read(data,sizeof(char));
        if (infile.eof()){
            break;
        }

        string inData ="";  //读取的字符串
        inData+=data[0];
        map<string, int>::iterator iter;    //map迭代器
        iter=inDic.find(inData);    //在字典中搜索inData

        if (iter== inDic.end()){    //无匹配项
            inDic.insert(pair<string,int>(inData,i));   //加入字典
            output->index=0;
            output->lastChar=inData[0];
            outfile.write((char*)output,sizeof(outData));
            i++;
        }
        else{   //有匹配项

            while(!infile.eof()) {
                infile.read(data,sizeof(char));
                preIndex=iter->second;

                if (infile.eof()){  //读到文件尾

                    char lastChar = inData[inData.size() - 1];
                    inData.erase(inData.end() - 1);
                    iter = inDic.find(inData);
                    output->index=iter->second;
                    output->lastChar=lastChar;
                    outfile.write((char*)output,sizeof(outData));
                    break;
                }

                inData+=data[0];    //将新读入的字符加入inData，直到在字典中无匹配项
                iter=inDic.find(inData);

                if (iter == inDic.end()){
                    inDic.insert(pair<string,int>(inData,i));
                    output->index=preIndex;
                    output->lastChar=data[0];
                    outfile.write((char*)output,sizeof(outData));
                    i++;
                    break;
                }
            }

        }
    }
    infile.close();
    end = clock();  //结束计时
    cout<<"compress time: "<<(double(end - start) / CLOCKS_PER_SEC)<<" /s"<<endl;
}

void LZ::decode(string dstfile){
    clock_t start,end;
    start = clock();
    ifstream infile;
    ofstream outfile;
    infile.open(file+".lz", ios::in|ios::binary);
    outfile.open(dstfile, ios::out|ios::binary);
    cout<<"new file opened."<<endl;
    outData *getdata = new outData;     //用结构体读入
    int j=1;//count
    if (!infile){
        cout<<"infile open error!";
        return;
    }
    if(!outfile){
        cout<<"outfile open error!";
        return;
    }
    cout<<"start decoding......"<<endl;

    map<int,string>::iterator iter2;    //迭代器
    string readData="";     //文件读入的字符串

    while(!infile.eof()){
        infile.read((char*)getdata,sizeof(outData));
        if (infile.eof()){
            break;
        }
        int readIndex;
        char readChar;
        readIndex=getdata->index;
        readChar=getdata->lastChar;
        if(readIndex==0){   //新出现的字符
            readData=readChar;
            for(int c=0;c < readData.size();c++){
                outfile.write(&readData[c],sizeof(char));   //直接写出到解压文件
            }
            outDic.insert(pair<int,string>(j,readData));    //插入字典
            j++;
        }
        else{
            iter2=outDic.find(readIndex);   //在字典中匹配前缀序号
            readData = iter2->second + readChar;    //输出字符串

            for(int c=0;c < readData.size();c++){
                outfile.write(&readData[c],sizeof(char));
            }
            outDic.insert(pair<int,string>(j,readData));    //插入字典
            j++;
        }
    }
    end=clock();
    cout<<"decode time:"<<(double(end - start) / CLOCKS_PER_SEC)<<" /s"<<endl;
    infile.close();
    outfile.close();

}
#endif //FILETEST_LZ_H

