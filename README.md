# raylib templ
simple template for raylib games, focusing on shipping a web build.

## Desktop build instructions
Configure
```
# if on Linux, i recommend using ninja multi config:
cmake -S. -Bbuild -G"Ninja Multi-Config"

# on Windows, the default is fine:
cmake -S. -Bbuild
```

Build
```
cmake --build build
```

## Emscripten build instructions (web!)
Set up emscripten
```
git clone https://github.com/emscripten-core/emsdk.git
emsdk install latest
emsdk activate latest
```

Configure
```
# again, remove the "Ninja Multi-Config" bit if on Windows.
cmake -S. -Bbuild-web -G"Ninja Multi-Config" --toolchain emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake -DPLATFORM=Web
```

Build
```
cmake --build build-web
```
