cmake_minimum_required(VERSION 3.0)
project(files_transfer)
find_package(CURL REQUIRED)

add_executable(FilesTransfer main.cpp)

target_link_libraries(FilesTransfer CURL::libcurl_shared)
