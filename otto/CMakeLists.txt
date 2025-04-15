cmake_minimum_required(VERSION 2.6.4)
project(CFS_OTTO_APP C)

include_directories(fsw/public_inc)
include_directories(fsw/mission_inc)
include_directories(fsw/platform_inc)
include_directories(fsw/src)

include_directories(${hwlib_MISSION_DIR}/fsw/public_inc)

aux_source_directory(fsw/src APP_SRC_FILES)

# Create the app module
add_cfe_app(otto ${APP_SRC_FILES})

target_include_directories(otto PUBLIC fsw/inc)

add_cfe_app_dependency(otto osal)
