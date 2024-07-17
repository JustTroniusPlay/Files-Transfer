# Files Transfer
Small Project for downloading files from the web and redirecting them to disk space on your PC throught console. 

Work In Progress. This project essentially just programming training and not something really big or very serious.

## Overview, general idea and planned features.

Idea of this program is pretty simple: you can use it in console to download files from sources, that listed in one file, and load them to choosen catalogue. 


Program will take three inputs(separated by space):

1. Path to your file with links.
2. Path to catalogue.
3. Number of files that will be downloaded in parallel(MIN=1, MAX=999).

## Installation
To compile this program you need:
1) [CMake](https://cmake.org/).
2) [libcurl](https://curl.se/libcurl/).
3) [mbedTLS](https://github.com/Mbed-TLS/mbedtls?ysclid=lyojcgpxrv612954469)(! ! ! need to be compiled before libcurl! ! !).
4) Utility 'make' (I personaly used make command available throught MinGW).
5) [MinGW](https://sourceforge.net/projects/mingw/)

First step is to download and build needed libraries. 
Than in directory with source code open PowerShell/CMD/GitBash and use command 'cmake -G"MinGW Makefiles" .'.

After cmake will creat necessary files use 'make' or 'mingw32-make' to create .exe file with working program.

Youn can add path to this .exe in environmental variables for it to be always available in your console.

## Plans.

Right now this project still in development, most of basic functionality available, but still need some tweaking, maybe code rewriting and making it more readable/optimized. 
 