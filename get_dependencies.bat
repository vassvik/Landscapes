@echo off 

rem Create file directories, and clean them up if they  exist

if exist "dependencies" rmdir /S /Q dependencies
mkdir dependencies

if not exist "include" mkdir include

if exist "include\imgui" rmdir /S /Q include\imgui
if exist "include\GLFW" rmdir /S /Q include\GLFW
if exist "include\stb" rmdir /S /Q include\stb
mkdir include\imgui
mkdir include\GLFW
mkdir include\stb

if not exist "lib" mkdir lib
if exist "lib\glfw3.lib" del lib\glfw3.lib
if exist "lib\imgui.lib" del lib\imgui.lib
if exist "lib\stb.lib"   del lib\stb.lib

rem Cet all dependencies, build and copy over files
pushd dependencies
    REM Clone repos
    git clone https://github.com/ocornut/imgui/
    git clone https://github.com/glfw/glfw
    git clone https://github.com/nothings/stb.git

    REM Build imgui
    pushd imgui
        cl /nologo /O2 /MP /MT /c *.cpp
        lib /nologo *.obj /out:imgui.lib
        copy imgui.lib ..\..\lib\imgui.lib
        copy *.h ..\..\include\imgui\
        del *.obj
    popd

    REM Build glfw
    pushd glfw
        mkdir build 
        pushd build 
            cmake -G "NMake Makefiles" .. -DGLFW_BUILD_EXAMPLES=OFF -DGLFW_BUILD_TESTS=OFF -DGLFW_BUILD_DOCS=OFF -DUSE_MSVC_RUNTIME_LIBRARY_DLL=OFF -DCMAKE_BUILD_TYPE=RELEASE
        popd
        cmake --build build --config Release
        copy build\src\glfw3.lib ..\..\lib\glfw3.lib
        copy include\GLFW\*.h ..\..\include\GLFW\
    popd

    REM Copy stb files
    pushd stb
        echo #define STB_IMAGE_IMPLEMENTATION > test.cpp
        echo #include "stb_image.h" >> test.cpp
        echo #define STB_IMAGE_WRITE_IMPLEMENTATION >> test.cpp
        echo #include "stb_image_write.h" >> test.cpp
        echo #define STB_RECT_PACK_IMPLEMENTATION >> test.cpp
        echo #include "stb_rect_pack.h" >> test.cpp
        echo #define STB_TRUETYPE_IMPLEMENTATION >> test.cpp
        echo #include "stb_truetype.h" >> test.cpp
        cl /nologo /O2 /MT /c test.cpp
        lib /nologo *.obj /out:stb.lib

        copy stb_truetype.h ..\..\include\stb\
        copy stb_image_write.h ..\..\include\stb\
        copy stb_rect_pack.h ..\..\include\stb\
        copy stb_image.h ..\..\include\stb\
    popd
popd