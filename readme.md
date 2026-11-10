# cpp-starter

CMake + vcpkg 로 시작하는 C++ 프로젝트 템플릿.

- C++20 기준 (`vcpkg.json` 의 `$cpp-version` 으로 변경 가능)
- 라이브러리 1개 + 실행파일 1개 + GoogleTest 테스트 구조
- 코드 스타일·네이밍은 STL 방식을 따른다
- 프로젝트 이름·버전·C++ 버전은 **`vcpkg.json` 한 곳**에서 관리한다

---

# 1. 요구사항

| 항목 | 버전 | 비고 |
|---|---|---|
| CMake | 4.1 이상 | `cmake_minimum_required(VERSION 4.1)` |
| Ninja | 1.11 이상 | preset 기본 생성기 |
| vcpkg | manifest 모드 | `VCPKG_ROOT` 환경변수 필요 |
| 컴파일러 (macOS) | AppleClang | Xcode / Command Line Tools |
| 컴파일러 (Windows) | MSVC | Visual Studio 2022 이상 |

> Linux preset 은 아직 없다. 파일 접미사 규칙(`_linux`)은 이미 동작한다.

---

# 2. 시작하기

## 2-1. 새 프로젝트로 만들 때 고칠 곳

템플릿을 복사한 뒤 아래만 바꾸면 된다.

| 파일 | 키 | 예시 | 쓰이는 곳 |
|---|---|---|---|
| `vcpkg.json` | `name` | `cpp-starter` | `project()` 이름, 실행파일 이름, 테스트 이름(`<name>_test`) |
| `vcpkg.json` | `$package-name` | `starter` | 라이브러리 이름, `include/<이름>` 폴더, C++ namespace |
| `vcpkg.json` | `version-string` | `0.0.1` | `project(VERSION)`, `version.hpp`, Windows 리소스 |
| `vcpkg.json` | `$cpp-version` | `20` | C++ 표준 (`17`, `20`, `23`, `26` 만 허용) |
| `platform/win32/app.rc.in` | `CompanyName` 등 | `d3v-friends` | Windows exe 속성 정보 |
| `include/`, `source/`, `test/` | 하위 폴더 이름 | `starter` → 새 이름 | `$package-name` 과 같아야 함 |

주의
- `$package-name` 은 C++ namespace 로도 쓰이므로 **식별자 규칙**을 따라야 한다. (`my_app` ✅ / `my-app` ❌)
- `version-string` 은 `MAJOR.MINOR.PATCH` 세 자리로 쓴다.
- `$` 로 시작하는 키는 vcpkg 가 주석으로 취급하므로 vcpkg 동작에는 영향이 없다.

## 2-2. 로컬 preset 만들기 (CLion 용)

`CMakeUserPresets.json` 은 `.gitignore` 대상이라 저장소에 없다. 각자 PC 에서 아래 내용으로 만든다.
vcpkg 경로만 자기 환경에 맞게 고치면 된다.

```json
{
  "version": 10,
  "configurePresets": [
    { "name": "local-apple", "hidden": true,
      "environment": { "VCPKG_ROOT": "/Users/<user>/sdk/cpp/vcpkg" } },
    { "name": "local-win32", "hidden": true,
      "environment": { "VCPKG_ROOT": "C:/sdk/cpp/vcpkg" } },

    { "name": "local-apple-debug",   "inherits": ["apple-debug",   "local-apple"] },
    { "name": "local-apple-release", "inherits": ["apple-release", "local-apple"] },
    { "name": "local-win32-debug",   "inherits": ["win32-debug",   "local-win32"] },
    { "name": "local-win32-release", "inherits": ["win32-release", "local-win32"] }
  ],
  "buildPresets": [
    { "name": "local-apple-debug",   "configurePreset": "local-apple-debug" },
    { "name": "local-apple-release", "configurePreset": "local-apple-release" },
    { "name": "local-win32-debug",   "configurePreset": "local-win32-debug" },
    { "name": "local-win32-release", "configurePreset": "local-win32-release" }
  ],
  "testPresets": [
    { "name": "local-apple-debug", "configurePreset": "local-apple-debug", "output": { "outputOnFailure": true } },
    { "name": "local-win32-debug", "configurePreset": "local-win32-debug", "output": { "outputOnFailure": true } }
  ]
}
```

