add_executable(Breakout
    buffers/BufferLayout.cpp
    buffers/IndexBuffer.cpp
    buffers/VertexBuffer.cpp
    models/Ball.cpp
    models/Brick.cpp
    models/GameObject.cpp
    models/Model.cpp
    models/Player.cpp
    models/PowerUp.cpp
    models/Sprite.cpp
    models/Square.cpp
    Game.cpp
    Main.cpp
    Shader.cpp
    Texture.cpp
    VertexArray.cpp
    deps/glad/src/glad.c
)

target_include_directories(Breakout PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/glad/include
    ${CMAKE_CURRENT_SOURCE_DIR}/deps/stb_image/include
)

target_link_libraries(Breakout PRIVATE
    OpenGL::GL
    glfw
    glm::glm
    assimp::assimp
    ${CMAKE_DL_LIBS}
)

if(MSVC)
    set_target_properties(Breakout PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:Breakout>
    )
endif()

# the game loads everything from res/ next to the executable
add_custom_command(TARGET Breakout POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/res
        $<TARGET_FILE_DIR:Breakout>/res
)
