#include <iostream>
#include <opencv2/core/core.hpp>
#include "opencv2/imgcodecs.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <string.h>
#include <fstream>
#include <vector>


// usage: ./radial_undistort imageListFile.txt cameraCalibrationFile.txt
//corrects images for distortion (radial and axial), based on calibration of camera using the Matlab Camera Calbiration Toolbox (http://www.vision.caltech.edu/bouguetj/calib_doc/)
// and correction routine in OpenCV (undistort function)
// the camera calibration matrix is a 3x3 matrix with the focal length in pixels along the diagonal (and a 1 at [3,3])
// the distortion vector corrects radial distortion (elements 1,2, and 5-8; but typically  5-8 are zero)
// and axial distortion (elements 3,4).
// usage

using namespace std;
using namespace cv;

void getCalibrationData(string cameraName, Mat &cameraMatrix, Mat &distCoeffs);

int main(int argc, char ** argv)
{
 if(argc != 3)
 {
   cout << "usage ./radial_undisort Target_images_file camera_calibration_data_file "<<endl;
   return -1;
 }
    string imgListFileName  = argv[1];//"Target_images2.txt";
    string cameraName = argv[2];//"DGP1_Left_Air_calibdata.txt";
    string outfolder = "./output/";
    string infolder = "./input/";

// read in camera calibration data
    Mat cameraMatrix = Mat::eye(3,3, CV_64F);
    Mat distCoeffs   = Mat::zeros(8,1, CV_64F); //(k1,k2,p1,p2,k3,k4,k5,k6)
    getCalibrationData(cameraName, cameraMatrix, distCoeffs);

// read in names of images to process
    vector<string> imgList;
    ifstream imgListFile;
    imgListFile.open(imgListFileName);

    if(imgListFile.is_open()){
      string line;
      while(getline(imgListFile, line)){
        imgList.push_back(line);
      }
      imgListFile.close();
    }
    else {cout << "could not open " << imgListFileName << endl; return -1;}

// undistort images and write out corrected images
    for(vector<string>::iterator it = imgList.begin(); it != imgList.end(); it++)
    {
        string infile = infolder + (*it);
        Mat rawIm = imread(infile);
        Mat undistIm  = rawIm.clone();
        undistort(rawIm, undistIm, cameraMatrix, distCoeffs);

        //write out corrected image
        int lastdot = (*it).find_last_of(".");
        string basename = (*it).substr(0, lastdot);
        string outname = outfolder + basename + "_distcorr.jpg";
        cout << outname << endl;
        imwrite(outname, undistIm);
    }

    return 0;
}


//FUNCTIONS
void getCalibrationData(string cameraName, Mat &cameraMatrix, Mat &distCoeffs)
{
  ifstream calData;
  calData.open(cameraName);
  string line;
  //double d;
  if(calData.is_open()){
   while( getline(calData, line))
   {
      if(line.compare("Camera Matrix") == 0){
         getline(calData, line);
         cameraMatrix.at<double>(0,0) = stod(line);
         getline(calData, line);
         cameraMatrix.at<double>(0,1) = stod(line);
         getline(calData, line);
         cameraMatrix.at<double>(0,2) = stod(line);
         getline(calData, line);
         cameraMatrix.at<double>(1,0) = stod(line);
         getline(calData, line);
         cameraMatrix.at<double>(1,1) = stod(line);
         getline(calData, line);
         cameraMatrix.at<double>(1,2) = stod(line);
         getline(calData, line);
         cameraMatrix.at<double>(2,0) = stod(line);
         getline(calData, line);
         cameraMatrix.at<double>(2,1) = stod(line);
         getline(calData, line);
         cameraMatrix.at<double>(2,2) = stod(line);
      }
        if(line.compare("Distortion Parameters") == 0){
        getline(calData, line);
        distCoeffs.at<double>(0,0) = stod(line);
        getline(calData, line);
        distCoeffs.at<double>(1,0) = stod(line);
        getline(calData, line);
        distCoeffs.at<double>(2,0) = stod(line);
        getline(calData, line);
        distCoeffs.at<double>(3,0) = stod(line);
        getline(calData, line);
        distCoeffs.at<double>(4,0) = stod(line);
        getline(calData, line);
        distCoeffs.at<double>(5,0) = stod(line);
        getline(calData, line);
        distCoeffs.at<double>(6,0) = stod(line);
        getline(calData, line);
        distCoeffs.at<double>(7,0) = stod(line);
     }
   }
   calData.close();
  }
  else { cout << "could not open " << cameraName << endl; }
}
