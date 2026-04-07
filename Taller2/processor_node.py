import rclpy
from rclpy.node import Node
from std_msgs.msg import Int32,Float32

class ProcessorNode(Node):
    def __init__(self):
        super().__init__('processor_node')
        self.subscription = self.create_subscription(Int32,'sensor_data',self.listener_callback,10)
        self.publisher_ = self.create_publisher(Float32,'temperature_celsius',10)

    def listener_callback(self,msg):
        raw_value = msg.data
        temperature = (raw_value/1023.0)*100.0
        temp_msg = Float32()
        temp_msg.data = temperature
        self.publisher_.publish(temp_msg)
        self.get_logger().info(f'Procesado: {temperature:.2f} C')

def main(args=None):
    rclpy.init(args=args)
    node = ProcessorNode()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()