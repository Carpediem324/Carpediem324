#include "opencv2/opencv.hpp"
#include <iostream>
#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <sstream>
#include <cv_bridge/cv_bridge.h>
#include "std_msgs/Int16.h"
#include "std_msgs/Int32.h"
#include "std_msgs/Bool.h"
#include "std_msgs/Int32MultiArray.h"
#include "vision_msgs/Detection2DArray.h"
#include "vision_msgs/Detection2D.h"
#include "vision_msgs/ObjectHypothesisWithPose.h"

#define CAM_NUM 0

using namespace std;
using namespace cv::ml;
using namespace cv;

Mat frame1;
Mat sub_image1;

bool callback = false;

int detect_length = 0;
bool detect = false;

float Max_Area = 0;

// int sign[4]={0,0,0,0};
int red = 0;
int green = 0;
int greenleft = 0;
int uturn = 0;

bool A_flag = false;
bool A2_flag = false;


bool loop_flag = false;
float Area = 0;



//###########################KORUS ROS#################################
std_msgs::Int32MultiArray sign_msg;
std_msgs::Int32 sign_test_msg;
std_msgs::Bool A_flag_msg;
std_msgs::Bool A2_flag_msg;
ros::Publisher final_pub;
ros::Publisher Stop_final_pub;

ros::Publisher A_flag_pub;
ros::Publisher A2_flag_pub;


ros::Subscriber detection_sub;      //yolov7 관련
ros::Subscriber detect_length_sub;  //yolov7 관련
ros::Subscriber detect_flag_sub;    //yolov7 관련

//###################33vision_msgs ROS#########10#######################





void imageCallback(const sensor_msgs::ImageConstPtr &msg);


void detectionlength_callback(const std_msgs::Int16::ConstPtr &msg)
{
	detect_length = msg->data;
}

void detect_flag_callback(const std_msgs::Bool::ConstPtr &msg)
{
	detect = msg -> data;
}

