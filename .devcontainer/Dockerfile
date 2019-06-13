FROM xilinx-vivado:2018.1

RUN apt-get update && apt-get install -y \
    vim \
    curl \
    build-essential \
    cmake \
    git \
    xterm \
    screen \
    libncurses-dev \
    libfuse-dev \
    liblmdb-dev \
    libprotobuf-dev \
    protobuf-compiler

WORKDIR /metal

# libjq
RUN curl -L https://github.com/stedolan/jq/releases/download/jq-1.5/jq-1.5.tar.gz --output jq-1.5.tar.gz && tar -xzf jq-1.5.tar.gz && rm jq-1.5.tar.gz
RUN cd jq-1.5 && ./configure --with-oniguruma=builtin --disable-maintainer-mode && make LDFLAGS=-all-static && mkdir -p builtin/lib && make install

# PSLSE
ENV PSLSE_ROOT=/metal/pslse
RUN git clone https://github.com/ibm-capi/pslse $PSLSE_ROOT
ENV PSLVER=8
ENV VPI_USER_H_DIR=/opt/Xilinx/Vivado/2018.1/data/xsim/include
RUN cd $PSLSE_ROOT && make

# SNAP
ENV SNAP_ROOT=/metal/snap
RUN git clone -b sim_screen https://github.com/rs22/snap $SNAP_ROOT
RUN cd $SNAP_ROOT && make software

# Metal FS
RUN apt-key adv --recv-keys --keyserver keyserver.ubuntu.com 60C317803A41BA51845E371A1E9377A2BA9EF27F \
    && printf "deb http://ppa.launchpad.net/ubuntu-toolchain-r/test/ubuntu xenial main\ndeb-src http://ppa.launchpad.net/ubuntu-toolchain-r/test/ubuntu xenial main" > /etc/apt/sources.list.d/backports.list \
    && apt-get update \
    && apt-get install -y gcc-7 g++-7 \
    && update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-7 60 --slave /usr/bin/g++ g++ /usr/bin/g++-7
ENV METAL_ROOT=/metal/metal
RUN git clone -b software_cpp https://github.com/osmhpi/metal_fs $METAL_ROOT
RUN mkdir $METAL_ROOT/build && cd $METAL_ROOT/build && cmake .. && make metal_fs
RUN ln -s $METAL_ROOT/build/metal_fs $METAL_ROOT/src/metal_fpga/sw/metal_fs

RUN echo "source /opt/Xilinx/Vivado/2018.1/settings64.sh" >> /root/.bashrc
RUN echo "export XILINXD_LICENSE_FILE=/tmp/null" >> /root/.bashrc
RUN echo "export PSLSE_ROOT=$PSLSE_ROOT" >> /root/.bashrc
RUN echo "export SNAP_ROOT=$SNAP_ROOT" >> /root/.bashrc
RUN echo "export LC_ALL=C" >> /root/.bashrc

# Set the default shell to bash instead of sh
ENV SHELL /bin/bash
