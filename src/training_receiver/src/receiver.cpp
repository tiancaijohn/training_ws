#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/pose_stamped.hpp"

class PoseReceiver : public rclcpp :: Node{
    public:
        using Msg = geometry_msgs::msg::PoseStamped;

        PoseReceiver():Node("pose_receiver"){
            sub_ = create_subscription<Msg>(
                "/training_pose",
                rclcpp::QoS(10).reliable(),
                [this](Msg::ConstSharedPtr msg){
                    onMessage(msg);
                }
            );
        }

    private:
        rclcpp::Subscription<Msg>::SharedPtr sub_;



        void onMessage(Msg::ConstSharedPtr msg)
        {
            RCLCPP_INFO(
                get_logger(),
                "Received:x=%.3f y=%.3f z=%.3f frame=%s",
                msg->pose.position.x,
                msg->pose.position.y,
                msg->pose.position.z,
                msg->header.frame_id.c_str()
            );
        }
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<PoseReceiver>();

    rclcpp::spin(node);

    rclcpp::shutdown();
    return 0;
}