set(HEADER_LIST 
    ${ControlDeck_SOURCE_DIR}/include/ControlDeck/cartridge.h 
    ${ControlDeck_SOURCE_DIR}/include/ControlDeck/common.h 
    ${ControlDeck_SOURCE_DIR}/include/ControlDeck/ines.h 
    ${ControlDeck_SOURCE_DIR}/include/ControlDeck/joypad.h 
    ${ControlDeck_SOURCE_DIR}/include/ControlDeck/nes.h 
    ${ControlDeck_SOURCE_DIR}/include/ControlDeck/Render.h 
    ${ControlDeck_SOURCE_DIR}/include/ControlDeck/CPU/AddressingMode.h 
    ${ControlDeck_SOURCE_DIR}/include/ControlDeck/CPU/AddressingModeHandler.h 
    ${ControlDeck_SOURCE_DIR}/include/ControlDeck/CPU/cpu2A03.h 
    ${ControlDeck_SOURCE_DIR}/include/ControlDeck/CPU/InstructionSet.h 
    ${ControlDeck_SOURCE_DIR}/include/ControlDeck/CPU/SystemComponents.h 
    ${ControlDeck_SOURCE_DIR}/include/ControlDeck/PPU/ColorPalette.h 
    ${ControlDeck_SOURCE_DIR}/include/ControlDeck/PPU/PPU2C02.h 
    ${ControlDeck_SOURCE_DIR}/include/ControlDeck/PPU/PPUComponents.h)

add_library(libControlDeck 
    nes.cpp
    Render.cpp 
    ines.cpp
    CPU/AddressingMode.cpp
    CPU/AddressingModeHandler.cpp
    CPU/CPU2A03.cpp
    CPU/InstructionSet.cpp
    PPU/PPU2C02.cpp
    PPU/PPUComponents.cpp 
    ${HEADER_LIST})


target_include_directories(libControlDeck PUBLIC ../include)
#target_include_directories(libControlDeck PRIVATE ??)

target_compile_features(libControlDeck PUBLIC cxx_std_11)

source_group(TREE "${PROJECT_SOURCE_DIR}/include" PREFIX "Header files" FILES ${HEADER_LIST})