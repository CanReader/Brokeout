set(PROJECT_NAME Breakout)

################################################################################
# Source groups
################################################################################
set(no_group_source_files
    "res/projection.frag.glsl"
    "res/projection.vert.glsl"
    "res/spriteProjection.frag.glsl"
    "res/spriteProjection.vert.glsl"
)
source_group("" FILES ${no_group_source_files})

set(Header_Files
    "buffers/BufferLayout.h"
    "buffers/IndexBuffer.h"
    "buffers/VertexBuffer.h"
    "Camera.h"
    "Game.h"
    "models/Ball.h"
    "models/Brick.h"
    "models/GameObject.h"
    "models/Mesh.h"
    "models/Model.h"
    "models/Player.h"
    "models/Sprite.h"
    "models/Square.h"
    "Shader.h"
    "Texture.h"
    "Vertex.h"
    "VertexArray.h"
)
source_group("Header Files" FILES ${Header_Files})

set(Source_Files
    "buffers/BufferLayout.cpp"
    "buffers/IndexBuffer.cpp"
    "buffers/VertexBuffer.cpp"
    "deps/glad/src/glad.c"
    "Game.cpp"
    "Main.cpp"
    "models/Ball.cpp"
    "models/Brick.cpp"
    "models/GameObject.cpp"
    "models/Model.cpp"
    "models/Player.cpp"
    "models/Sprite.cpp"
    "models/Square.cpp"
    "Shader.cpp"
    "Texture.cpp"
    "VertexArray.cpp"
)
source_group("Source Files" FILES ${Source_Files})

set(ALL_FILES
    ${no_group_source_files}
    ${Header_Files}
    ${Source_Files}
)

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} ${ALL_FILES})

################################################################################
# Include directories
################################################################################
target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/deps/glad/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/deps/stb_image/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/deps/glm"
)

################################################################################
# Dependencies
################################################################################
if(MSVC)
    # Windows: use bundled libraries
    target_include_directories(${PROJECT_NAME} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/deps/glfw/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/deps/assimp/include"
    )

    target_link_libraries(${PROJECT_NAME} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/deps/glfw/lib/glfw3.lib"
        "${CMAKE_CURRENT_SOURCE_DIR}/deps/assimp/lib/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,Win32>/assimp-vc143-mtd.lib"
        opengl32
    )

    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:_DEBUG>"
        "$<$<CONFIG:Release>:NDEBUG>"
        "_CONSOLE"
        "_MBCS"
    )

    set_target_properties(${PROJECT_NAME} PROPERTIES
        VS_GLOBAL_KEYWORD "Win32Proj"
    )
else()
    # Linux/macOS: use system packages
    find_package(PkgConfig REQUIRED)
    find_package(OpenGL REQUIRED)

    pkg_check_modules(GLFW3 REQUIRED glfw3)
    pkg_check_modules(ASSIMP REQUIRED assimp)

    target_include_directories(${PROJECT_NAME} PRIVATE
        ${GLFW3_INCLUDE_DIRS}
        ${ASSIMP_INCLUDE_DIRS}
    )

    target_link_libraries(${PROJECT_NAME} PRIVATE
        OpenGL::GL
        ${GLFW3_LIBRARIES}
        ${ASSIMP_LIBRARIES}
        ${CMAKE_DL_LIBS}
    )

    target_link_directories(${PROJECT_NAME} PRIVATE
        ${GLFW3_LIBRARY_DIRS}
        ${ASSIMP_LIBRARY_DIRS}
    )
endif()
