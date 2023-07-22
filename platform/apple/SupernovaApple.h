#ifndef SupernovaApple_h
#define SupernovaApple_h

#include "System.h"

class SupernovaApple: public Supernova::System{

public:

    SupernovaApple();
    
    virtual ~SupernovaApple();
    
    virtual sg_context_desc getSokolContext();

    virtual int getScreenWidth();
    virtual int getScreenHeight();
    
    virtual void showVirtualKeyboard();
    virtual void hideVirtualKeyboard();
    
    virtual std::string getAssetPath();
    virtual std::string getUserDataPath();
    virtual std::string getLuaPath();
    
    virtual bool getBoolForKey(const char *key, bool defaultValue);
    virtual int getIntegerForKey(const char *key, int defaultValue);
    virtual long getLongForKey(const char *key, long defaultValue);
    virtual float getFloatForKey(const char *key, float defaultValue);
    virtual double getDoubleForKey(const char *key, double defaultValue);
    virtual Supernova::Data getDataForKey(const char* key, const Supernova::Data& defaultValue);
    virtual std::string getStringForKey(const char *key, std::string defaultValue);

    virtual void setBoolForKey(const char *key, bool value);
    virtual void setIntegerForKey(const char *key, int value);
    virtual void setLongForKey(const char *key, long value);
    virtual void setFloatForKey(const char *key, float value);
    virtual void setDoubleForKey(const char *key, double value);
    virtual void setDataForKey(const char* key, Supernova::Data& value);
    virtual void setStringForKey(const char* key, std::string value);

    virtual void removeKey(const char *key);

    virtual void initializeAdMob(bool tagForChildDirectedTreatment, bool tagForUnderAgeOfConsent);
    virtual void loadInterstitialAd(std::string adUnitID);
    virtual bool isInterstitialAdLoaded();
    virtual void showInterstitialAd();
};


#endif /* SupernovaSokol_h */