- 쉘에 `VCPKG_ROOT` 가 이미 있으면 이 파일 없이 공용 preset(`apple-debug` 등)을 바로 써도 된다.
- 공용 preset 과 **같은 이름**을 user 파일에 다시 정의하면 중복 에러가 난다. 그래서 `local-` 접두사를 쓴다.
- CLion: Settings → Build, Execution, Deployment → CMake 에서 `local-*` 프로필만 Enable 한다.

## 2-3. 빌드 / 테스트

```bash
# macOS Debug (테스트 포함)
cmake --preset local-apple-debug
cmake --build --preset local-apple-debug
ctest --preset local-apple-debug

# macOS Release
cmake --preset local-apple-release
cmake --build --preset local-apple-release
```

캐시가 꼬였을 때는 `--fresh` 를 붙여 다시 configure 한다. (CLion: Tools → CMake → Reset Cache and Reload Project)

```bash
cmake --preset local-apple-debug --fresh
```

---

# 3. Preset

| preset | 빌드 타입 | vcpkg triplet | 테스트 | 비고 |
|---|---|---|---|---|
| `apple-debug` | Debug | `arm64-osx` | ON | gtest 설치 (`tests` feature) |
| `apple-release` | Release | `arm64-osx` | OFF | |
| `win32-debug` | Debug | `x64-windows-static` | ON | 정적 CRT `/MTd` |
| `win32-release` | Release | `x64-windows-static` | OFF | 정적 CRT `/MT`, 콘솔창 없음 |

- 각 preset 은 `condition` 으로 호스트 OS 를 검사한다. mac 에서는 `win32-*` 가 목록에 안 보인다.
- 빌드 폴더: `build/<preset이름>/`
- `compile_commands.json` 이 자동 생성된다. (`CMAKE_EXPORT_COMPILE_COMMANDS=ON`)
- 테스트를 켜려면 **두 개를 같이** 켜야 한다. 하나만 켜면 `find_package(GTest)` 에서 실패한다.

```json
"cacheVariables": {
  "VCPKG_MANIFEST_FEATURES": "tests",
  "STARTER_BUILD_TESTS": "ON"
}
```

---

# 4. 폴더 구조

```
cpp-starter/
├── CMakeLists.txt
├── CMakePresets.json          # 공용 preset (git 포함)
├── CMakeUserPresets.json      # 로컬 preset (git 제외, 직접 생성)
├── vcpkg.json                 # 이름/버전/C++ 버전/의존성
├── vcpkg-configuration.json   # vcpkg baseline 고정
├── cmake/                     # CMake 함수 모음
│   ├── functions.cmake        #   아래 파일들을 한 번에 include
│   └── version.hpp.in         #   version.hpp 템플릿
├── include/<package-name>/    # 공개 헤더
├── source/<package-name>/     # 라이브러리 구현 + 비공개 헤더
├── source/app/                # 실행파일 (main.cpp)
├── test/<package-name>/       # 테스트
└── platform/win32/            # Windows 리소스 (app.rc.in, app.manifest, app.ico)
```

`include/<package-name>`, `source/<package-name>`, `source/app`, `test/<package-name>` 은 configure 할 때 없으면 자동으로 생성된다.

## 4-1. `include/<package-name>` — 공개 헤더

- 라이브러리 밖(app, 테스트, 다른 프로젝트)에서 include 하는 헤더만 둔다.
- include 할 때는 항상 패키지 이름을 붙인다.

```cpp
#include "starter/hello_world.hpp"   // ✅
#include "hello_world.hpp"           // ❌ 다른 라이브러리와 이름이 겹칠 수 있음
```

- 공개 헤더는 `source/` 의 비공개 헤더를 include 하면 안 된다. 필요하면 전방 선언이나 PIMPL 로 숨긴다.

## 4-2. `source/<package-name>` — 라이브러리 구현

- 공개 헤더의 구현체(`.cpp`)
- 비공개 헤더/소스는 hpp, cpp 를 같은 폴더에 둔다.
- 이 폴더의 파일은 전부 라이브러리(`<package-name>`)로 빌드된다. **`main()` 을 두면 안 된다.** (테스트의 `gmock_main` 과 충돌)

## 4-3. `source/app` — 실행파일

