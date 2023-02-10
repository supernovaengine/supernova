//
// (c) 2023 Eduardo Doria.
//

#ifndef SYSTEM_H
#define SYSTEM_H

#define S_LOG_VERBOSE 1
#define S_LOG_DEBUG 2
#define S_LOG_WARN 3
#define S_LOG_ERROR 4

#include <stdio.h>
#include <string>
#include <vector>
#include "io/Data.h"
#include "sokol_gfx.h"

namespace Supernova {
    class System {
    protected:

        System() {}

    public:
        
        std::vector<std::string> args;

        static System& instance();

        virtual ~System() {}

        // *******
        // Used for user and Lua
        // *******
        virtual int getScreenWidth() = 0;
        virtual int getScreenHeight() = 0;

        virtual void showVirtualKeyboard();
        virtual void hideVirtualKeyboard();

        virtual bool isFullscreen();
        virtual void requestFullscreen();
        virtual void exitFullscreen();

        virtual char getDirSeparator();
        
        virtual std::string getAssetPath();
        virtual std::string getUserDataPath();
        virtual std::string getLuaPath();
        virtual std::string getShaderPath();

        // *******
        // Used only for engine
        // *******
        virtual sg_context_desc getSokolContext();

        virtual FILE* platformFopen(const char* fname, const char* mode);
        virtual bool syncFileSystem();

        virtual void platformLog(const int type, const char *fmt, va_list args);

        virtual bool getBoolForKey(const char *key, bool defaultValue);
        virtual int getIntegerForKey(const char *key, int defaultValue);
        virtual long getLongForKey(const char *key, long defaultValue);
        virtual float getFloatForKey(const char *key, float defaultValue);
        virtual double getDoubleForKey(const char *key, double defaultValue);
        virtual Data getDataForKey(const char *key, const Data& defaultValue);
        virtual std::string getStringForKey(const char *key, std::string defaultValue);

        virtual void setBoolForKey(const char *key, bool value);
        virtual void setIntegerForKey(const char *key, int value);
        virtual void setLongForKey(const char *key, long value);
        virtual void setFloatForKey(const char *key, float value);
        virtual void setDoubleForKey(const char *key, double value);
        virtual void setDataForKey(const char *key, Data& value);
        virtual void setStringForKey(const char* key, std::string value);

        virtual void removeKey(const char *key);

    };

}


#endif //SYSTEM_H
