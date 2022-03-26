# I assume you want to use LibProject as a library in MainProject.
include_directories(${PR_LAB2_SOURCE_DIR}/src/include)

add_executable(main main.c list.c workers.c)