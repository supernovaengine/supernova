#ifndef SupernovaAndroid_H_
#define SupernovaAndroid_H_

#include <stdio.h>
#include <string>
#include "system/System.h"

class SupernovaAndroid: public Supernova::System {

private:

    const char* logtag;

public:

    SupernovaAndroid();

    virtual int getScreenWidth();
    virtual int getScreenHeight();

	virtual void showVirtualKeyboard();
	virtual void hideVirtualKeyboard();

	std::string getUserDataPath();

    virtual FILE* platformFopen(const char* fname, const char* mode);
	virtual void platformLog(const int type, const char *fmt, va_list args);
};

#endif /* SupernovaAndroid_H_ */