- `main()` 이 있는 곳. 라이브러리를 링크해서 실행파일(`<name>`)을 만든다.

## 4-4. `test/<package-name>` — 테스트

- GoogleTest 테스트 파일. `main()` 은 쓰지 않는다. (`GTest::gmock_main` 이 제공)
- 공개 API(`include/`)만 테스트하는 것을 원칙으로 한다.
- 테스트 이름은 PascalCase 로 짓는다. (`TEST(HelloWorld, SayHello)`)

```cpp
#include <gtest/gtest.h>               // TEST, EXPECT_*
#include <gmock/gmock.h>               // EXPECT_THAT, matchers
#include "starter/hello_world.hpp"

TEST(HelloWorld, SayHello) {
    starter::say_hello();
    SUCCEED();
}
```

## 4-5. 플랫폼별 파일 — 파일명 접미사 규칙

파일명이 `_(apple|win32|android|linux)` 로 끝나면 해당 플랫폼에서만 빌드에 포함된다.
접미사가 없으면 모든 플랫폼 공용이다.

```
source/starter/
├── file.cpp           # 공용
├── file_apple.cpp     # macOS / iOS 에서만
├── file_win32.cpp     # Windows 에서만
├── file_android.cpp   # Android 에서만
└── file_linux.cpp     # 그 외 (Linux)
```

| 판단 순서 | CMake 조건 | 접미사 |
|---|---|---|
| 1 | `APPLE` | `_apple` |
| 2 | `WIN32` | `_win32` |
| 3 | `ANDROID` | `_android` |
| 4 | 나머지 | `_linux` |

대상 확장자: `.cpp`, `.c`, `.hpp`, `.h`

---

# 5. 빌드 결과물 (타깃)

| 타깃 | 종류 | 이름 출처 | 소스 |
|---|---|---|---|
| `starter` (`starter::starter`) | 정적 라이브러리 | `$package-name` | `include/starter`, `source/starter` |
| `cpp-starter` | 실행파일 | `name` | `source/app` (라이브러리 링크) |
| `cpp-starter_test` | 테스트 실행파일 | `name` + `_test` | `test/` (라이브러리 + gmock_main 링크) |

모든 타깃에 공통으로 적용되는 설정 (`target_settings`)

| 항목 | MSVC | GCC / Clang |
|---|---|---|
| C++ 표준 | `cxx_std_<$cpp-version>` (PUBLIC, 링크한 쪽까지 전달) | 같음 |
| 경고 | `/W4 /permissive-` | `-Wall -Wextra -Wpedantic` |
| 소스 인코딩 | `/utf-8` | (기본 UTF-8) |
| Windows 전용 | `NOMINMAX`, `WIN32_LEAN_AND_MEAN`, `bcrypt` 링크 | — |

---

# 6. 자동 생성 파일

## 6-1. `version.hpp`

`vcpkg.json` 의 `version-string` 으로 configure 할 때 생성된다.

- 위치: `build/<preset>/generated/<package-name>/version.hpp`
- 템플릿: `cmake/version.hpp.in`

```cpp
#include <iostream>               // std::cout
#include "starter/version.hpp"    // starter::version

int main() {
    std::cout << "version " << starter::version << '\n';            // "0.0.1"
    static_assert(starter::version_major >= 0);                     // 컴파일 타임 사용 가능
}
```

| 이름 | 타입 | 예 |
|---|---|---|
| `version` | `std::string_view` | `"0.0.1"` |
| `version_major` / `version_minor` / `version_patch` | `int` | `0` / `0` / `1` |

## 6-2. Windows 리소스 (`release_win32`)

Windows 에서만 동작한다. (다른 OS 에서는 그냥 건너뜀)

| 항목 | 내용 |
|---|---|
| `app.rc.in` → `build/<preset>/generated/win32/<타깃>.rc` | 아이콘, 파일 버전 정보 |
| `app.manifest` | UTF-8 코드페이지 (`activeCodePage`, Windows 10 1903+) |
| Release 링크 옵션 | `/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup` → 콘솔창 없이 실행, `main()` 그대로 사용 |
| Debug | 콘솔창 유지 |

---

# 7. 패키지(의존성) 추가하기

## 7-1. vcpkg 패키지

1. `vcpkg.json` 의 `dependencies` 에 추가

