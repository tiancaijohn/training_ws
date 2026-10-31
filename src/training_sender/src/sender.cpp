#include <chrono>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <thread>

#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/pose_stamped.hpp"

class PoseSender : public rclcpp :: Node{
    public:
        using Msg = geometry_msgs::msg::PoseStamped;

        PoseSender() : Node("pose_sender"){
            pub_ = create_publisher<Msg>("/training_pose",  rclcpp::QoS(10).reliable());
        }

        bool has_subscriber() const {
            return pub_->get_subscription_count() > 0;
        }

        void send(double x, double y, double z){
            Msg msg;
            msg.header.stamp = now();
            msg.header.frame_id = "map";

            msg.pose.position.x = x;
            msg.pose.position.y = y;
            msg.pose.position.z = z;

            msg.pose.orientation.w = 1.0;

            pub_->publish(msg);

            RCLCPP_INFO(
                get_logger(),"Sent: x=%.3f y=%.3f z=%.3f", x, y, z
            );
         }

    private:
         rclcpp::Publisher<Msg>::SharedPtr pub_;
};

int main(int argc, char **argv){
    rclcpp::init(argc, argv);

    auto node = std::make_shared<PoseSender>();

    std::cout << "Waiting for receiver..." << std::endl;

    while(rclcpp::ok() && !node->has_subscriber()){
        rclcpp::spin_some(node);
        std::this_thread::sleep_for(
            std::chrono::milliseconds(100)
        );
    }

    std::string line;

    while(rclcpp::ok())
    {
        std::cout << "Enter x y z or q to quit: " << std::flush;
        if(!std::getline(std::cin, line) || line == "q"){
            break;
        }

        std::istringstream input(line);
        double x, y, z;
        std::string extra;
        
        if(!(input >> x >> y >> z) || (input >> extra)){
            std::cout << "Please enter exactly threee numbers.\n";
            continue;
        }

        if(!rclcpp::ok()){
            break;
        }

        node->send(x, y, z);
        rclcpp::spin_some(node);
    }

    rclcpp::shutdown();
    return 0;

}
