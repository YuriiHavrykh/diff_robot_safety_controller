#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"
#include "geometry_msgs/msg/twist_stamped.hpp"
#include <cmath>
#include <chrono>

using namespace std::chrono_literals;

class SafetyController : public rclcpp::Node {
public:
    SafetyController() : Node("safety_controller") {
        this->set_parameter(rclcpp::Parameter("use_sim_time", true));

        scan_sub_ = this->create_subscription<sensor_msgs::msg::LaserScan>(
            "/scan", 10, std::bind(&SafetyController::scan_callback, this, std::placeholders::_1));

        cmd_sub_ = this->create_subscription<geometry_msgs::msg::TwistStamped>(
            "/cmd_vel_raw", 10, std::bind(&SafetyController::cmd_callback, this, std::placeholders::_1));

        cmd_pub_ = this->create_publisher<geometry_msgs::msg::TwistStamped>(
            "/diff_drive_controller/cmd_vel", 10);

        timer_ = this->create_wall_timer(100ms, std::bind(&SafetyController::timer_callback, this));
        
        last_cmd_.header.frame_id = "base_link";
    }

private:
    void timer_callback() {
        auto out_msg = last_cmd_;
        out_msg.header.stamp = this->get_clock()->now();

        if (is_blocked_ && out_msg.twist.linear.x > 0.0) {
            out_msg.twist.linear.x = 0.0;
            out_msg.twist.angular.z = 0.0;
            RCLCPP_WARN_THROTTLE(this->get_logger(), *this->get_clock(), 1000, "EMERGENCY STOP: Wall detected!");
        }

        cmd_pub_->publish(out_msg); 
    }

    void scan_callback(const sensor_msgs::msg::LaserScan::SharedPtr msg) {
        bool danger = false;
        for (float range : msg->ranges) {
            if (std::isnan(range) || std::isinf(range)) continue; 
            
            if (range <= 0.5f) { 
                danger = true;
                break;
            }
        }
        is_blocked_ = danger;
    }

    void cmd_callback(const geometry_msgs::msg::TwistStamped::SharedPtr msg) {
        last_cmd_ = *msg;
    }

    bool is_blocked_ = false;
    geometry_msgs::msg::TwistStamped last_cmd_;
    
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr scan_sub_;
    rclcpp::Subscription<geometry_msgs::msg::TwistStamped>::SharedPtr cmd_sub_;
    rclcpp::Publisher<geometry_msgs::msg::TwistStamped>::SharedPtr cmd_pub_;
};

int main(int argc, char ** argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<SafetyController>());
    rclcpp::shutdown();
    return 0;
}