#配置头文件搜索路径
#include_directories()
#配置库文件搜索路径
#link_directories()

#设置需要编译的源文件列表
set(SRC_LIST muduo_server.cpp)
#把指定路径下的所有源文件名字放入变量名SRC_LIST里面
#aux_source_directory(. SRC_LIST)

#设置可执行文件最终的存储路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
#设置库文件最终的存储路径
set(LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

#生成可执行文件
add_executable(muduo_server ${SRC_LIST})

#添加动态库和静态库
add_library(server_shared SHARED ${SRC_LIST})
add_library(server_static STATIC ${SRC_LIST})
#重新命名动态库和静态库
SET_TARGET_PROPERTIES(server_static PROPERTIES OUTPUT_NAME "server")
SET_TARGET_PROPERTIES(server_shared PROPERTIES OUTPUT_NAME "server")

#muduo_server这个目标程序，需要链接muduo_net muduo_base pthread这三个库文件
target_link_libraries(muduo_server muduo_net muduo_base pthread)

MESSAGE("cmake done........")
