# Supernova Game Engine

www.supernovaengine.org

Supernova is a **free** and open source cross-platform game engine for create 2D and 3D projects with Lua or C++. It is lightweight and promote the simplest way to do the best results.

Licensed under Apache 2.0 license, you can use freely for whatever you want, including commercial projects.

Now it supports:
  - Android
  - iOS
  - Web (with Emscripten)

For future versions:
  - Windows
  - OSX
  - Linux


**Supernova is NOT ready for Production!**

Please, don't try. We are under development. :-)

## Getting started

Visit our wiki to see tutorials and documentation:

https://github.com/deslon/supernova/wiki

## Code sample
### Triangle Hello World (Lua)
```
Supernova.setCanvasSize(1000,480)

cena = Scene()
triangulo = Shape()

triangulo:addVertex(0, 100, 0)
triangulo:addVertex(-50, -50, 0)
triangulo:addVertex(50, -50, 0)

triangulo:setPosition(300,300,0)
triangulo:setColor(0.6, 0.2, 0.6, 1)

cena:addObject(triangulo)

Supernova.setScene(cena)
```
### Triangle Hello World (C++)

```
#include "Supernova.h"

#include "Scene.h"
#include "Shape.h"
#include "Camera.h"

Shape triangulo;
Scene cena;

void init(){
    Supernova::setCanvasSize(1000,480);
    
    triangulo.addVertex(Vector3(0, 100, 0));
    triangulo.addVertex(Vector3(-50, -50, 0));
    triangulo.addVertex(Vector3(50, -50, 0));
    
    triangulo.setPosition(Vector3(300,300,0));
    triangulo.setColor(0.6, 0.2, 0.6, 1);
    cena.addObject(&triangulo);
  
    Supernova::setScene(&cena);
}
```

##License


Copyright 2016 Eduardo Dória Lima

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
