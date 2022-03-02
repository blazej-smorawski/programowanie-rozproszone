cmake_minimum_required(VERSION 3.20)
project(pr_lab1 C)

set(CMAKE_C_STANDARD 11)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(pr_lab1 main.c)
add_executable(pr_lab1-pthread main-pthread.c)
add_executable(pr_lab1-pthread-bad main-pthread-bad.c)
target_link_libraries(pr_lab1 PRIVATE Threads::Threads)
