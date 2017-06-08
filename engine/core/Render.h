#ifndef render_h
#define render_h


class Render {

public:

    virtual bool load() = 0;
    virtual bool draw() = 0;
    virtual void destroy() = 0;

};

#endif /* Render_h */
