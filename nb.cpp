/***************************************************************************
 * 
 * Copyright (c) 2014 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 
 
/**
 * @file train.cpp
 * @author shirenxie01(shirenxie01@baidu.com)
 * @date 2014/04/12 13:38:40
 * @version $Revision$ 
 * @brief 
 *  
 **/
#include <iostream>
#include <vector>
#include <map>
#include<string.h>
#include<dirent.h>
#include <fstream>  
#include<sstream>
#include <stdlib.h> 
#include <time.h>
#include <math.h>
using namespace std;

int main()
{
    //read file get word vector
    DIR *dp;
    struct dirent *dirp;
    string dirname = "/home/rockics/machine_learning/peileyuan/class01/data01/";
    vector<string> filename_list;
    vector<string> filename_list_test;
    vector<string> type_list;
    map<string, double> type_p;
    map<string, int> word_count;
    map<string, double> type_default_p;
    map<string, map<string, double> > type_word;
    int total = 0;
    map<string, int> type_word_total;

    //验证文件的合法性
    if((dp = opendir(dirname.c_str())) == NULL){
        return 1;
    }
    while((dirp=readdir(dp))!=NULL){
        if((strcmp(dirp->d_name,".")==0)||(strcmp(dirp->d_name,"..")==0)) {
            continue;
        }
        //srand((unsigned)time(NULL));
        int myrand = rand()%101; 
        if (myrand > 80) {
            filename_list_test.push_back(dirname+dirp->d_name);
        }else {
            filename_list.push_back(dirname+dirp->d_name);
        }
    }

    vector<string>::iterator path;
    map<string,double>::iterator map_it, map_end;
    std::string::size_type pos0,pos1;
    cout << "共有"<<filename_list.size() + filename_list_test.size()<< "个文件"<<endl;
    cout << "其中训练样本"<<filename_list.size()<< "个文件"<<endl;
    cout << "测试样本"<<filename_list_test.size()<< "个文件"<<endl;
    for (path = filename_list.begin(); path != filename_list.end(); path++){
        string mypath = *path;
        pos0 = mypath.find("data01/");
        string type = mypath.substr(pos0+7,-1);
        pos1 = type.find('.');
        type = type.substr(0, pos1);
        string::iterator t = type.begin();
        while(t != type.end())
        {
            if(*t >= '0' && *t <= '9')
            {
                type.erase(t);
            }else {
                t++;
            }
        }
        bool not_in = 0;
        for (map_it = type_p. begin(); map_it != type_p.end(); map_it++)
        {
            if (type == map_it->first) 
            {
                not_in = 1;
                map_it->second ++;
            }
        }
        if (not_in == 0) {
            type_p.insert(make_pair(type, 1));
        }
        total ++;
        
        //read file count word 
        ifstream infile(mypath.c_str());
        if(!infile){
            cout << "地址拼接有误:" << mypath.c_str() << endl;
            return 1;
        }

        string line;
        string word;
        map<string, map<string, double> >::iterator map_map_it;
        map<string, int>::iterator it_word_count;
        while (getline(infile, line)){
            istringstream stream(line);
            while(stream >> word) {
                //训练样本中词的计数
                it_word_count = word_count.find(word);
                if (it_word_count == word_count.end()) {
                    word_count.insert(make_pair(word,1));
                }
                //保存每个类别每个单词的计数
                map_map_it = type_word.find(type);
                if (map_map_it == type_word.end()) {
                    map<string, double> temp_map;
                    temp_map.insert(make_pair(word, 1));
                    type_word.insert(make_pair(type, temp_map));
                }else {
                    map<string, double>::iterator word_it;
                    word_it = map_map_it->second.find(word);
                    if (word_it == map_map_it->second.end()) {
                        map_map_it->second.insert(make_pair(word, 1));
                    } else{
                        word_it->second++;
                    }
                }
                //保存每个类别单词总数
                map<string, int>::iterator type_word_total_it;
                type_word_total_it = type_word_total.find(type);
                if (type_word_total_it == type_word_total.end()) {
                    type_word_total.insert(make_pair(type, 1));
                }else{
                    type_word_total_it->second++;
                }
            }
        }
        infile.close();
        infile.clear();
    }

    cout << "测试样本中共有"<< word_count.size() << "个单词"<<endl;
    cout <<"训练数据中个类别的先验概率:"<<endl;

    //计算每个词在每个类别的概率
    //拉普拉斯平滑处理
    double lalp = 0.1;
    map<string, map<string, double> >::iterator it_1_lay;
    map<string, double>::iterator it_2_lay;
    for(it_1_lay = type_word.begin(); it_1_lay != type_word.end();it_1_lay++) {
        map<string, int>::iterator it_type_count = type_word_total.find(it_1_lay->first);
        type_default_p.insert(make_pair(it_1_lay->first, lalp/(it_type_count->second+word_count.size()*lalp)));
        for (it_2_lay = it_1_lay->second.begin(); it_2_lay != it_1_lay->second.end(); it_2_lay++) {
            it_2_lay->second = (double)(it_2_lay->second+lalp)/(it_type_count->second+word_count.size()*lalp);
        }
    }
    //计算每个类别先验概率
    for (map_it = type_p.begin(); map_it != type_p.end(); map_it++){
        map_it->second = map_it->second/total;
        cout << map_it->first << "\t" << map_it->second << endl;
    }
    //输出debug信息
    map<string,int>::iterator it_kk;
    for(it_kk = type_word_total.begin();it_kk != type_word_total.end(); it_kk++) {
        //cout << it_kk->first << "\t" << it_kk->second << endl;
    }
    for(it_1_lay = type_word.begin(); it_1_lay != type_word.end();it_1_lay++) {
        for (it_2_lay = it_1_lay->second.begin(); it_2_lay != it_1_lay->second.end(); it_2_lay++) {
            //cout << it_1_lay->first << "\t"<< it_2_lay->first << "\t" << it_2_lay->second << endl;
        }
    }

    //return 0;
    //预测
    int all_test = 0;
    int all_true = 0;
    for (path = filename_list_test.begin(); path != filename_list_test.end(); path++){
        all_test ++;
        string mypath = *path;
        pos0 = mypath.find("data01/");
        string type = mypath.substr(pos0+7,-1);
        pos1 = type.find('.');
        type = mypath.substr(pos0+7, pos1);
        string::iterator t = type.begin();
        while(t != type.end())
        {
            if(*t >= '0' && *t <= '9')
            {
                type.erase(t);
            }else {
                t++;
            }
        }
        //分别计算X属于每个类别的概率
        //cal begin
        string mytype = "unknow";
        map<string, double> type_map;
        for (map_it = type_p.begin(); map_it != type_p.end(); map_it++){
            //开始计算之前都要初始化
            mytype = "unknow";
            double log_value = 0; 
            double max_value = -10000000; 
            //read file count word 
            ifstream infile(mypath.c_str());
            if(!infile){
                cout << "地址拼接有误:" << mypath.c_str() << endl;
                return 1;
            }

            string line;
            map<string, map<string, double> >::iterator map_map_it;
            string word;
            map_map_it = type_word.find(map_it->first);
            if (map_map_it == type_word.end()) {//模型中找不到对应的类别信息
                //do nothing
            } else {
                while (getline(infile, line)){
                    istringstream stream(line);
                    while(stream >> word) {
                        map<string, double>::iterator word_it;
                        word_it = map_map_it->second.find(word);
                        if (word_it == map_map_it->second.end()) {//在模型中找不到对应的单词
                            log_value += log(type_default_p[map_it->first]);
                        } else{
                            log_value += log(word_it->second);
                        }
                    }
                }
            }
            //先验概率
            double kk = map_it->second;
            log_value = log_value+log(kk);
            type_map.insert(make_pair(map_it->first, log_value));
            if (log_value > max_value) {
                max_value = log_value;
                mytype = map_it->first; 
            }
            //一个文件打开k次，k为类别数，需要注意关闭
            infile.close();
            infile.clear();
            //cal end
        }
        if (mytype == type) {
            all_true ++;
        }
        /*
        cout <<"-----------------------begin------------------------" << endl;
        cout << type << "\t" << mytype <<endl;
        cout << "分析信息：" << endl;
        map<string, double>::iterator hh;
        for(hh = type_map.begin(); hh!=type_map.end(); hh++) {
            cout << type << "\t" << hh->first << "\t" << hh->second <<endl;
        }
        cout <<"-----------------------end------------------------" << endl;
        */
    }


    double accuracy = (double)all_true/all_test;
    cout << "All test:" << all_test << endl;
    cout << "All true:" << all_true << endl;
    cout << "Accuracy:" << accuracy << endl;
    return 0;

}



















/* vim: set ts=4 sw=4 sts=4 tw=100 */
