# Running tests and generating docs
Tested on Linux/Docker. Make sure to install gcc make cmake git.
```
# generate binaries and docs with:
$ cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_DOC=on && cmake --build build 
# run tests with:
$ (pushd build && ctest --output-on-failure; popd)
# view docs with firefox or your favourite browser:
$ firefox build/docs/doc_doxygen/html/index.html
# run a test, log a timestamp every second to a provided logfile:
$ build/log_to_file_every_second /test
$ tail -f ./test
```
## Docker
Could also run in docker as such:
```
$ docker build -t interview .
$ docker run interview --name test
$ docker exec -i test tail -f /tmp/logfile.txt
```
# Dependencies
Dependencies are fetched and built using CMake, I've used the following libs:
* Taywee for parsing arguments
* googletest for testing the Logger API
# Notes
* The logger class is multi-threading safe but doesn't take advantage of multiple threads.
* It can output to well-formed `std::ostream`s.

