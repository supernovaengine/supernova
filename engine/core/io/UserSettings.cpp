//
// (c) 2020 Eduardo Doria.
//

#include "UserSettings.h"
#include "system/System.h"
#include "util/Base64.h"

using namespace Supernova;

bool UserSettings::getBoolForKey(const char *key){
    return getBoolForKey(key, false);
}

int UserSettings::getIntegerForKey(const char *key){
    return getIntegerForKey(key, 0);
}

float UserSettings::getFloatForKey(const char *key){
    return getFloatForKey(key, 0);
}

double UserSettings::getDoubleForKey(const char *key){
    return getDoubleForKey(key, 0);
}

std::string UserSettings::getStringForKey(const char *key){
    return getStringForKey(key, "");
}

Data UserSettings::getDataForKey(const char *key){
    return getDataForKey(key, NULL);
}

bool UserSettings::getBoolForKey(const char *key, bool defaultValue){
    return System::instance().getBoolForKey(key, defaultValue);
}

int UserSettings::getIntegerForKey(const char *key, int defaultValue){
    return System::instance().getIntegerForKey(key, defaultValue);
}

float UserSettings::getFloatForKey(const char *key, float defaultValue){
    return System::instance().getFloatForKey(key, defaultValue);
}

double UserSettings::getDoubleForKey(const char *key, double defaultValue){
    return System::instance().getDoubleForKey(key, defaultValue);
}

std::string UserSettings::getStringForKey(const char *key, std::string defaultValue){
    return System::instance().getStringForKey(key, defaultValue);
}

Data UserSettings::getDataForKey(const char *key, const Data& defaultValue){
    std::string ret = System::instance().getStringForKey(key, "");

    if (ret.empty())
        return defaultValue;

    return Data(&Base64::decode(ret).front(), (unsigned int)ret.length(), false, true);
}

void UserSettings::setBoolForKey(const char *key, bool value){
    System::instance().setBoolForKey(key, value);
}

void UserSettings::setIntegerForKey(const char *key, int value){
    System::instance().setIntegerForKey(key, value);
}

void UserSettings::setFloatForKey(const char *key, float value){
    System::instance().setFloatForKey(key, value);
}

void UserSettings::setDoubleForKey(const char *key, double value){
    System::instance().setDoubleForKey(key, value);
}

void UserSettings::setStringForKey(const char* key, std::string value){
    System::instance().setStringForKey(key, value);
}

void UserSettings::setDataForKey(const char *key, Data& value){
    System::instance().setStringForKey(key, Base64::encode(value.getMemPtr(), value.length()));
}
