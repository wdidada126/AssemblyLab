cmake_minimum_required(VERSION 3.10)
project(VideoConferenceDemo_nasm C ASM_NASM)

set(CMAKE_C_STANDARD 11)

enable_language(ASM_NASM)

set(CMAKE_ASM_NASM_OBJECT_FORMAT elf64)

include_directories(${CMAKE_SOURCE_DIR})

set(ASM_FILES
    video_processing.asm
)

set(C_FILES
    main.c
    video_capture.c
    image_processing.c
    audio_processing.c
    network.c
)

add_executable(video_demo ${C_FILES} ${ASM_FILES})

set_target_properties(video_demo PROPERTIES
    LINKER_LANGUAGE C
    POSITION_INDEPENDENT_CODE ON
)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(video_demo m)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    target_link_libraries(video_demo m)
    set_property(TARGET video_demo APPEND_STRING PROPERTY LINK_FLAGS " -no_warn_unused_flag")
endif()
