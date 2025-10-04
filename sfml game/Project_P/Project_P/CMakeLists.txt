# CMakeList.txt: проект CMake для Project_P; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}//bin")

set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc")
set(SFML_STATIC_LIBRARIES TRUE)
# Добавьте источник в исполняемый файл этого проекта.
add_executable (Project_P "GameEngine.cpp" "Player.cpp" "Animator.cpp" "Map.cpp" "Enemy.cpp" "../include/Headers/GameEngine.h" "../include/Headers/Player.h" "../include/Headers/Animator.h" "../include/Headers/Map.h" "../include/Headers/Enemy.h" "../include/Headers/Bullet.h" "../include/Headers/Sound.h" "Bullet.cpp" "Sound.cpp")

set(SFML_DIR "${CMAKE_SOURCE_DIR}//lib//cmake//SFML")
find_package(SFML COMPONENTS graphics audio window system REQUIRED)

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

add_custom_command(TARGET Project_P POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                   ${CMAKE_SOURCE_DIR}/openal32.dll
                   ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(Project_P sfml-system sfml-window sfml-graphics sfml-audio sfml-network)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Project_P PROPERTY CXX_STANDARD 20)
endif()

# TODO: Добавьте тесты и целевые объекты, если это необходимо.
