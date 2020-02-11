#include <opencv2/core/core.hpp>
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
   if(argc != 4)
   {
      cout << "incorrect arguments" << endl <<"args: Left_filename Right_filename frame_interval" <<endl;
      return -1;
   }

   string Lfile = argv[1];
   string Rfile =  argv[2];
   char *endptr;
   int FRAME_INTERVAL = strtol(argv[3], &endptr, 10);

   //get filename bases for output files
   // and other basic setup
   int L_lastdot = Lfile.find_last_of(".");
   int R_lastdot = Rfile.find_last_of(".");
   string Lfile_base = Lfile.substr(0,L_lastdot);
   string Rfile_base = Rfile.substr(0,R_lastdot);

   string LoutListFile = "Left_image_list.txt";
   ofstream LoutList;
   LoutList.open(LoutListFile);
   string RoutListFile = "Right_image_list.txt";
   ofstream RoutList;
   RoutList.open(RoutListFile);
   string outfolder ="./output/";

   //open video files and get basic info about the video stream
   VideoCapture vidL(Lfile);
   VideoCapture vidR(Rfile);
   if(!vidL.isOpened()){
     cout << "Error. the Left video cannot be opened." << endl;
     return -1;
   }
   if(!vidR.isOpened()){
     cout << "Error. the Right video cannot be opened." << endl;
     return -1;
   }
   int fps = (int)vidL.get(CV_CAP_PROP_FPS);
   int frame_count = (int)vidL.get(CV_CAP_PROP_FRAME_COUNT);
   int seconds = (int)frame_count/fps;
   string secondsStr = to_string(seconds);
   cout << "frame count" << frame_count << " fps: " <<fps << "  seconds: " << frame_count/fps<<endl;

   // create windows to display video
   namedWindow("Left_Camera",WINDOW_AUTOSIZE);
   namedWindow("Right_Camera", WINDOW_AUTOSIZE);
   int speed = 100;  // 100% of normal video speed
   int speedMin = 5;  // minium speed for video frame extraction
   string time = "0";
   createTrackbar("Speed (%)", "Left_Camera",&speed,200);
   displayStatusBar("Left_Camera","Time: " + time + "/" + secondsStr);
   cout << "Press ESC to exit" <<endl;

   // run through the video extracting frames at set interval
   Mat frameL;
   Mat frameR;
   int frame_num = 0;
   int file_num = 0;
   while(1){
      if(!vidL.read(frameL) || !vidR.read(frameR))
        break;

      frame_num++;
      if(frame_num % FRAME_INTERVAL == 0) //write out images at specified intervals
      {
        file_num++;
        string numComp = to_string(file_num);
        string LfileName = Lfile_base + "_" + numComp + ".jpg";
        string RfileName = Rfile_base + "_" + numComp + ".jpg";
        string LfileFull = outfolder + LfileName;
        string RfileFull = outfolder + RfileName;
        imwrite(LfileFull, frameL);
        imwrite(RfileFull, frameR);
        LoutList << LfileName << endl;
        RoutList << RfileName << endl;
      }


      Mat frameL_resize;
      Mat frameR_resize;
      double scale = 0.35;
      resize(frameL, frameL_resize, Size(), scale, scale);
      resize(frameR, frameR_resize, Size(), scale, scale);

      imshow("Left_Camera", frameL_resize);
      imshow("Right_Camera",frameR_resize);

      time = to_string((float)frame_num/fps);
      displayStatusBar("Left_Camera","Time: " + time + "/" + secondsStr);

    // delay based on speed value set in trackbar
      int speedBounded;
      if (speed < speedMin)
        { speedBounded = speedMin ;}
      else
        { speedBounded = speed; }

      int delay = (int)(1000/((double)fps*((double)speedBounded/100)));
      if(waitKey(delay) == 27 )
      break;

   }
   RoutList.close();
   LoutList.close();
  return 0;
}