```json
"dependencies": [ "glm", "fmt" ]
```

2. `cmake/package.cmake` 의 `get_package()` 안에서 찾고 목록에 넣기

```cmake
function(get_package _OUT_VAR)
    set(_PACKAGE "")

    find_package(glm CONFIG REQUIRED)
    find_package(fmt CONFIG REQUIRED)

    set(_PACKAGE
            glm::glm
            fmt::fmt
    )
    set(${_OUT_VAR} ${_PACKAGE} PARENT_SCOPE)
endfunction()
```

여기 넣은 패키지는 라이브러리에 **PUBLIC** 으로 링크되므로 app, 테스트에서도 그대로 쓸 수 있다.
타깃 이름(`glm::glm` 등)은 vcpkg 설치 로그의 `find_package` / `target_link_libraries` 안내를 따른다.

## 7-2. Git 저장소 (FetchContent)

```cmake
include(FetchContent)
FetchContent_Declare(
        ctls
        GIT_REPOSITORY https://github.com/d3v-friends/cpp-tools.git
        GIT_TAG v0.0.42
)
FetchContent_MakeAvailable(ctls)
list(APPEND _PACKAGE ctls::ctls)
```

## 7-3. 테스트 전용 패키지

테스트에서만 쓰는 패키지는 `vcpkg.json` 의 `features.tests.dependencies` 에 넣는다.
`tests` feature 가 켜진 preset(debug)에서만 설치된다.

---

# 8. CMake 함수 레퍼런스 (`cmake/`)

| 파일 | 함수 | 사용법 | 설명 |
|---|---|---|---|
| `json.cmake` | `get_json` | `get_json(<OUT> <KEY>... FILE <경로> [DEFAULT <값>])` | JSON 에서 문자열/숫자 값 읽기. 중첩 키는 나열 (`features tests description`) |
| `path.cmake` | `mkdir` | `mkdir(<절대경로>...)` | 폴더가 없으면 생성 (절대경로만) |
| `sources.cmake` | `get_sources` | `get_sources(<OUT> "<확장자 regex>" <폴더>...)` | 폴더에서 소스 수집 + 플랫폼 접미사 필터 |
| `package.cmake` | `get_package` | `get_package(<OUT>)` | 링크할 패키지 목록 |
| `execute.cmake` | `make_library` | `make_library(<OUT> LIB_NAME <이름> [PACKAGES ...] SOURCES ...)` | 라이브러리 + `<이름>::<이름>` alias 생성 |
| `execute.cmake` | `make_executable` | `make_executable(<OUT> EXE_NAME <이름> [PACKAGES ...] SOURCES ...)` | 실행파일 생성 |
| `target.cmake` | `target_settings` | `target_settings(<타깃>)` | C++ 표준, 경고, Windows 공통 설정 |
| `target.cmake` | `is_available_cpp_version` | `is_available_cpp_version(<버전>)` | `17/20/23/26` 이외면 FATAL |
| `generator.cmake` | `generate_version_file` | `generate_version_file(NAME <ns> TARGETS <타깃>...)` | `version.hpp` 생성 + include 경로 추가 (`project()` 뒤에서) |
| `release.cmake` | `release_win32` | `release_win32(<타깃> RC_IN <rc.in> MANIFEST <manifest>)` | Windows 리소스/매니페스트/서브시스템 |
| `test.cmake` | `run_test` | `run_test(<테스트이름> "<regex>" <대상라이브러리> <폴더>...)` | gtest 실행파일 생성 + ctest 등록 |

새 함수는 `cmake/` 에 파일을 만들고 `cmake/functions.cmake` 에 include 를 추가한다.

---

# 9. CMake 작성 규칙

## 9-1. 함수 파라미터·지역 변수는 `_` 접두사

CMake 함수 안에서는 바깥(호출한 쪽) 변수를 그대로 읽을 수 있다.
이름을 틀려도 에러 없이 바깥 값이나 빈 값이 들어가므로, 함수 안 변수는 `_` 를 붙여 구분한다.

```cmake
function(get_sources _OUT_VAR _FILE_REGEX)   # ✅ 파라미터
    set(_PATHS "")                           # ✅ 지역 변수
    ...
    set(${_OUT_VAR} ${_ALL} PARENT_SCOPE)    # 결과는 OUT_VAR + PARENT_SCOPE 로 내보냄
endfunction()
```

