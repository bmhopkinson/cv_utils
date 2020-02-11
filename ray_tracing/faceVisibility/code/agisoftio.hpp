#include <string>
#include <vector>
#include "opencv2/core/core.hpp"
#include "tinyxml2.h"
#include <vector>

using namespace tinyxml2;
using namespace std;

#ifndef AGISOFTIO_HPP
#define AGISOFTIO_HPP

//structures
typedef struct{
   float fx, fy; //focal lengths along x and y axis; in pixels
   float cx, cy; //image center
   float k1 = 0; // radial distortion coefficients - may or may not exist so initialize them
   float k2 = 0;
   float k3 = 0;
   float p1 = 0; 
   float p2 = 0;
   float skew = 0; 
} calibration;

typedef struct{
	int id;
	int width; // image width
	int height; // image height
    calibration calib;
	
} SensorAS;


typedef struct{
	int id;
	std::string label;
	int sensor_id;
	bool enabled;
	cv::Mat T;//(4, 4, CV_32FC1);
	cv::Mat Tinv;
	
} CameraAS;

//function declarations
bool readCameras(XMLElement* camerasXML, vector<CameraAS> &cameras);
bool readSensors( XMLElement* sensorsXML, vector<SensorAS> &sens);


#endif
