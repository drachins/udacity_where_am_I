#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>
#include <ostream>

// Define a global client that can request services
ros::ServiceClient client;


// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;

    if(!client.call(srv)){
        ROS_ERROR("Failed to call service /ball_chaser/command_robot.");
    }    
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image& img)
{
    int white_pixel = 255;
    int count = 0;

    int rgb_cml = 0;
    int right_cml = 0;
    int left_cml = 0;
    int centre_cml = 0;
    
    

    for(int i = 0; i < img.height; i++){
        for(int k = 0; k < img.width; k++){
            for(int t = 0; t < 3; t++){
                count++;
                if(img.data[count] == 255){
                    rgb_cml++;
                    if(rgb_cml == 3){
                        if(k < 250){
                            left_cml++;
                        }
                        else if(k >= 250 && k <= 550){
                            centre_cml++;
                        }
                        else if(k > 550){
                            right_cml++;
                        }
                    }
                    
                }
                   
                    
            }

        }
            
    
    }
    

    if(left_cml > right_cml && left_cml > centre_cml){
        drive_robot(0, 0.1);
    }
    
    else if(right_cml > left_cml && right_cml > centre_cml){
        drive_robot(0, -0.1);
    }
    else if(centre_cml > left_cml && centre_cml > right_cml){
        drive_robot(0.5, 0);
    }

    else if(left_cml == right_cml == centre_cml == 0){
        drive_robot(0,0);
    }


}
int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}
