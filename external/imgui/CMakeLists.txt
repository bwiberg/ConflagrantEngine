project(imgui)
add_library(imgui
        imconfig.h imgui_internal.h stb_rect_pack.h stb_textedit.h stb_truetype.h imgui.h
        imgui.cpp imgui_demo.cpp imgui_draw.cpp)