## 9-2. 결과는 `OUT_VAR` + `PARENT_SCOPE`

function 안에서 `set()` 한 변수는 함수 밖으로 나가지 않는다. 결과를 돌려줄 땐 첫 번째 인자로 변수 이름을 받는다.

## 9-3. 인자가 여러 종류면 `cmake_parse_arguments`

```cmake
#                      건너뛸개수 접두사  옵션  값1개        값여러개
cmake_parse_arguments(PARSE_ARGV 1 ARG   ""   "LIB_NAME"  "PACKAGES;SOURCES")

if (ARG_UNPARSED_ARGUMENTS)   # 오타 인자 잡기
    message(FATAL_ERROR "모르는 인자: ${ARG_UNPARSED_ARGUMENTS}")
endif ()
```

- 값이 1개인 키워드(이름 등)는 반드시 "값1개" 칸에 둔다. 넘치는 값을 `UNPARSED` 로 잡을 수 있다.

## 9-4. `enable_testing()` 은 최상위에서

함수 안에서 호출하면 효과가 없다. (`CTestTestfile.cmake` 가 비어서 ctest 가 테스트를 못 찾음)

## 9-5. 디버깅 팁

```bash
# 정의 안 된 변수를 읽는 곳마다 경고 → 변수명 오타 찾기
cmake --preset local-apple-debug --fresh --warn-uninitialized
```

---

# 10. 문제 해결

| 증상 | 원인 | 해결 |
|---|---|---|
| `Could not find a package configuration file provided by "GTest"` | 테스트는 켜졌는데 `tests` feature 가 꺼짐, 또는 vcpkg toolchain 이 안 붙음 | preset 으로 configure 했는지 확인. `VCPKG_MANIFEST_FEATURES=tests` + `STARTER_BUILD_TESTS=ON` 같이 켜기 |
| CLion 에서 GTest 못 찾음 | CLion 기본 프로필(`cmake-build-debug`)로 configure | CMake 설정에서 preset 프로필(`local-*`)만 Enable |
| `ctest` 에 `No tests were found` | `enable_testing()` 이 함수 안에 있음 / 테스트 소스 없음 | 최상위 `CMakeLists.txt` 에서 호출 |
| `TEST` 에서 `a type specifier is required` | 테스트 파일에 gtest 헤더 누락 | `#include <gtest/gtest.h>` |
| `Undefined symbols: _main` / `main` 중복 | `main()` 위치 오류 | `main()` 은 `source/app` 에만 |
| preset 목록이 안 뜸 / preset 로드 실패 | user 파일과 공용 파일에 같은 이름, 또는 없는 preset 참조 | user preset 은 `local-` 접두사, 참조 이름 확인 |
| 옵션을 바꿨는데 반영 안 됨 | `option()` 은 캐시 값이 있으면 기본값 무시 | `--fresh` 로 다시 configure |
| `$cpp-version` 에러 | `17/20/23/26` 이외 값 | `vcpkg.json` 값 수정 |
| Windows `LNK2038` 런타임 불일치 | static triplet 인데 `/MD` 로 빌드 | preset 의 `CMAKE_MSVC_RUNTIME_LIBRARY` 확인 |

---

# 11. 참고: 컴파일러 / C++ 모듈

- 크로스플랫폼은 **각 플랫폼 기본 컴파일러 + 표준 C++ 코드**로 간다. (mac: AppleClang, Windows: MSVC, Linux: GCC 14+)
- 표준 라이브러리 구현은 플랫폼마다 다르다. (libc++ / MS STL / libstdc++) 차이가 나면 `<version>` 의 기능 매크로로 분기한다.
- C++20 모듈은 AppleClang 에서 쓸 수 없다. (`clang-scan-deps` 미포함) mac 에서 실험하려면 Homebrew LLVM 을 컴파일러로 지정한 별도 preset 을 쓴다.

---

# 12. TODO

- [ ] Linux preset (`arm64-linux`, GCC 14)
- [ ] GitHub Actions CI (mac / linux / windows)
- [ ] ASan / UBSan preset
- [ ] `.clang-format`, `.editorconfig`
- [ ] install / export (`find_package(starter)`)

---

# License

MIT
