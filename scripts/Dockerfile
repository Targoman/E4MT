FROM alpine:3.7
LABEL maintainers "S.Mehran M.ziabary <zaiabry@targoman.com>"

WORKDIR /tmp
RUN apk update && apk upgrade && apk add --no-cache bash sed grep coreutils qt5-qtbase zlib libxml2
RUN apk add --virtual .build-dependencies --no-cache gcc g++ make qt5-qtbase-dev zlib-dev libxml2-dev curl git \
    && ln -s /lib/libz.so /usr/lib/ \
    && git clone -b $(curl -s https://api.github.com/repos/Targoman/E4MT/releases/latest | grep tag_name | cut -d ':' -f 2 | sed 's/[\", ]//g') --recursive --single-branch https://github.com/Targoman/E4MT.git \
    && cd /tmp/E4MT && bash ./buildAndTest.sh full release /tmp/local \
    && make install; echo \
    && mkdir -p /etc/E4MT/FastTextModels \
    && mv /tmp/local/bin/* /usr/bin \
    && mv /tmp/local/lib/* /usr/lib \
    && mv /tmp/local/conf/* /etc/E4MT/ \
    && rm -rf /tmp/ \
    && apk del .build-dependencies

COPY ./scripts/E4MT.server.conf ./scripts/tsa.ini /etc/E4MT/

VOLUME /etc/E4MT/FastTextModels

EXPOSE 10000

ENTRYPOINT [ "E4MT", "-c","/etc/E4MT/E4MT.server.conf","-l", "en", "-m", "Server", "--admin-mode", "LegacyTcp" ]
