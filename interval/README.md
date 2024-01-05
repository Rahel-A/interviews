# Running tests
Make sure to install gcc make cmake
```
$ make -S . -B build && cmake --build build && (pushd build && ctest --output-on-failure; popd)
```
