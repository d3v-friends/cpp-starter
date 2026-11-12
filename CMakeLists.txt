cmake_minimum_required(VERSION 4.1)

# 미리 생성한 함수 가져오기
include(cmake/functions.cmake)

# vcpkg.json 파일위치
set(VCPKG_JSON_PATH "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg.json")

# vcpkg.json 에서 기본정보 가저오기
get_json(VCPKG_NAME "name" FILE "${VCPKG_JSON_PATH}")
get_json(APP_VERSION "version-string" FILE "${VCPKG_JSON_PATH}")
get_json(PACKAGE_NAME "$package-name" FILE "${VCPKG_JSON_PATH}")
get_json(CPP_VERSION "$cpp-version" FILE "${VCPKG_JSON_PATH}")
is_available_cpp_version(${CPP_VERSION})


# 기본폴더 생성하기
mkdir("${CMAKE_CURRENT_SOURCE_DIR}/include/${PACKAGE_NAME}")
mkdir("${CMAKE_CURRENT_SOURCE_DIR}/source/${PACKAGE_NAME}")
mkdir("${CMAKE_CURRENT_SOURCE_DIR}/source/app")
mkdir("${CMAKE_CURRENT_SOURCE_DIR}/test/${PACKAGE_NAME}")

# 기본값
set(FILE_EXT "\\.(cpp|c|hpp|h)$")

# 프로젝트 기본설정
project(${VCPKG_NAME} VERSION ${APP_VERSION} LANGUAGES C CXX) # 프로젝트 이름 지정하기
set(CMAKE_CXX_STANDARD ${CPP_VERSION})
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 패키지 불러오기
set(PACKAGES "")
# vcpkg 에서 가저오기
#find_package(glm CONFIG REQUIRED)

#git 프로젝트에서 라이브러리 불러오기
#include(FetchContent)
#FetchContent_Declare(
#        ctls
#        GIT_REPOSITORY https://github.com/d3v-friends/cpp-tools.git
#        GIT_TAG v0.0.42
#)
#FetchContent_MakeAvailable(ctls)

# 변수에 라이브러리 목록 추가하기
# list(APPEND PACKAGES glm::glm ctls::ctls)


# 라이브러리 만들기
# cmake 에서 이름을 한번 지정하면 전부 외부 노출이 된다.
get_sources(
        LIB_SOURCES
        "${FILE_EXT}"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/${PACKAGE_NAME}"
        "${CMAKE_CURRENT_SOURCE_DIR}/source/${PACKAGE_NAME}"
)
make_library(
        LIB
        LIB_NAME ${PACKAGE_NAME}
        PACKAGES ${PACKAGES}
        SOURCES ${LIB_SOURCES}
)
target_settings(${LIB})


# 앱 만들기
get_sources(
        EXE_SOURCES
        "${FILE_EXT}"
        "${CMAKE_CURRENT_SOURCE_DIR}/source/app"
)
make_executable(
        EXE
        EXE_NAME ${VCPKG_NAME}
        SOURCES ${EXE_SOURCES}
        PACKAGES ${LIB}
)
target_settings(${EXE})
release_win32(
        ${EXE}
        RC_IN "${CMAKE_CURRENT_SOURCE_DIR}/platform/win32/app.rc.in"
        MANIFEST "${CMAKE_CURRENT_SOURCE_DIR}/platform/win32/app.manifest"
)

# version generate
generate_version_file(
        NAME ${PACKAGE_NAME}
        TARGETS ${LIB} ${EXE}
)

# header generate
generate_header(
        OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/include/${PACKAGE_NAME}/${PACKAGE_NAME}.hpp"
        DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/${PACKAGE_NAME}"
        BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include"
)


# test
# CMakeUserPresets.json 에서 관리한다
# "cacheVariables": {
#    "BUILD_TESTS": "ON"
#  }
option(BUILD_TESTS "테스트 빌드" OFF)
if (BUILD_TESTS)
    enable_testing()
    run_test(
            "${VCPKG_NAME}_test"
            "${FILE_EXT}"
            ${LIB}
            "${CMAKE_CURRENT_SOURCE_DIR}/test"
    )
endif ()
