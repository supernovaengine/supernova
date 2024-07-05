#include "SupernovaSokol.h"

#include "sokol_gfx.h"
#include "sokol_app.h"
#include "sokol_glue.h"

SupernovaSokol::SupernovaSokol(){

}

int SupernovaSokol::getScreenWidth(){
    return sapp_width();
}

int SupernovaSokol::getScreenHeight(){
    return sapp_height();
}

sg_environment SupernovaSokol::getSokolEnvironment(){
    return sglue_environment();
}

sg_swapchain SupernovaSokol::getSokolSwapchain(){
    return sglue_swapchain();
}

bool SupernovaSokol::isFullscreen(){
    return sapp_is_fullscreen();
}

void SupernovaSokol::requestFullscreen(){
    if (!sapp_is_fullscreen())
        sapp_toggle_fullscreen();
}

void SupernovaSokol::exitFullscreen(){
    if (sapp_is_fullscreen())
        sapp_toggle_fullscreen();
}

void SupernovaSokol::setMouseCursor(Supernova::CursorType type){
    if (type == Supernova::CursorType::ARROW){
        sapp_set_mouse_cursor(SAPP_MOUSECURSOR_ARROW);
    }else if (type == Supernova::CursorType::IBEAM){
        sapp_set_mouse_cursor(SAPP_MOUSECURSOR_IBEAM);
    }else if (type == Supernova::CursorType::CROSSHAIR){
        sapp_set_mouse_cursor(SAPP_MOUSECURSOR_CROSSHAIR);
    }else if (type == Supernova::CursorType::POINTING_HAND){
        sapp_set_mouse_cursor(SAPP_MOUSECURSOR_POINTING_HAND);
    }else if (type == Supernova::CursorType::RESIZE_EW){
        sapp_set_mouse_cursor(SAPP_MOUSECURSOR_RESIZE_EW);
    }else if (type == Supernova::CursorType::RESIZE_NS){
        sapp_set_mouse_cursor(SAPP_MOUSECURSOR_RESIZE_NS);
    }else if (type == Supernova::CursorType::RESIZE_NWSE){
        sapp_set_mouse_cursor(SAPP_MOUSECURSOR_RESIZE_NWSE);
    }else if (type == Supernova::CursorType::RESIZE_NESW){
        sapp_set_mouse_cursor(SAPP_MOUSECURSOR_RESIZE_NESW);
    }else if (type == Supernova::CursorType::RESIZE_ALL){
        sapp_set_mouse_cursor(SAPP_MOUSECURSOR_RESIZE_ALL);
    }else if (type == Supernova::CursorType::NOT_ALLOWED){
        sapp_set_mouse_cursor(SAPP_MOUSECURSOR_NOT_ALLOWED);
    }
}

void SupernovaSokol::setShowCursor(bool showCursor){
    sapp_show_mouse(showCursor);
}

std::string SupernovaSokol::getAssetPath(){
    return "assets";
}

std::string SupernovaSokol::getUserDataPath(){
    return ".";
}

std::string SupernovaSokol::getLuaPath(){
    return "lua";
}