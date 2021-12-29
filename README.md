![Logo](http://www.supernovaengine.org/img/logo_blue.png)

![](https://github.com/supernovaengine/supernova/workflows/IOS/badge.svg)
![](https://github.com/supernovaengine/supernova/workflows/Android/badge.svg)
![](https://github.com/supernovaengine/supernova/workflows/Web/badge.svg)

&nbsp;

**This version is deprecated**

&nbsp;

Supernova is a **free** and open source cross-platform game engine for create 2D and 3D projects with Lua or C++. It is lightweight and promote the simplest way to do the best results.

Licensed under MIT license, you can use freely for whatever you want, including commercial projects.

It supports:
  - Android
  - iOS
  - Web (with Emscripten)

## Code sample
### Triangle Hello World (Lua)
```
Engine.setCanvasSize(1000,480)

cena = Scene()
triangulo = Polygon()

triangulo:addVertex(0, -100, 0)
triangulo:addVertex(-50, 50, 0)
triangulo:addVertex(50, 50, 0)

triangulo:setPosition(300,300,0)
triangulo:setColor(0.6, 0.2, 0.6, 1)

cena:addObject(triangulo)

Engine.setScene(cena)
```
### Triangle Hello World (C++)

```
#include "Supernova.h"

#include "Scene.h"
#include "Polygon.h"

using namespace Supernova;
Polygon triangulo;
Scene cena;

void init(){
    Engine::setCanvasSize(1000,480);

    triangulo.addVertex(Vector3(0, -100, 0));
    triangulo.addVertex(Vector3(-50, 50, 0));
    triangulo.addVertex(Vector3(50, 50, 0));

    triangulo.setPosition(Vector3(300,300,0));
    triangulo.setColor(0.6, 0.2, 0.6, 1);
    cena.addObject(&triangulo);

    Engine::setScene(&cena);
}
```
