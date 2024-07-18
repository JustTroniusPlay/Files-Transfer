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

void multi_clear(CURL** curl_arr, CURLM* multi, int save_amount)
{
    for(int i_curl = 0; i_curl < save_amount; i_curl++)
    {
        curl_multi_remove_handle(multi, &curl_arr[i_curl]);
        curl_easy_cleanup(&curl_arr[i_curl]);
    }
}

static double diffclock(clock_t clock1,clock_t clock2)
{
    double diffticks=clock1-clock2;
    double diffms=(diffticks)/(CLOCKS_PER_SEC/1000);
    return diffms;
}

string dir_creator(string path_to_dir)
{
    bool flag = std::filesystem::create_directory(path_to_dir);
    char user_answ = ' ';
    while(!flag)
    {
        cout << "Directory \"" << path_to_dir << "\" already exist. Do you wish to use it or not?[y/n]" << endl;
        cin >> user_answ;
        switch(user_answ)
        {
            case 'y':
                cout << "Current directory will be used." << endl;
                flag = true;      
                break;
            case 'n':
                cout << "Please enter new destination directory path:" << endl;
                cin >> path_to_dir;
                flag = std::filesystem::create_directory(path_to_dir);
                break;
            default:
                cout << "Invalid input: please try again." << endl;
                break;
            }
            
        
    }
    return path_to_dir;
}

int pos_finder(const string& str, char symbol)
{
    int index = 0;
    for(int i = str.length() - 1; i > 0; i--)
    {
        if(str[i] == symbol && !(i + 1 > str.length()))
        {
            index = i + 1;
            break;
        }
    }
    return index;
    
}

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
        string err = "ERROR: Path to source file \"" + path_to_file + "\" is invalid to continue.";
        throw err; 
    }
    in.close();
}

void saver(string path_to_dir,  const vector<string>&list)
{ 
    curl_global_init(CURL_GLOBAL_ALL);
    CURL *curl = curl_easy_init();
    //curl_easy_setopt(curl, CURLOPT_VERBOSE, true); for debug
    path_to_dir = dir_creator(path_to_dir); 
    char proh_symb[] = {'\\', '/', ':', '*', '?', '\"','<', '>', '|', '+'};
    if(curl)
    {
        for(string str : list)
        {
            const char* url = str.c_str();
            curl_easy_setopt(curl, CURLOPT_URL, url);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);                

            int index = pos_finder(str, '/');
            string file_name = str.substr(index, str.length() - 1);
            for(int ch_ind = 0; ch_ind < 10; ch_ind++)
            {
                std::replace(file_name.begin(), file_name.end(), proh_symb[ch_ind], '_');
            }
            string to_save = path_to_dir + "\\" + file_name;
            const char* to_dir = to_save.c_str();

            FILE *check = fopen(to_dir,"r");
            int copy_idx = 1;
            while(check)
            {
                int point_idx = pos_finder(file_name, '.');
                string file_format = file_name.substr(point_idx, file_name.length() - 1);
                string file_true_name = file_name.substr(0, point_idx - 1);
                string idx = std::to_string(copy_idx);
                to_save = path_to_dir + "\\" + file_true_name + '(' + idx + ')' + '.' + file_format;
                to_dir = to_save.c_str();
                check = fopen(to_dir, "r");
                if(check)
                {
                    copy_idx++;
                }
            }

            FILE *file = fopen(to_dir, "wb");
            if(!file)
            {
                fclose(file);
                url = NULL;
                to_dir = NULL;
                curl_easy_cleanup(curl);
                curl_global_cleanup();
                string err = "ERROR: Path to destination directory \"" + path_to_dir + "\" is invalid to continue. Possible cause: directory is forbbiden for use.";
                throw err; 
            }
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
            cout << "Start fetching file. . ." << endl;
            CURLcode res = curl_easy_perform(curl); 
            if (res != CURLE_OK) 
            {
                cout << "Download of file \"" << url << "\" failed with error \'" << curl_easy_strerror(res) <<"\'!" << endl;
                cout << "Moving to next URL. . ." << endl << endl;
            }
            else
            {
                cout << "Download of file \"" << url << "\" was succsesful!" << endl;
                cout << "Moving to next URL. . ." << endl << endl;
            }
            fclose(file);
            index = 0;
            url = NULL;
            to_dir = NULL;
        }
        curl_easy_cleanup(curl);
    }
    else
    {
        curl_easy_cleanup(curl);
        cout << "ERROR: connection failed." << endl;
    }
    cout << "All URLs was used." << endl;
    curl_global_cleanup();
}

