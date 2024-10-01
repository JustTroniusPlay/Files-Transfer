#include "lib.h"

void multi_clear(CURL** curl_arr, CURLM* multi, int save_amount)//to clear all curl handless from multi
{
    for(int id_download = 0; id_download < save_amount; id_download++)
    {
        curl_multi_remove_handle(multi, &curl_arr[id_download]);
        curl_easy_cleanup(&curl_arr[id_download]);
    }
}

static double diffclock(clock_t clock1,clock_t clock2)//to count how much time program take to work
{
    double diffticks=clock1-clock2;
    double diffms=(diffticks)/(CLOCKS_PER_SEC/1000);
    return diffms;
}

string dir_creator(string&path_to_dir)//to create new directory or to use one that already exist
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
    cout << endl;
    return path_to_dir;
}

int pos_finder(const string& str, char symbol)//to find position of choosen symbol in string
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

void reader(string&path_to_file, vector<string>&list)//read path to file with links
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

string to_dir_checked(const string&str, const string&path_to_dir)//to make valid path to file 
{
    int index = pos_finder(str, '/');
    char proh_symb[] = {'\\', '/', ':', '*', '?', '\"','<', '>', '|', '+'};//prohibited symbols for creating a file in Windows
    string file_name = str.substr(index, str.length() - 1);

    for(int ch_ind = 0; ch_ind < 10; ch_ind++)
    {
        std::replace(file_name.begin(), file_name.end(), proh_symb[ch_ind], '_');
    }
    string to_save = path_to_dir + "\\" + file_name;
    const char* to_dir = to_save.c_str();

    FILE *check = fopen(to_dir,"rb");//to understand if we need to create a copy
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
    fclose(check);
    return to_save;
}

/*void saver_(string&path_to_dir,  const vector<string>&list)//individual file download. Saved just for history
{ 
    curl_global_init(CURL_GLOBAL_ALL);
    CURL *curl = curl_easy_init();
    path_to_dir = dir_creator(path_to_dir); 

    FILE *file;
    char errbuffer[CURL_ERROR_SIZE+1] = {};
   
    if(curl)
    {
        for(string str : list)
        {
            const char* url = str.c_str();
            string to_save = to_dir_checked(str, path_to_dir);
            const char* to_dir = to_save.c_str();

            file = fopen(to_dir, "wb");
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

            curl_easy_setopt(curl, CURLOPT_URL, url);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
            curl_easy_setopt(curl, CURLOPT_CAINFO, "./ca-bundle.crt");
            curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errbuffer);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
            //curl_easy_setopt(curl, CURLOPT_VERBOSE, true);//additional info about connection

            cout << "Start fetching file. . ." << endl;
            CURLcode res = curl_easy_perform(curl); 
            if (res != CURLE_OK) 
            {
                cout << "Download of file: " << endl << "\"" << url << "\"" << endl << "...failed with error:" << endl << "\'" << curl_easy_strerror(res) <<"\'" << endl;
                cout << "Errors buffer result: " << endl << errbuffer << endl;
                cout << "Moving to next URL. . ." << endl << endl;
            }
            else
            {
                cout << "Download of file \"" << url << "\" was succsesful!" << endl;
                cout << "Moving to next URL. . ." << endl << endl;
            }
            //index = 0;
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
    fclose(file);
    curl_global_cleanup();
}*/

void saver(string&path_to_dir, const vector<string>& list, int simult)//for multi download
{
    curl_global_init(CURL_GLOBAL_ALL);

    path_to_dir = dir_creator(path_to_dir);
    CURLM *multi = curl_multi_init();
    curl_multi_setopt(multi, CURLMOPT_MAXCONNECTS, simult);

    CURL *curl_arr[999];
    FILE *files[999];
    CURL **pCurl_Arr = curl_arr;
    int added_file = simult;
    char errbuffer[CURL_ERROR_SIZE+1] = {};
    char *errbuffers[999];

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
        for(int id_download = 0; id_download < simult; id_download++)
        {
            curl_arr[id_download] = curl_easy_init();
            if(curl_arr[id_download])
            {
                const char* url = list[url_index].c_str();
                string to_save = to_dir_checked(list[url_index], path_to_dir);
                const char* to_dir = to_save.c_str();

                files[id_download] = fopen(to_dir, "wb");
                if(!files[id_download])
                {
                    for(int f_id = 0; f_id < url_index; f_id++)
                    {  
                        fclose(files[f_id]);
                    }
                    url = NULL;
                    to_dir = NULL;
                    multi_clear(pCurl_Arr, &multi, simult);
                    curl_multi_cleanup(multi);
                    curl_global_cleanup();
                    string err = "ERROR: Path to destination directory \"" + path_to_dir + "\" is invalid to continue. Possible cause: directory is forbbiden for use.";
                    throw err; 
                }

                curl_easy_setopt(curl_arr[id_download], CURLOPT_URL, url);
                curl_easy_setopt(curl_arr[id_download], CURLOPT_WRITEFUNCTION, write_data);
                errbuffers[id_download] = errbuffer;
                curl_easy_setopt(curl_arr[id_download], CURLOPT_ERRORBUFFER, errbuffers[id_download]);
                curl_easy_setopt(curl_arr[id_download], CURLOPT_CAINFO, "./ca-bundle.crt");
                curl_easy_setopt(curl_arr[id_download], CURLOPT_WRITEDATA, files[id_download]);
                curl_easy_setopt(curl_arr[id_download], CURLOPT_HEADER, 0L);
                //curl_easy_setopt(curl_arr[id_download], CURLOPT_VERBOSE, true);//for adittional debug info
                curl_multi_add_handle(multi, curl_arr[id_download]);
                cout << "File \"" << url << "\" successfuly added to download." << endl;
                url = NULL;
                to_dir = NULL;
                url_index++;
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
        cout << endl << "Start fetching added files. . ." << endl;

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
        int buffer_id = 0;
        do
        {
            msg = curl_multi_info_read(multi, &msgq);
            if(msg && (msg->msg == CURLMSG_DONE))
            {
                char *r_url;
                curl_easy_getinfo(msg->easy_handle, CURLINFO_EFFECTIVE_URL, &r_url);
                cout << "Download for URL: \"" << r_url << "\""  << endl << "...ended with:" << endl << "\'" << curl_easy_strerror(msg->data.result) << "\'" << endl;
                cout << "Errors buffer result: " << endl << errbuffers[buffer_id] << endl << endl;
                
                buffer_id++;
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
    for(int f_id = 0; f_id < list.size(); f_id++)
    {  
        fclose(files[f_id]);
    }
    curl_multi_cleanup(multi);
    curl_global_cleanup();
}

// Callback function to handle downloaded data
static size_t write_data(void *contents, size_t size, size_t nmemb, void *stream) 
{
    size_t written = fwrite(contents, size, nmemb, (FILE *)stream);
    if(ferror((FILE *)stream))
    {
        cout << "Writing to file was not succesful." << endl;
    }
    return written;
}