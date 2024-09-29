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

void reader(string&, vector<string>&);

void saver_(string&, const vector<string>&);

void saver(string&, const vector<string>&, int simult);

void multi_clear(CURL**, CURLM*, int);

int pos_finder(const string&, char);

string to_dir_checked(const string&, const string&);

string dir_creator(string&);

static size_t write_data(void *, size_t, size_t, void *);

static double diffclock(clock_t, clock_t);