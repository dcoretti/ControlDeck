add_executable(ControlDeckNES WIN32
    main.cpp 
    shaderLoader.h
    
    imgui/imconfig.h
    imgui/imgui.h
    imgui/imgui.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_impl_glfw.cpp
    imgui/imgui_impl_glfw.h
    imgui/imgui_impl_opengl3.cpp
    imgui/imgui_impl_opengl3.h
    imgui/imgui_internal.h
    imgui/imgui_widgets.cpp
    imgui/imstb_rectpack.h
    imgui/imstb_textedit.h
    imgui/imstb_truetype.h
    )
target_compile_features(ControlDeckNES PRIVATE cxx_std_11)

# Copy static files / config to build dir
# This sort of works only because we can refer to the config from the exe via ../shaders/
# because I can't figure out how to get this to move to the DEBUG or RELEASE folder.
# not sure I'd actually want it there and maybe it just needs to be assumed to go ../config/blah
configure_file(shaders/shader.frag ${CMAKE_CURRENT_BINARY_DIR}/shaders/shader.frag COPYONLY)
configure_file(shaders/shader.vert ${CMAKE_CURRENT_BINARY_DIR}/shaders/shader.vert COPYONLY)


target_link_libraries(ControlDeckNES PRIVATE libControlDeck)
target_link_libraries(ControlDeckNES PRIVATE glfw)
target_link_libraries(ControlDeckNES PRIVATE gl3w)
# target_include_directories(ControlDeckNES PRIVATE gl3w)
