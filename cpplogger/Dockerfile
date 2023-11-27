FROM debian:bookworm AS base

# RUN set -ex; \
#     apt-get update; \
#     apt-get install -y <deps>; \
#     mkdir -p /usr/src

FROM base AS builder

RUN set -ex; \
    apt-get update; \
    apt-get install -y g++ cmake git

COPY CMakeLists.txt /usr/src/logger/
COPY docs /usr/src/logger/docs/
COPY src /usr/src/logger/src/

WORKDIR /usr/src/logger
RUN cmake -S . -B build
RUN cmake --build build

FROM base AS runtime
COPY --from=builder /usr/src/logger/build/log_to_file_every_second /usr/local/bin
ENTRYPOINT ["log_to_file_every_second", "/tmp/logfile.txt"]

