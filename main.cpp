#include <iostream>
#include <fstream>
#include <vector>

using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::ifstream;
using std::vector;

void reader(string, vector<string>&);

void reader(string path_to_file, vector<string>&list)
{
    ifstream in;
    string line;

    in.open(path_to_file);
    if(in.is_open())
    {
        while(getline(in, line))
        {
            list.push_back(line);
        }
    }
    else
    {
        cout << "ERROR: incorrect input file path." << endl; //User can re-unput file path???
    }
    in.close();
}

int main()
{
    cout << "File Transfer. Starting programm." << endl \
    << "Please enter: The path to file with URLs; Path to catalogue; Number of files to download simultaneously:" << endl;
    
    string pth;
    string dest;
    float fl_amt;
 
    cin >> pth >> dest >> fl_amt;
    cout << endl;

    vector<string> url_list;
    reader(pth, url_list);
    
    return 0;
}