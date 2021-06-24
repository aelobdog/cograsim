CMAKE_MINIMUM_REQUIRED(VERSION 3.1)

OPTION(Debug "Set to ON to compile with debug symbols" OFF)
OPTION(G "Set to ON to compile with optimisations and debug symbols" OFF)
OPTION(LR_TETRA "Enable povray output for DNA tetramers" OFF)
OPTION(NOPNG "Disable png support" OFF)

PROJECT(cogli2)

# c++-11 support is mandatory
IF(CMAKE_VERSION VERSION_LESS "3.1")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
ELSE()
	SET(CMAKE_CXX_STANDARD 11)
	SET(CMAKE_CXX_STANDARD_REQUIRED ON)
ENDIF()

# The default behaviour is to compile with optimisations
SET(CMAKE_BUILD_TYPE Release)

IF(Debug)
	SET(CMAKE_BUILD_TYPE Debug)
	MESSAGE(STATUS "Compiling in Debug mode")
ENDIF(Debug)

IF(G)
	SET(CMAKE_BUILD_TYPE RelWithDebInfo)
	MESSAGE(STATUS "Compiling in Release+Debug symbols mode")
ENDIF(G)

IF(LR_TETRA)
	ADD_DEFINITIONS(-DLR_TETRA)
ENDIF(LR_TETRA)

IF(NOT CMAKE_BUILD_TYPE MATCHES Debug)
	IF(INTEL)
		ADD_DEFINITIONS(-fast)
	ELSE()
		ADD_DEFINITIONS(-ffast-math)
	ENDIF(INTEL)
ENDIF()

# get the current svn version, if svn is installed. Avoid warnings if it isn't
FIND_PACKAGE(Subversion)
IF(Subversion_FOUND)
	EXECUTE_PROCESS(COMMAND "svnversion" WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} OUTPUT_VARIABLE revision OUTPUT_STRIP_TRAILING_WHITESPACE)
ELSE()
	SET(revision SVN_NOT_FOUND)
ENDIF()

# return the current date as day/month/year
MACRO(CURRENT_DATE result)
    IF(WIN32)
        EXECUTE_PROCESS(COMMAND "cmd" " /C date /T" OUTPUT_VARIABLE ${result})
        string(REGEX REPLACE "(..)/(..)/..(..).*" "\\1/\\2/\\3" ${result} ${${result}})
    ELSEIF(UNIX)
        EXECUTE_PROCESS(COMMAND "date" "+%d/%m/%Y" OUTPUT_VARIABLE ${result})
        string(REGEX REPLACE "(..)/(..)/..(..).*" "\\1/\\2/\\3" ${result} ${${result}})
    ELSE(WIN32)
        MESSAGE(SEND_ERROR "date not implemented")
        SET(${result} 000000)
    ENDIF (WIN32)
ENDMACRO(CURRENT_DATE)

CURRENT_DATE(today)
ADD_DEFINITIONS(-D'SVN_VERSION="${revision}"' -D'BUILD_TIME="${today}"')
ADD_COMPILE_OPTIONS(-Wall)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

if(WIN32)
	set(LIBS glfw3 opengl32)
