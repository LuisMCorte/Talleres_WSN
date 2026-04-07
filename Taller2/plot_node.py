import rclpy
from rclpy.node import Node
from std_msgs.msg import Float32
import matplotlib.pyplot as plt
import time
import os


class PlotterNode(Node):
    def __init__(self):
        super().__init__('plotter_node')

        self.subscription = self.create_subscription(
            Float32,
            'temperature_celsius',
            self.listener_callback,
            10
        )

        self.data = []
        self.time_data = []
        self.start_time = time.time()

        # Timer para actualizar la gráfica cada 5 segundos
        self.timer = self.create_timer(5.0, self.plot_data)

        # Crear carpeta para guardar las gráficas
        os.makedirs('/ros2_ws/data', exist_ok=True)

        self.get_logger().info('PlotterNode iniciado. Esperando datos en /temperature_celsius')


    def listener_callback(self, msg: Float32):
        """Callback que recibe la temperatura"""
        current_time = time.time() - self.start_time

        self.data.append(msg.data)
        self.time_data.append(current_time)

        # Mantener solo los últimos 100 puntos
        if len(self.data) > 50:
            self.data.pop(0)
            self.time_data.pop(0)

        self.get_logger().debug(f'Temperatura recibida: {msg.data:.2f} °C')


    def plot_data(self):
        """Genera y guarda la gráfica"""
        if len(self.data) < 2:
            return

        plt.figure(figsize=(10, 6))
        plt.plot(self.time_data, self.data, 'b-', linewidth=2)
        plt.xlabel('Tiempo (segundos)')
        plt.ylabel('Temperatura (°C)')
        plt.title('Sensor de Temperatura en Tiempo Real')
        plt.grid(True)
        plt.tight_layout()

        # Guardar la imagen
        plot_path = '/ros2_ws/data/sensor_plot.png'
        plt.savefig(plot_path)
        plt.close()

        self.get_logger().info(f'Gráfico actualizado → {plot_path}')


def main(args=None):
    rclpy.init(args=args)
    node = PlotterNode()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == '__main__':
    main()