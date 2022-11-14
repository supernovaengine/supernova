#include "Engine.h"
#include "SupernovaSokol.h"
#include "Input.h"

#include "sokol_gfx.h"
#include "sokol_app.h"
#include "sokol_glue.h"

void sokol_init(void) {
    Supernova::Engine::systemViewLoaded();
    Supernova::Engine::systemViewChanged();
}

void sokol_frame(void) {
    Supernova::Engine::systemDraw();
}

int convMouseButtom(sapp_mousebutton mouse_button){
    if (mouse_button == SAPP_MOUSEBUTTON_LEFT){
        return S_MOUSE_BUTTON_LEFT;
    }else if (mouse_button == SAPP_MOUSEBUTTON_RIGHT){
        return S_MOUSE_BUTTON_RIGHT;
    }else if (mouse_button == SAPP_MOUSEBUTTON_MIDDLE){
        return S_MOUSE_BUTTON_MIDDLE;
    }
    return S_MOUSE_BUTTON_LEFT;
}

static void sokol_event(const sapp_event* e) {
    if (e->type == SAPP_EVENTTYPE_RESIZED)
        Supernova::Engine::systemViewChanged();
    else if (e->type == SAPP_EVENTTYPE_CHAR)
        Supernova::Engine::systemCharInput(e->char_code);
    else if (e->type == SAPP_EVENTTYPE_KEY_DOWN){
        if (e->key_code == SAPP_KEYCODE_TAB)
            Supernova::Engine::systemCharInput('\t');
        if (e->key_code == SAPP_KEYCODE_BACKSPACE)
            Supernova::Engine::systemCharInput('\b');
        if (e->key_code == SAPP_KEYCODE_ENTER)
            Supernova::Engine::systemCharInput('\r');
        if (e->key_code == SAPP_KEYCODE_ESCAPE)
            Supernova::Engine::systemCharInput('\e');
        //Use same keycode of GLFW
        Supernova::Engine::systemKeyDown(e->key_code, e->key_repeat, e->modifiers);
    }else if (e->type == SAPP_EVENTTYPE_KEY_UP)
        //Use same keycode of GLFW
        Supernova::Engine::systemKeyUp(e->key_code, e->key_repeat, e->modifiers);
    else if (e->type == SAPP_EVENTTYPE_SUSPENDED)
        Supernova::Engine::systemPause();
    else if (e->type == SAPP_EVENTTYPE_RESUMED)
        Supernova::Engine::systemResume();  
    else if (e->type == SAPP_EVENTTYPE_MOUSE_UP)
        Supernova::Engine::systemMouseUp(convMouseButtom(e->mouse_button), e->mouse_x, e->mouse_y, e->modifiers);  
    else if (e->type == SAPP_EVENTTYPE_MOUSE_DOWN)
        Supernova::Engine::systemMouseDown(convMouseButtom(e->mouse_button), e->mouse_x, e->mouse_y, e->modifiers);
    else if (e->type == SAPP_EVENTTYPE_MOUSE_UP)
        Supernova::Engine::systemMouseUp(convMouseButtom(e->mouse_button), e->mouse_x, e->mouse_y, e->modifiers);
    else if (e->type == SAPP_EVENTTYPE_MOUSE_MOVE)
        Supernova::Engine::systemMouseMove(e->mouse_x, e->mouse_y, e->modifiers);
    else if (e->type == SAPP_EVENTTYPE_MOUSE_SCROLL)
        Supernova::Engine::systemMouseScroll(e->scroll_x, e->scroll_y, e->modifiers);
    else if (e->type == SAPP_EVENTTYPE_MOUSE_ENTER)
        Supernova::Engine::systemMouseEnter();
    else if (e->type == SAPP_EVENTTYPE_MOUSE_LEAVE)
        Supernova::Engine::systemMouseLeave();
}

void sokol_cleanup(void) {
    Supernova::Engine::systemShutdown();
}

sapp_desc sokol_main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    sapp_desc app_desc = {0};

    app_desc.init_cb = sokol_init;
    app_desc.frame_cb = sokol_frame;
    app_desc.event_cb = sokol_event;
    app_desc.cleanup_cb = sokol_cleanup;
    app_desc.width = 800;
    app_desc.height = 600;
    app_desc.sample_count = 4;
    app_desc.gl_force_gles2 = false;
    app_desc.window_title = "Supernova";

    Supernova::Engine::systemInit(argc, argv);

    return app_desc;
}