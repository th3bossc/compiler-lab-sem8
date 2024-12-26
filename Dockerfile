FROM ubuntu:20.04
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
  bison \
  flex \
  libreadline-dev \
  libc6-dev \
  libfl-dev \
  wget \
  vim \
  make \
  gcc \
  curl \
  git \
  build-essential \
  gdb \
  zsh \
  && echo "Dependencies installed."

# Install Oh My Zsh (with debug output)
RUN echo "Installing Oh My Zsh..." \
  && curl -fsSL https://raw.githubusercontent.com/ohmyzsh/ohmyzsh/master/tools/install.sh | sh -x \
  && echo "Oh My Zsh installed."



RUN useradd -m expl

COPY ./dotfiles/.zshrc /home/expl/.zshrc
COPY ./dotfiles/.oh-my-zsh /home/expl/.oh-my-zsh

RUN chsh -s $(which zsh) expl
USER expl


RUN cd /home/expl \
  && git clone https://github.com/silcnitc/xsm_expl.git \
  && cd ./xsm_expl \
  && make

RUN cd /home/expl/xsm_expl \
  && echo "#!/bin/bash \n" | cat - xsm > temp \
  && mv temp xsm

RUN cd /home/expl/xsm_expl \
  && rm -rf .git

WORKDIR /home/expl/xsm_expl

