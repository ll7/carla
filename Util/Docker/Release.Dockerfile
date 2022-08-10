
# # run the docker container as
# # sudo -E docker run --rm --gpus all -it --net=host carla:latest /bin/bash

# FROM carla:latest as build

# FROM nvidia/vulkan:1.1.121-cuda-10.1--ubuntu18.04

# RUN useradd -m carla
# WORKDIR /home/carla

# COPY --from=build --chown=carla:carla /home/carla/carla/Dist/CARLA_Shipping_*/LinuxNoEditor .

# RUN apt-key adv --fetch-keys \
#     https://developer.download.nvidia.com/compute/cuda/repos/ubuntu1804/x86_64/3bf863cc.pub

# RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y \
#         libsdl2-2.0 xserver-xorg libvulkan1 libomp5 --no-install-recommends && \
#     apt-get install -y \
#         xdg-user-dirs xdg-utils \
#         libjpeg-dev libpng-dev libtiff-dev \
#         python3-pip && \
#     apt-get clean && \
#     python3 -m pip install pip --upgrade && python3 -m pip install pygame

# USER carla
# ENTRYPOINT ["/bin/bash", "./CarlaUE4.sh"]
# CMD ["-carla-server"]
