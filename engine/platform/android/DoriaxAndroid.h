// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#ifndef DoriaxAndroid_H_
#define DoriaxAndroid_H_

#include <stdio.h>
#include <string>
#include "System.h"

class DoriaxAndroid: public doriax::System {

private:

    const char* logtag;

public:

    DoriaxAndroid();

    virtual int getScreenWidth() override;
    virtual int getScreenHeight() override;

    virtual void showVirtualKeyboard(std::wstring text) override;
    virtual void hideVirtualKeyboard() override;

    virtual void quit() override;

    virtual std::string getUserDataPath() override;

    virtual FILE* platformFopen(const char* fname, const char* mode) override;
    virtual void platformLog(const int type, const char *fmt, va_list args) override;

    virtual bool getBoolForKey(const char *key, bool defaultValue) override;
    virtual int getIntegerForKey(const char *key, int defaultValue) override;
    virtual long getLongForKey(const char *key, long defaultValue) override;
    virtual float getFloatForKey(const char *key, float defaultValue) override;
    virtual double getDoubleForKey(const char *key, double defaultValue) override;
    virtual std::string getStringForKey(const char *key, const std::string& defaultValue) override;

    virtual void setBoolForKey(const char *key, bool value) override;
    virtual void setIntegerForKey(const char *key, int value) override;
    virtual void setLongForKey(const char *key, long value) override;
    virtual void setFloatForKey(const char *key, float value) override;
    virtual void setDoubleForKey(const char *key, double value) override;
    virtual void setStringForKey(const char* key, const std::string& value) override;

    virtual void removeKey(const char* key) override;

    virtual void initializeAdMob(bool tagForChildDirectedTreatment, bool tagForUnderAgeOfConsent) override;
    virtual void setMaxAdContentRating(doriax::AdMobRating rating) override;
    virtual void loadInterstitialAd(const std::string& adUnitID) override;
    virtual bool isInterstitialAdLoaded() override;
    virtual void showInterstitialAd() override;
};

#endif /* DoriaxAndroid_H_ */
