cmake_minimum_required(VERSION 3.4.1)
#-fstack-protector

add_library(gles3jni SHARED gles3jni.c)
target_link_libraries(gles3jni GLESv3 android EGL log m)
set_target_properties(gles3jni PROPERTIES LINKER_LANGUAGE C)
target_compile_options(gles3jni PRIVATE -fstack-protector)