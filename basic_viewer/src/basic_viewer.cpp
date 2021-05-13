#include <opencv2/core/core.hpp>
#include "opencv2/imgcodecs.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <string>
#include <iostream>
#include <deque>

using namespace std;

int calculateDelay(int speed, int speed_min, double frames_per_second);
void addFrametoBuffer(cv::Mat &frame, deque<cv::Mat> &frame_buffer);
//void callback_b0(int state,  void* user_data, bool &review);
void callback_b0(int state,  void* user_data);
void reviewRecentFrames(deque<cv::Mat> &frame_buffer,  int frame_num_current, int delay);


int main( int argc, char** argv)
{

  if(argc != 2)
   {
      cout << "incorrect arguments" << endl <<"args: filename" <<endl;
      cout << "press Ctrl+p to access buttons" << endl;
      return -1;
   }

   string filename = argv[1];
 //open video files and get basic info about the video stream
   cv::VideoCapture vid(filename, cv::CAP_GSTREAMER);  // GStreamer worked best with H.265 for me. ffmpeg and V4L2 did not work
   if(!vid.isOpened()){
     cout << "Error. the video cannot be opened." << endl;
     return -1;
   }

   double fps = vid.get(CV_CAP_PROP_FPS) ;
   int frame_count = static_cast<int>(vid.get(CV_CAP_PROP_FRAME_COUNT) );
   double duration = static_cast<double>(frame_count)/fps;
   string duration_str = to_string(duration);
   cout << "frame count" << frame_count << " fps: " <<fps << "  seconds: " << duration <<endl;

   // create windows to display video
   cv::namedWindow("Camera",cv::WINDOW_AUTOSIZE);
   int speed = 100;
   int speed_min = 5;
   string frame_num_str = "0";
   cv::createTrackbar("Speed (%)", "Camera",&speed,200);
   cv::displayStatusBar("Camera","Frame #: " + frame_num_str + "/" + to_string(frame_count) );
   bool review = false;  //indicator to enter review
   cv::createButton("review", callback_b0,  &review, CV_PUSH_BUTTON, 0 );
   cout << "Press ESC to exit" <<endl;


   // run through the video extracting frames at set interval
   cv::Mat frame;
   int frame_num = 0;

   deque<cv::Mat>  frame_buffer; 

   while( 1 ){
     if(!vid.read(frame))
        break;

      frame_num++;
      cv::Mat frame_resize;
      double scale = 0.35;
      cv::resize(frame, frame_resize, cv::Size(), scale, scale);
      cv::imshow("Camera", frame_resize);
      addFrametoBuffer(frame_resize, frame_buffer);

      cv::displayStatusBar("Camera","Frame #: " + to_string(frame_num) + "/" +  to_string(frame_count) );

     //if (review): call review function here which allows going back wards
     int delay = calculateDelay(speed, speed_min, fps);
     if(review){
        reviewRecentFrames(frame_buffer, frame_num, delay);
        review = false;
     }

      if(cv::waitKey(delay) == 27 )
         break;
   }
}


int calculateDelay(int speed, int speed_min, double frames_per_second){
   if (speed < speed_min)
      { speed = speed_min; }

   double delay = 1000 / (frames_per_second * (static_cast<double>(speed) / 100) ); //delay in ms
   return static_cast<int>(delay);   //opencv requires integer ms
 }

void addFrametoBuffer(cv::Mat &frame, deque<cv::Mat> &frame_buffer)
{
    unsigned int max_size = 10;
    if(frame_buffer.size() == max_size){
        frame_buffer.pop_back();
    }
    frame_buffer.push_front(frame.clone());
}

//void callback_b0(int state, void* user_data, bool &review){
void callback_b0(int state, void* user_data){
//flip state of review
  bool* p_review = (bool*)user_data;
  if(*p_review){
     *p_review = false;
  }
  else {
     *p_review = true;
  }

}

void reviewRecentFrames(deque<cv::Mat> &frame_buffer,  int frame_num, int delay){
   for(deque<cv::Mat>::iterator dit = frame_buffer.begin(); dit != frame_buffer.end(); ++dit, frame_num--){
     cv::imshow("Camera", *dit);
     cv::displayStatusBar("Camera","Frame #: " + to_string(frame_num) );
     if(cv::waitKey(delay) == 27 )
         break;
   }
}

