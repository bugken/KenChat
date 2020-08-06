#把指定路径下的所有源文件名字放入变量名SRC_LIST里面
aux_source_directory(. SRC_LIST)
aux_source_directory(../ PUBLIC_LIST)

#指定生成可执行文件
add_executable(ChatClient ${SRC_LIST} ${PUBLIC_LIST})

#指定可执行文件链接时需要的库文件
target_link_libraries(ChatClient pthread)