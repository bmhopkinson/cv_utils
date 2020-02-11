#include <opencv2/core/core.hpp>
#include "opencv2/imgcodecs.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


using namespace cv;
using namespace std;

//Global variables and compiler directives

int main(int argc, char ** argv)
{
   if(argc != 3)
   {
      cout << "incorrect arguments" << endl <<"args: filename frame_interval" <<endl;
      return -1;
   }

   string filename = argv[1];
   char *endptr;
   int FRAME_INTERVAL = strtol(argv[2], &endptr, 10);

   //get filename bases for output files
   // and other basic setup
   int lastdot = filename.find_last_of(".");
   string file_base = filename.substr(0,lastdot);
   string outListFile = "image_list.txt";
   ofstream outList;
   outList.open(outListFile);
   string outfolder ="./output/";

   //open video files and get basic info about the video stream
   VideoCapture vid(filename);
   if(!vid.isOpened()){
     cout << "Error. the video cannot be opened." << endl;
     return -1;
   }

   int fps = (int)vid.get(CV_CAP_PROP_FPS);
   int frame_count = (int)vid.get(CV_CAP_PROP_FRAME_COUNT);
   int seconds = (int)frame_count/fps;
   string secondsStr = to_string(seconds);
   cout << "frame count" << frame_count << " fps: " <<fps << "  seconds: " << frame_count/fps<<endl;

   // create windows to display video
   namedWindow("Camera",WINDOW_AUTOSIZE);

   int speed = 100;
   int speedMin = 5;
   string time = "0";
   createTrackbar("Speed (%)", "Camera",&speed,200);
   displayStatusBar("Camera","Time: " + time + "/" + secondsStr);
   cout << "Press ESC to exit" <<endl;

   // run through the video extracting frames at set interval
   Mat frame;
   int frame_num = 0;
   int file_num = 0;
   while(1){
      if(!vid.read(frame))
        break;

      frame_num++;
      if(frame_num % FRAME_INTERVAL == 0) //write out images at specified intervals
      {

        file_num++;
        string numComp = to_string(file_num);
        string outfileName = file_base + "_" + numComp + ".jpg";
        string outfileFull = outfolder + outfileName;
        imwrite(outfileFull, frame);
        outList << outfileName << endl;
      }


      Mat frame_resize;
      double scale = 0.35;
      resize(frame, frame_resize, Size(), scale, scale);
      imshow("Camera", frame_resize);
      time = to_string((float)frame_num/fps);
      displayStatusBar("Camera","Time: " + time + "/" + secondsStr);

      int speedBounded;
      if (speed < speedMin)
        { speedBounded = speedMin ;}
      else
        { speedBounded = speed; }

      int delay = (int)(1000/((double)fps*((double)speedBounded/100)));
      if(waitKey(delay) == 27 )
      break;

   }
   outList.close();
  return 0;
}
