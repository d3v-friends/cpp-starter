# Prepare

1. cmake
2. clang
3. vcpkg

# CLion

- Settings > File > Settings on Windows and Linux, CLion > Preferences
- CMake Options

# vcpkg

1. package 설치

~~~
vcpkg install [nm]
~~~

2. CMakeLists.txt 에 추가

~~~
find_package(unofficial-nana CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE unofficial::nana::nana)
~~~

~~~
-DCMAKE_TOOLCHAIN_FILE=[vcpkg root]/scripts/buildsystems/vcpkg.cmake

# cmake .. -DCMAKE_TOOLCHAIN_FILE=/home/ciaolee/cpp_libs/vcpkg/scripts/buildsystems/vcpkg.cmake
~~~ 