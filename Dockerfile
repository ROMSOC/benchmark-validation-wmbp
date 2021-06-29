# Use Ubuntu 16.04 as parent image
FROM ubuntu:xenial

# Install LifeV dependencies
RUN apt-get update && \
    apt-get install -y \
        g++ \
        cmake \
        git \
        vim \
        libblacs-mpi-dev \
        libscalapack-mpi-dev \
        libsuitesparse-dev \
        trilinos-all-dev \
        libboost-program-options-dev \
        libparmetis-dev \
        libmetis-dev \
        libhdf5-openmpi-dev \
        libmumps-dev \
        libsuperlu-dev \
        libtbb-dev \
        libptscotch-dev \
        binutils-dev \
        libiberty-dev \
        libtriangle-dev && \
    groupadd -r lifev && \
    useradd -l -m -g lifev lifev

# Set user 'lifev'
USER lifev

# Set working directory
WORKDIR /home/lifev

# Copy the content of the current dir into the WORKDIR
ADD --chown=lifev:lifev . /home/lifev
