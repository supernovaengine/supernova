//
// (c) 2020 Eduardo Doria.
//

#ifndef USERSETTINGS_H
#define USERSETTINGS_H

#include <string>
#include "io/Data.h"

namespace Supernova {
    class UserSettings {
    public:

        static bool getBoolForKey(const char *key);
        static int getIntegerForKey(const char *key);
        static float getFloatForKey(const char *key);
        static double getDoubleForKey(const char *key);
        static std::string getStringForKey(const char *key);
        static Data getDataForKey(const char *key);

        static bool getBoolForKey(const char *key, bool defaultValue);
        static int getIntegerForKey(const char *key, int defaultValue);
        static float getFloatForKey(const char *key, float defaultValue);
        static double getDoubleForKey(const char *key, double defaultValue);
        static std::string getStringForKey(const char *key, std::string defaultValue);
        static Data getDataForKey(const char *key, const Data& defaultValue);

        static void setBoolForKey(const char *key, bool value);
        static void setIntegerForKey(const char *key, int value);
        static void setFloatForKey(const char *key, float value);
        static void setDoubleForKey(const char *key, double value);
        static void setStringForKey(const char* key, std::string value);
        static void setDataForKey(const char *key, Data& value);

    };
}


#endif //USERSETTINGS_H
