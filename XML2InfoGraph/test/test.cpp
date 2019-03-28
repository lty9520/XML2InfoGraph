#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <fstream>
#include <time.h>

#include "../include/xml2json.hpp"
#define WIN32  
// #include <sys/time.h>


using namespace std;

void getCurrentTime()
{
	system("time");
}

int main(int argc, const char *argv[])
{
    //file<> fdoc("track_orig.xml");
    string xml_str, json_str;
    ifstream inf;
    ofstream outf;
    ostringstream oss;
    char BOM[4] = {(char)0xEF, (char)0xBB, (char)0xBF, '\0'}; /*BOM String*/
    int64_t start_time, end_time;

    inf.open("007.xml");
    outf.open("007.json");
    oss.str("");
    oss << inf.rdbuf();
    xml_str = oss.str();
    inf.close();
    //getCurrentTime();
    json_str = xml2json(xml_str.c_str());
    //getCurrentTime();
    //cout << "time1: " << end_time - start_time << endl;
    outf << BOM << json_str;
    outf.close();
	/*
    inf.open("test_track_2.xml");
    outf.open("test_track_2.js.txt");
    oss.str("");
    oss << inf.rdbuf();
    xml_str = oss.str();
    inf.close();
    getCurrentTime();
    json_str = xml2json(xml_str.c_str());
    getCurrentTime();
    //cout << "time2: " << end_time - start_time << endl;
    outf << BOM << json_str;
    outf.close();

    inf.open("test_track_3.xml");
    outf.open("test_track_3.js.txt");
    oss.str("");
    oss << inf.rdbuf();
    xml_str = oss.str();
    inf.close();
    getCurrentTime();
    json_str = xml2json(xml_str.c_str());
    getCurrentTime();
    //cout << "time3: " << end_time - start_time << endl;
    outf << BOM << json_str;
    outf.close();
*/
    return 0;
}
