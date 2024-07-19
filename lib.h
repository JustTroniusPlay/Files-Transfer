#include <iostream>
#include <fstream>
#include <vector>
#include <curl\curl.h>
#include <stdio.h>
#include <string.h>
#include <filesystem>
#include <algorithm>

using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::ifstream;
using std::ofstream;
using std::vector;

void reader(string, vector<string>&);

void saver(string, const vector<string>&);

void saver_multi(string, const vector<string>&, int simult);

void multi_clear(CURL**, CURLM*, int);

int pos_finder(const string&, char);

string dir_creator(string);

size_t write_data(void *, size_t, size_t, FILE *);

static double diffclock(clock_t, clock_t);