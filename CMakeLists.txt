cmake_minimum_required(VERSION 3.3.0)
project(Squirrel)
add_custom_command(
	OUTPUT ${PROJECT_BINARY_DIR}/icon.ico
	COMMAND ${IMAGEMAGICK_PATH}magick convert ${PROJECT_SOURCE_DIR}/res/icon.svg ${PROJECT_BINARY_DIR}/icon.ico
	DEPENDS ${PROJECT_SOURCE_DIR}/res/icon.svg
)
add_custom_command(
	OUTPUT ${PROJECT_BINARY_DIR}/icon-en.ico
	COMMAND ${IMAGEMAGICK_PATH}magick convert ${PROJECT_SOURCE_DIR}/icon-en.svg ${PROJECT_BINARY_DIR}/icon-en.ico
	DEPENDS ${PROJECT_SOURCE_DIR}/icon-en.svg
)
configure_file(Dll.rc.in Dll.rc)
add_library(Squirrel SHARED Squirrel.cpp SquirrelLangBarItemButton.cpp SquirrelFactory.cpp Dll.cpp Dll.def ${PROJECT_BINARY_DIR}/Dll.rc util.cpp PhoneticCombination.cpp CandidateWindow.cpp KeyState.cpp ${PROJECT_BINARY_DIR}/icon.ico ${PROJECT_BINARY_DIR}/icon-en.ico)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
add_definitions(/D_UNICODE=1 /DUNICODE=1)
