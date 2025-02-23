#FROM ubuntu:24.04
FROM ubuntu:22.04
#FROM ubuntu:18.04

ARG DEBIAN_FRONTEND=noninteractive

RUN apt update 
RUN apt install -y g++ 
RUN apt install -y make
RUN apt install -y binutils
RUN apt install -y qtbase5-dev qt5-qmake 
#qt5-tools-dev qt5-tools-dev-tools
#RUN apt install -y qt6-base-dev qt6-tools-dev qt6-tools-dev-tools
#RUN apt install -y qt5-default
RUN apt install -y cmake
RUN apt install -y libssl-dev
RUN apt install -y libboost-system-dev
RUN apt install -y zlib1g-dev
RUN apt install -y libcurl4-openssl-dev
RUN apt install -y git
RUN apt install -y qtcreator
RUN pwd && git clone https://github.com/samoilovv/TinkoffInvestBot.git
RUN cd TinkoffInvestBot && git submodule update --init --recursive
RUN cd TinkoffInvestBot && mkdir build && cd build && cmake .. 
RUN cd TinkoffInvestBot/build && make

ENTRYPOINT ["qtcreator", ""]  
