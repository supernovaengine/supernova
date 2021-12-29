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

sg_context_desc SupernovaSokol::getSokolContext(){
    return sapp_sgcontext();
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

std::string SupernovaSokol::getAssetPath(){
    return "assets";
}

std::string SupernovaSokol::getLuaPath(){
    return "lua";
}