# Running tests and generating docs
Tested on Linux/Docker. Make sure to install gcc make cmake git.
```
# generate binaries and docs with:
$ cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_DOC=on && cmake --build build 
# run tests with:
$ (pushd build && ctest --output-on-failure; popd)
```
# Dependencies
Dependencies are fetched and built using CMake, I've used the following libs:
* googletest for testing the Logger API
# Notes

