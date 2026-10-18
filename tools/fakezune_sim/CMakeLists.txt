cmake_minimum_required(VERSION 3.16)
project(HighBoyFakeZuneSim C)

# Headless harness for the High Boy Fake Zune front end.
#
# Compiles the REAL boot screen, home hub and screen manager (ui_manager.c) from
# firmware_p4 against LVGL on the host, with the hardware services stubbed out,
# then drives them with scripted key presses and dumps PNG-able frames.
# It needs no ESP-IDF, SDL or hardware.

set(CMAKE_C_STANDARD 11)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(REPO_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../..")
set(P4_ROOT "${REPO_ROOT}/firmware_p4")
set(UI_ROOT "${P4_ROOT}/components/Applications/ui")
set(HLE_INCLUDE_DIR "${REPO_ROOT}/tools/hle/include")

file(GLOB_RECURSE P4_INCLUDE_DIRS LIST_DIRECTORIES true CONFIGURE_DEPENDS
    "${P4_ROOT}/components/*/include")
set(P4_INCLUDES "")
foreach(_d ${P4_INCLUDE_DIRS})
    if(IS_DIRECTORY ${_d})
        list(APPEND P4_INCLUDES ${_d})
    endif()
endforeach()

include(FetchContent)
set(LV_BUILD_CONF_DIR "${HLE_INCLUDE_DIR}" CACHE PATH "Directory containing lv_conf.h" FORCE)
set(CONFIG_LV_BUILD_DEMOS OFF CACHE BOOL "" FORCE)
set(CONFIG_LV_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(CONFIG_LV_USE_THORVG_INTERNAL OFF CACHE BOOL "" FORCE)
FetchContent_Declare(lvgl GIT_REPOSITORY https://github.com/lvgl/lvgl.git GIT_TAG v9.4.0)
FetchContent_MakeAvailable(lvgl)

add_executable(fakezune_sim
    src/main.c
    src/stubs.c
    ${UI_ROOT}/ui_manager.c
    ${UI_ROOT}/screens/boot/boot_ui.c
    ${UI_ROOT}/screens/home/home_ui.c
)
# Shim headers (esp_log.h, freertos/...) come from the HLE tree; the UI headers
# come straight from firmware_p4 so a signature change breaks this build.
target_include_directories(fakezune_sim PRIVATE ${HLE_INCLUDE_DIR} ${P4_INCLUDES})
target_include_directories(fakezune_sim SYSTEM PRIVATE ${lvgl_SOURCE_DIR} ${lvgl_SOURCE_DIR}/src)
target_compile_definitions(fakezune_sim PRIVATE HLE_BUILD)
target_compile_options(fakezune_sim PRIVATE -Wall -Wextra -Wno-unused-parameter
    -Wno-unused-variable -Wno-sign-compare -Wno-missing-field-initializers
    -Wno-implicit-function-declaration -fno-strict-aliasing)
# ui_manager.c names every screen's open function. Only the ones the harness
# provides are resolved; the rest bind to NULL, which ui_switch_screen reports
# as "screen not available" - the same path an unbuilt screen takes.
target_link_options(fakezune_sim PRIVATE -no-pie -Wl,--unresolved-symbols=ignore-all)
target_link_libraries(fakezune_sim PRIVATE lvgl pthread m)

enable_testing()
add_test(NAME fakezune_sim
    COMMAND fakezune_sim ${CMAKE_CURRENT_BINARY_DIR}/frames)