void detections_callback(const vision_msgs::Detection2DArray::ConstPtr &detections2dArray)
{
    int isDone = false;
    int detectionsCount = 0;

	A_flag_msg.data = A_flag;
    A_flag_pub.publish(A_flag_msg);

    A2_flag_msg.data = A2_flag;
    A2_flag_pub.publish(A2_flag_msg);

   	ROS_INFO("---- ");
    ROS_INFO("Object detection message");

	sign_msg.data.clear();

	// aDetection.clear();
	red = 0;
	green = 0;
	greenleft = 0;
	uturn = 0;

    try
    {

		cout << "detect_length : " << detect_length << endl;
		cout << "loop_flag : " << loop_flag << endl;

		string Class_name;
        Max_Area = 0;

        

		for(int i = 0; i < detect_length; i++)
		{
			
			vision_msgs::Detection2D aDetection = detections2dArray->detections[i];

			
			// ROS_INFO(" ");

			// Id and confidence
			vision_msgs::ObjectHypothesisWithPose result = aDetection.results[0];
			int id = result.id;
			float score = result.score;
			// ROS_INFO("   id    %i", id);
			// ROS_INFO("   score %f", score);



			switch (id) 
			{
				case 0:
					Class_name = "green";
					break;
				case 1:
					Class_name = "red";
					break;
				case 2:
					Class_name = "greenleft";
					break;
				case 3:
					Class_name = "uturn";
					break;
				case 4:
					Class_name = "A1";
					break;
				case 5:
					Class_name = "A2";
					break;
				case 6:
					Class_name = "A3";
					break;
				case 7:
					Class_name = "B1";
					break;
				case 8:
					Class_name = "B2";
					break;
				case 9:
					Class_name = "B3";
					break;
			} 

			// Bounding boxes
			vision_msgs::BoundingBox2D boundingBox2d = aDetection.bbox;
			geometry_msgs::Pose2D center = boundingBox2d.center;
			// ROS_INFO("   position (%f, %f)", center.x, center.y);
			// ROS_INFO("   size %f x %f", boundingBox2d.size_x, boundingBox2d.size_y);

			Area = boundingBox2d.size_x * boundingBox2d.size_y;

			cout << "Class_name [ " << detectionsCount << "] : " << Class_name << " Area: " << Area << endl;

			cout << "score : " << score << endl;

			if (Class_name == "green")
            {
                green = 1;
            }
            else if (Class_name == "red")
            {
                red = 1;
            }
            else if (Class_name == "greenleft")
            {
                greenleft = 1;
            }
			else if (Class_name == "uturn")
			{
				uturn = 1;
			}
			else if (Class_name == "A1" && Area>1150 )   //1500  A1인지
            {
                A_flag=true;
                cout<<"Area: "<<Area<<endl;
            }
            
            else if (Class_name == "A2" && Area>1150 && A_flag == true) //    A1이 인지된 이후    A2인지
            {
                A2_flag=true;
                cout<<"Area: "<<Area<<endl;
            }
            

	catch (exception& e)
    {
    //   ROS_INFO("Exception %s", e.what());
      isDone = true;
    }

}

void imageCallback(const sensor_msgs::ImageConstPtr &msg)
{
	// printf("이미지 수신중\n");
	Mat image1 = cv_bridge::toCvShare(msg)->image;
	sub_image1 = image1.clone();
	cv::waitKey(1);
	callback = true;
	// imshow("image1", image1);10
}

int main(int argc, char **argv)
{
	ros::init(argc, argv, "obj_detecting");
	ros::NodeHandle nh;
	// ros::AsyncSpinner spinner(2);
	// spinner.start();
	image_transport::ImageTransport it(nh);
	image_transport::Publisher pub = it.advertise("camera/vision/image_raw", 1);
	image_transport::Subscriber sub_image = it.subscribe("/yolov7/yolov7/visualization", 1, imageCallback);
	
	detect_length_sub = nh.subscribe("/yolov7/detect_length", 1000, detectionlength_callback);
	detection_sub = nh.subscribe("/yolov7/yolov7", 10, detections_callback);
	detect_flag_sub = nh.subscribe("/yolov7/flag_pub", 10, detect_flag_callback);

	// Korus ver. topic
    //traffic_sign_pub = nh.advertise<std_msgs::Int32MultiArray>("Vision/traffic_sign", 100);

    A_flag_pub = nh.advertise<std_msgs::Bool>("Vision/A_sign", 10);
    A2_flag_pub = nh.advertise<std_msgs::Bool>("Vision/A2_sign", 10);




	ros::Rate loop_rate(25);
	sensor_msgs::ImagePtr msg;
	printf("Waiting for ---/yolov7/yolov7/visualization---\n");

	string cam_index = "/dev/video" + to_string(CAM_NUM);

	VideoCapture cap(cam_index);
	// VideoCapture cap("/home/kroad/catkin_ws/src/VISION/src/video/kcity_obj1_0918.mp4");
	cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
	cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);

	
	while (ros::ok())
	{

		cap >> frame1;
		imshow("publish_image", frame1);

		if (callback == true)
		{

			Mat img;
			cv::resize(sub_image1, img, cv::Size(640, 480), 0, 0);
			imshow("YoloV7",img);

		}

		if (detect == true)
		{
			cout<<"A_flag: "<<A_flag<< " [1 = A detect, 0 = nothing detect]" << endl;
            cout<<"A_flag2: "<<A_flag2<< " [1 = A detect, 0 = nothing detect]" << endl;

            if (A_flag == true && A2_flag == true)
            {
			    A2_flag_pub.publish(A2_flag_msg); //a2감지 퍼블리시
                cout<<"미션 종료"<<endl;
            }

		}
		else  //아무것도 감지되지않았을때
		{	
			cout << "-------------------------------------------------\n" << endl;
			cout << "\n" << endl;
			ROS_INFO("---- ");
    		ROS_INFO("Detect Nothing");


			A_flag_msg.data = A_flag;
   			A_flag_pub.publish(A_flag_msg);

            A2_flag_msg.data = A2_flag;
   			A2_flag_pub.publish(A2_flag_msg);

			cout<<"A_flag: "<<A_flag<< " [1 = A detect, 0 = nothing detect]" <<endl;
            cout<<"A2_flag: "<<A2_flag<< " [1 = A detect, 0 = nothing detect]" <<endl;


			red = 0;
			green = 0;
			greenleft = 0;
			uturn = 0;

			cout << "-------------------------------------------------\n" << endl;
			cout << "\n" << endl;
		}

		waitKey(10);   //키입력시 정지 10번키

		// ros::waitForShutdown();
		ros::spinOnce();
		loop_rate.sleep();
	}
	return 0;
}
