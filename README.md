# Files Transfer
Small Project for downloading files from the web and redirecting them to disk space on your PC throught console. 

Work In Progress. This project essentially just personal programming training and not something really big or very serious.

!!! STILL NOT WORK, so do not bother to install for now, but I will appreciate if you want to help with advice.

## Overview, general idea and planned features.

Idea of this program is pretty simple: you can use it in console to download files from sources, that listed in one file, and load them to choosen catalogue. 

Program will take three inputs(separated by space):

1. Path to your file with links.
2. Path to catalogue.
3. Number of files that will be downloaded in parallel(MIN=1, MAX=999).

Only for WINDOWS.
## Installation

For this programm to be installed you need:
1) [libcurl](https://curl.se/libcurl/).
2) [MbedTLS](https://github.com/Mbed-TLS/mbedtls).(!!!!YOU NEED TO BUILD THIS BEFORE libcurl!!!!)
3) [CMake](https://cmake.org/) to compile and build other libs.
4) [MinGW64](https://www.mingw-w64.org/downloads/#mingw-builds) compilers.

Download MinGW64 first.

After downloading libraries you can use CMake tool to build them. If you don't know how to do that, I am not gonna do full guide here, BUT...
You can watch this [video](https://youtu.be/H1naJEHsxbQ?si=NF3JLHCiIbm1qRF9) I used as reference and/or follow instructions below: 
1) Unpack downloaded archive that contain source code(you can rename it "src" for convenience).
2) Place source code where you like and create folder called "build" in the same directory.
3) Open CMake, paste path to source code, than to build folder.
4) Click "Configure" and choose "Specify native compilers". If everything correct programm will find needed compilers. Click "Finish" and wait.
5) Find "CMAKE_INSTALL_PREFIX" in CMake gui and paste there a path to folder where you want to compiled library to be.
    *! For libcurl find another thing called "CURL_USE_MBEDTLS" and click checkbox there so libcurl can use mbedtls.
7) Click "Configure" and than "Generate".
8) Open GitBash in "build" folder and use next commands:
   * mingw32-make
   * mingw32-make install
   After that in directory on step 5) you will see compiled library.
9) After building MbedTLS repeat 1-7 steps for libcurl.
10) Go to Environment Variables and find CMAKE_PREFIX_PATH(or create if it not exist) and put there a paths to your compiled libraries folders.
11) Than still here in EV find Path. Find "bin" folders in compiled libraries folders and paste them in Path.
12) Than find "cmake" in "lib" folder in your compiled libcurl folder. Open file "CURLTargets.cmake" and find this line:
    add_library(CURL::libcurl SHARED IMPORTED)
    Copy "CURL::libcurl" or sinilar thing if it slightly changed to libcurl_shared.
13) Download files of this rep, save them somewhere, open file CMakeLists.txt and find "target_link_libraries(a CURL::libcurl)". If "CURL::libcurl" different from what you copied just replace it in this file.
14) Open GitBush in this folder and use next commands:
    * cmake -G"MinGW Makefile" .
    * "make" or "mingw32-make"
15) Add path to FilesTransfer.exe to Environment Variables in Path so you can call programm in console whenever you want.





 
