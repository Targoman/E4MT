FROM alpine:3.7
LABEL maintainers "S.Mehran M.ziabary <zaiabry@targoman.com>"

WORKDIR /tmp
RUN mkdir -p /etc/E4MT
RUN apk update && apk upgrade \
    && apk add --no-cache bash sed grep coreutils qt5-qtbase zlib libxml2 \
    && apk add --virtual .build-dependencies --no-cache gcc g++ make qt5-qtbase-dev zlib-dev libxml2-dev curl git \
    && ln -s /lib/libz.so /usr/lib/ \
    && git clone -b $(curl -s https://api.github.com/repos/Targoman/E4MT/releases/latest | grep tag_name | cut -d ':' -f 2 | sed 's/[\", ]//g') --recursive --single-branch https://github.com/Targoman/E4MT.git \
    && cd E4MT && bash ./buildAndTest.sh full release && make install
    && apk del .build-dependencies

RUN mv ~/local/bin/* /usr/bin \
    && mv ~/local/lib/* /usr/lib \
    && mv ~/local/conf/* /etc/E4MT/ \
    && rm -rf /tmp/*

COPY ./scripts/E4MT.server.conf ./scripts/tsa.ini /etc/E4MT/

EXPOSE 10000

ENTRYPOINT [ "E4MT", "-c","/etc/E4MT/E4MT.server.conf","-l", "en", "-m", "Server", "--admin-mode", "LegacyTcp" ]
