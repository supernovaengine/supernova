1.Rename jquant2.c "box" to "j_box" and "boxptr" to "j_boxptr" so that to avoid conflict with 
Medusa/Engine/External/jpeg/jquant2.c:268:3: error: redefinition of 'box' as different kind of symbol
} box;
  ^
In module 'Darwin' imported from /Users/fjz/Documents/MAC/Medusa/Engine/External/jpeg/jinclude.h:35:
/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator9.0.sdk/usr/include/curses.h:537:28: note: previous definition is here
extern NCURSES_EXPORT(int) box (WINDOW *, chtype, chtype);              /* generated */
                           ^
/Users/fjz/Documents/MAC/Medusa/Engine/External/jpeg/jquant2.c:270:9: error: unknown type name 'box'
typedef box * boxptr;
