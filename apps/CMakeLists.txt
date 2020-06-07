add_executable(ControlDeckNES WIN32
    main.cpp glad.c shaderLoader.h glad/glad.h)
target_compile_features(ControlDeckNES PRIVATE cxx_std_11)

configure_file(shaders/shader.frag ${CMAKE_CURRENT_BINARY_DIR}/shaders/shader.frag COPYONLY)
configure_file(shaders/shader.vert ${CMAKE_CURRENT_BINARY_DIR}/shaders/shader.vert COPYONLY)


target_link_libraries(ControlDeckNES PRIVATE libControlDeck)
target_link_libraries(ControlDeckNES PRIVATE glfw)
