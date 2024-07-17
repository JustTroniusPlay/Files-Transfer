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
int pos_finder(const string&);
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

int pos_finder(const string& str)
{
    int index = 0;
    for(int i = str.length() - 1; i > 0; i--)
    {
        if(str[i] == '/' && !(i + 1 > str.length()))
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

            int index = pos_finder(str);
            string tmp = str.substr(index, str.length() - 1);
            for(int ch_ind = 0; ch_ind < 10; ch_ind++)
            {
                std::replace(tmp.begin(), tmp.end(), proh_symb[ch_ind], '_');
            }
            string to_save = path_to_dir + "\\" + tmp;
            const char* to_dir = to_save.c_str();

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
    int added_file;

    if(simult > list.size())
    {
        simult = list.size();
    }
    int lenght = list.size();
    int url_index = 0;
    while(lenght != 0)
    {
        if(simult > lenght)
        {
            simult = lenght;
        }
        
        for(int i_curl = 0; i_curl < simult; i_curl++)
        {
            curl_arr[i_curl] = curl_easy_init();
            if(curl_arr[i_curl])
            {
                const char* url = list[url_index].c_str();
                
                curl_easy_setopt(curl_arr[i_curl], CURLOPT_URL, url);
                curl_easy_setopt(curl_arr[i_curl], CURLOPT_WRITEFUNCTION, write_data);

                int index = pos_finder(list[url_index]);
                string tmp = list[url_index].substr(index, list[url_index].length() - 1);
                for(int ch_ind = 0; ch_ind < 10; ch_ind++)
                {
                    std::replace(tmp.begin(), tmp.end(), proh_symb[ch_ind], '_');
                }
                string to_save = path_to_dir + "\\" + tmp;
                const char* to_dir = to_save.c_str();
                url_index++;

                FILE *file = fopen(to_dir, "wb");
                if(!file)
                {
                    fclose(file);
                    url = NULL;
                    to_dir = NULL;
                    multi_clear(pCurl_Arr, &multi, simult);
                    curl_multi_cleanup(multi);
                    curl_global_cleanup();
                    string err = "ERROR: Path to destination directory \"" + path_to_dir + "\" is invalid to continue. Possible cause: directory is forbbiden for use.";
                    throw err; 
                }
                curl_easy_setopt(curl_arr[i_curl], CURLOPT_WRITEDATA, file);
                curl_multi_add_handle(multi, curl_arr[i_curl]);
                added_file++;
                cout << "File \"" << tmp << "\" successfuly added to download." << endl;
                fclose(file);
                url = NULL;
                to_dir = NULL;
            }
            else
            {
                cout << "ERROR: connection failed for Curl." << endl;
            }
        }

        cout << "Start fetching added files. . ." << endl;

        int running_handles;
        struct CURLMsg *msg;
        int msgq = 0;
        do
        {
            CURLMcode mc = curl_multi_perform(multi, &running_handles);
            while((msg = curl_multi_info_read(multi, &msgq)) != NULL)//Why this is not working???
            {
                if(msg->msg == CURLMSG_DONE)
                {
                    CURL *e = msg->easy_handle;
                    char *r_url;
                    curl_easy_getinfo(e, CURLINFO_PRIVATE, &r_url);
                    cout << "File on URL: \"" << r_url << "\" was downloaded with result \'" << curl_easy_strerror(msg->data.result) << "\'!" << endl;
                    curl_easy_cleanup(e);
                }
            }
          
            if(mc == CURLM_OK)
            {
                cout << "End of current que." << endl << endl;
                break;
            }
            else
            {
                cout << "Download for this que failed with code " << (int)mc << "!" << endl << endl;
            }
        } while(running_handles);

        multi_clear(pCurl_Arr, &multi, simult);

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
    int clock_begin, clock_end;
    clock_begin = clock();
    cout << "File Transfer. Starting programm. . ." << endl \
    << "Please enter: The path to file with URLs; Path to directory; Number of files to download simultaneously:" << endl;
    
    string pth;
    string ptd;
    int fl_amt;
    
    cin >> pth >> ptd >> fl_amt;
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

    clock_end = clock();

    cout << "End of programm. . ." << endl << "Programm worked for: " << diffclock(clock_end, clock_begin) << "ms." << endl;

    return 0;
}