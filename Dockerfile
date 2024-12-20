FROM ubuntu:20.04

RUN apt-get update \ 
  && apt-get install -y bison flex libreadline-dev libc6-dev libfl-dev wget vim make gcc curl git build-essential

RUN useradd -m expl
USER expl

RUN cd /home/expl \
  && git clone https://github.com/silcnitc/xsm_expl.git \
  && cd ./xsm_expl \
  && make

WORKDIR /home/expl/xsm_expl

