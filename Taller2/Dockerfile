FROM osrf/ros:jazzy-desktop

# IMPORTANTE: usar el entrypoint oficial de ROS
ENTRYPOINT ["/ros_entrypoint.sh"]
CMD ["bash"]

# Instalar dependencias necesarias
RUN apt update && apt install -y \
    python3-pip \
    python3-serial \
    python3-colcon-common-extensions \
    && rm -rf /var/lib/apt/lists/*

# Crear workspace
WORKDIR /ros2_ws

# Crear estructura básica
RUN mkdir -p /ros2_ws/src

# (Opcional) instalar matplotlib para el punto 4
# Instalar matplotlib correctamente
RUN apt update && apt install -y python3-matplotlib