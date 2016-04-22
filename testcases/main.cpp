/* 
 * File:   newsimpletest.cpp
 * Author: milk
 *
 * Created on 2014-10-1, 13:31:49
 */

#include <stdlib.h>
#include <iostream>
#include <boost/algorithm/string.hpp>
#include "XMLConfigReader.h"

/*
 * Simple C++ Test Suite
 */

void test1(std::string& config) {
   std::cout << "SysConfig test 1" << std::endl;
   char ff = 0xFF;
   bool t = ff==0xFF;
   std::map<int, std::string> test = {
       {1, "1"},
       {2, "2"}
   };
   for (auto v:test) {
       std::cout<<v.first<<":"<<v.second<<std::endl;
   }
}

void test2() {
    std::cout << "newsimpletest test 2" << std::endl;
    std::cout << "%TEST_FAILED% time=0 testname=test2 (newsimpletest) message=error message sample" << std::endl;

}

int main(int argc, char** argv) {
    std::cout<<"Starting "<<argv[0]<<std::endl;
    
    std::cout << "%SUITE_STARTING% newsimpletest" << std::endl;
    std::cout << "%SUITE_STARTED%" << std::endl;

    std::cout << "%TEST_STARTED% test1 (newsimpletest)" << std::endl;
    std::string file(argv[0]);
    int pos = file.rfind('/');
    file = file.substr(0, pos+1) + "config.xml";
    test1(file);
    std::cout << "%TEST_FINISHED% time=0 test1 (newsimpletest)" << std::endl;

    std::cout << "%TEST_STARTED% test2 (newsimpletest)\n" << std::endl;
    test2();
    std::cout << "%TEST_FINISHED% time=0 test2 (newsimpletest)" << std::endl;

    std::cout << "%SUITE_FINISHED% time=0" << std::endl;
    
    std::cout<<"Ending "<<argv[0]<<std::endl;

    return (EXIT_SUCCESS);
}