elseif(UNIX AND NOT APPLE)
  	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
	find_package(OpenGL REQUIRED)
	add_definitions(${OPENGL_DEFINITIONS})
	find_package(X11 REQUIRED)
	# note that the order is important for setting the libs
	# use pkg-config --libs $(pkg-config --print-requires --print-requires-private glfw3) in a terminal to confirm
	set(LIBS ${GLFW3_LIBRARY} X11 Xrandr Xinerama Xi Xxf86vm Xcursor GL dl pthread)
	set (CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -ldl")
elseif(APPLE)
	INCLUDE_DIRECTORIES(/System/Library/Frameworks)
	FIND_LIBRARY(COCOA_LIBRARY Cocoa)
	FIND_LIBRARY(OpenGL_LIBRARY OpenGL)
	FIND_LIBRARY(IOKit_LIBRARY IOKit)
	FIND_LIBRARY(CoreVideo_LIBRARY CoreVideo)
	MARK_AS_ADVANCED(COCOA_LIBRARY OpenGL_LIBRARY)
	SET(APPLE_LIBS ${COCOA_LIBRARY} ${IOKit_LIBRARY} ${OpenGL_LIBRARY} ${CoreVideo_LIBRARY})
	SET(APPLE_LIBS ${APPLE_LIBS} ${GLFW3_LIBRARY} ${ASSIMP_LIBRARY})
	set(LIBS ${LIBS} ${APPLE_LIBS})
else()
	set(LIBS )
endif(WIN32)

# BSpline lib
add_subdirectory(Utils/BSpline)

# need this to make it possible to use <glm/glm.hpp>
include_directories(.)

# GLFW
add_subdirectory(glfw)
include_directories(glfw/include)

# GLAD
add_library(GLAD "glad/src/glad.c")
set(LIBS ${LIBS} GLAD)
include_directories(glad/include)

find_package(Freetype)
include_directories(${FREETYPE_INCLUDE_DIRS})

# BEGIN TO COMPILE STUFF
IF(CMAKE_BUILD_TYPE MATCHES Debug)
	SET(exe_name cogli2_debug)
ELSE()
	SET(exe_name cogli2)
ENDIF()

SET(cogli2_SOURCES
	Drawables/Drawable.cpp
	Drawables/Scene.cpp
	Drawables/Cylinder.cpp
	Drawables/Icosahedron.cpp
	Drawables/Arrow.cpp
	Drawables/Sphere.cpp
	Drawables/DNA.cpp
	Drawables/DHS.cpp
	Drawables/PatchyParticle.cpp
	Drawables/Shape.cpp
	Drawables/Triangle.cpp
	Drawables/Group.cpp
	Drawers/Drawer.cpp
	Drawers/OGLDrawer.cpp
	Drawers/PovDrawer.cpp
	Parsers/Parser.cpp
	Parsers/MGLParser.cpp
	Parsers/DNAParser.cpp
	Parsers/ManfredoParser.cpp
	Parsers/IcoParser.cpp
	Parsers/NathanParser.cpp
	Parsers/PatchyParser.cpp
	Parsers/StarrParser.cpp
	Parsers/ShellParser.cpp
	Parsers/RBCParser.cpp
	Parsers/LevyParser.cpp
	Parsers/TEPParser.cpp
	Parsers/IoxDNAParser.cpp
	Utils/StringHelper.cpp
	Utils/Generic.cpp
	Utils/Logging.cpp
	Utils/MathHelper.cpp
	OGL/OGLManager.cpp
	OGL/Shader.cpp
	OGL/Shapes/Mesh.cpp
	OGL/Shapes/OGLSphere.cpp
	OGL/Shapes/OGLCylinder.cpp
	OGL/Shapes/OGLIcosahedron.cpp
	OGL/Shapes/OGLCone.cpp
	Actions/ActionBuilder.cpp
	Actions/BaseAction.cpp
	Actions/Rotation.cpp
	Actions/Translation.cpp
	Actions/Spin.cpp
	povray/PovManager.cpp
	povray/DNARibbon.cpp
	Camera.cpp
	Lighting.cpp
	Cogli2.cpp
	main.cpp
)

ADD_EXECUTABLE(${exe_name} ${cogli2_SOURCES})
TARGET_LINK_LIBRARIES(${exe_name} glfw ${GLFW_LIBRARIES} BSpline ${CMAKE_DL_LIBS} ${LIBS})

include(CMakeRC.cmake)
SET(Assets_SOURCES
	OGL/Assets/Shaders/axes.fs
	OGL/Assets/Shaders/axes.vs
	OGL/Assets/Shaders/shape.fs
	OGL/Assets/Shaders/shape.vs
)

cmrc_add_resource_library(assets ${Assets_SOURCES})
TARGET_LINK_LIBRARIES(${exe_name} assets)

# if libpng is installed then we enable exporting in png format
FIND_PACKAGE(PNG)
IF(PNG_FOUND AND NOT NOPNG)
	ADD_DEFINITIONS(-DENABLE_PNG_EXPORT)
	MESSAGE(STATUS "Compiling with libpng support enabled")
	
	TARGET_LINK_LIBRARIES(${exe_name} ${PNG_LIBRARY})
ELSE()
	MESSAGE(STATUS "Compiling with no libpng support")
ENDIF()
