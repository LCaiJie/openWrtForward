#cmake最低版本需求，不加入此行会受到警告信息
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

#指定GCC的工具链
SET(CMAKE_C_COMPILER "mips-openwrt-linux-gcc")
SET(CMAKE_CXX_COMPILER "mips-openwrt-linux-g++")


#设置编译类型 Debug release
# set(CMAKE_BUILD_TYPE Debug)


#项目名称
PROJECT( HELLO ) 

#添加编译参数
# ADD_COMPILE_OPTIONS(-std=c11 -o2)

#把当前目录(.)下所有源代码文件和头文件加入变量SRC_LIST
AUX_SOURCE_DIRECTORY(. SRC_LIST)

#包含头文件的目录
INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/src/ownQueue"
                    "${PROJECT_SOURCE_DIR}/src/linkList")


#包含的目录
ADD_SUBDIRECTORY( src)

# 设置使用的模块
SET(EXTRA_LIBS OWM_QUEUE LINK_LIST)

#可执行文件的输出的路径
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

#生成应用程序 hello
ADD_EXECUTABLE(hello ${SRC_LIST})

#添加链接的共享库
TARGET_LINK_LIBRARIES( hello ${EXTRA_LIBS})

