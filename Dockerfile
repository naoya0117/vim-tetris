FROM ubuntu

COPY ./src /app
WORKDIR /app

RUN apt-get update && apt-get install -y \
    language-pack-ja \
    gcc \
    make \
    libncursesw5-dev \
&& apt-get clean \
&& rm -rf /var/lib/apt/lists/*
RUN locale-gen ja_JP.UTF-8
ENV LANG ja_JP.UTF-8

RUN make -f /app/makefile 

