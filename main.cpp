#include "functions.cpp"

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