void saver_multi(string path_to_dir, const vector<string>& list, int simult)
{
    path_to_dir = dir_creator(path_to_dir);
    curl_global_init(CURL_GLOBAL_ALL);
    CURL *curl_arr[999];
    CURL **pCurl_Arr = curl_arr;
    CURLM *multi = curl_multi_init();
    curl_multi_setopt(multi, CURLMOPT_MAXCONNECTS, simult);
    char proh_symb[] = {'\\', '/', ':', '*', '?', '\"','<', '>', '|', '+'};
    int added_file = simult;

    if(simult > list.size())
    {
        simult = list.size();
    }
    int lenght = list.size();
    int url_index = 0;
    while(lenght != 0)
    {
        added_file = simult;
        if(simult > lenght)
        {
            simult = lenght;
            added_file = simult;
        }
        cout << "Starting adding files to que." << endl << endl;
        for(int i_curl = 0; i_curl < simult; i_curl++)
        {
            curl_arr[i_curl] = curl_easy_init();
            if(curl_arr[i_curl])
            {
                const char* url = list[url_index].c_str();
                
                curl_easy_setopt(curl_arr[i_curl], CURLOPT_URL, url);
                curl_easy_setopt(curl_arr[i_curl], CURLOPT_WRITEFUNCTION, write_data);

                int index = pos_finder(list[url_index], '/');
                string file_name = list[url_index].substr(index, list[url_index].length() - 1);
                for(int ch_ind = 0; ch_ind < 10; ch_ind++)
                {
                    std::replace(file_name.begin(), file_name.end(), proh_symb[ch_ind], '_');
                }
                string to_save = path_to_dir + "\\" + file_name;
                const char* to_dir = to_save.c_str();
                url_index++;
                
                FILE *check = fopen(to_dir,"r");
                int copy_idx = 1;
                while(check)
                {
                    int point_idx = pos_finder(file_name, '.');
                    string file_format = file_name.substr(point_idx, file_name.length() - 1);
                    string file_true_name = file_name.substr(0, point_idx - 1);
                    string idx = std::to_string(copy_idx);
                    to_save = path_to_dir + "\\" + file_true_name + '(' + idx + ')' + '.' + file_format;
                    to_dir = to_save.c_str();
                    check = fopen(to_dir, "r");
                    if(check)
                    {
                        copy_idx++;
                    }
                }

                FILE *file = fopen(to_dir, "wb");
                if(!file)
                {
                    fclose(file);
                    url = NULL;
                    to_dir = NULL;
                    multi_clear(pCurl_Arr, &multi, simult);
                    curl_multi_cleanup(multi);
                    curl_global_cleanup();
                    string err = "ERROR: Path to destination directory \"" + to_save + "\" is invalid to continue. Possible cause: directory is forbbiden for use.";
                    throw err; 
                }
                curl_easy_setopt(curl_arr[i_curl], CURLOPT_WRITEDATA, file);
                curl_multi_add_handle(multi, curl_arr[i_curl]);
                cout << "File \"" << file_name << "\" successfuly added to download." << endl;
                fclose(file);
                url = NULL;
                to_dir = NULL;
            }
            else
            {
                multi_clear(pCurl_Arr, &multi, simult);
                curl_multi_cleanup(multi);
                curl_global_cleanup();
                string err = "ERROR: connection failed for Curl.";
                throw err;

            }
        }

        cout << "Start fetching added files. . ." << endl;

        struct CURLMsg *msg;
        int msgq = 0;
        int still_running = 1;
        while(still_running)
        {
            CURLMcode mc = curl_multi_perform(multi, &still_running);

            if(still_running)
            {
                mc = curl_multi_poll(multi, NULL, 0, 1000, NULL);
                if(mc) break;
            }
        } 

        do
        {
            msg = curl_multi_info_read(multi, &msgq);
            if(msg && (msg->msg == CURLMSG_DONE))
            {
                char *r_url;
                curl_easy_getinfo(msg->easy_handle, CURLINFO_EFFECTIVE_URL, &r_url);
                cout << "Download for URL: \"" << r_url << "\" ended with \'" << curl_easy_strerror(msg->data.result) << "\'!" << endl;
                added_file--;
            }
        } while(added_file);
         
        multi_clear(pCurl_Arr, &multi, simult);
        cout << "End of current que." << endl << endl;
        lenght -= simult;
        if(lenght < 0)
        {
            lenght = 0;
        }
    }

    cout << "All URLs were used." << endl;
    curl_multi_cleanup(multi);
    curl_global_cleanup();
}

// Callback function to handle downloaded data
size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) 
{
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

int main()
{
    int clock_begin;
    clock_begin = clock();
    cout << "File Transfer. Starting programm. . ." << endl \
    << "Please enter: The path to file with URLs; Path to directory; Number of files to download simultaneously:" << endl;
    
    string pth;
    string ptd;
    int fl_amt;
    
    cin >> pth >> ptd;
    while(!(cin >> fl_amt))
    {
        cin.clear(); 
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        cout << "Invalid number input; please re-enter:\n";
    }
    cout << endl;
    if(fl_amt > 999)
    {
        cout << "Amount of files to high. It will be caped on 999." << endl << endl;
        fl_amt = 999;
    }
    else if(fl_amt < 1)
    {
        cout << "Amount of files is too low. It will be defined as 1." << endl << endl;
        fl_amt = 1;
    }

    vector<string> url_list;

    try
    {
        reader(pth, url_list);
        if(fl_amt > 1)
        {
            saver_multi(ptd, url_list, fl_amt);
        }
        else
        {
            saver(ptd, url_list);
        }
    }
    catch(string err)
    {   
        cout << err << endl;
        return 1;
    }

    int clock_end = clock();

    cout << "End of programm. . ." << endl << "Programm worked for: " << diffclock(clock_end, clock_begin) << "ms." << endl;

    return 0;
}