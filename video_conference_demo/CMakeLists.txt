cmake_minimum_required(VERSION 3.10)
project(VideoConferenceDemo C ASM_MASM)

set(CMAKE_C_STANDARD 11)

enable_language(ASM_MASM)

include_directories(${CMAKE_SOURCE_DIR})

set(SOURCES
    main.c
    video_capture.c
    image_processing.c
    audio_processing.c
    network.c
    video_processing.asm
)

add_executable(video_demo ${SOURCES})

set_target_properties(video_demo PROPERTIES
    LINKER_LANGUAGE C
)
