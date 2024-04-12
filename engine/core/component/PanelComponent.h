//
// (c) 2024 Eduardo Doria.
//

#ifndef PANEL_COMPONENT_H
#define PANEL_COMPONENT_H

#include "util/FunctionSubscribe.h"

namespace Supernova{

    struct PanelComponent{
        Entity title = NULL_ENTITY;

        bool needUpdatePanel = true;
    };

}

#endif //PANEL_COMPONENT_H