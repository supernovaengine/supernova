//
// (c) 2020 Eduardo Doria.
//

#include "UserSettings.h"
#include "system/System.h"

using namespace Supernova;

bool UserSettings::getBoolForKey(const char *key){
    return System::instance().getBoolForKey(key, false);
}

int UserSettings::getIntegerForKey(const char *key){
    return System::instance().getIntegerForKey(key, 0);
}

float UserSettings::getFloatForKey(const char *key){
    return System::instance().getFloatForKey(key, 0);
}

double UserSettings::getDoubleForKey(const char *key){
    return System::instance().getDoubleForKey(key, 0);
}

std::string UserSettings::getStringForKey(const char *key){
    return System::instance().getStringForKey(key, "");
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
