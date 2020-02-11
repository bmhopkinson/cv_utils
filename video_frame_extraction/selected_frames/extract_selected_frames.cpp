#include <opencv2/core/core.hpp>
#include "opencv2/imgcodecs.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <list>
#include <regex>

using namespace cv;
using namespace std;


list<int> readFrameFile(const string &filename);
string NameOnly(const string &fullPath);
string outFolder = "./output/";

int main(int argc, char ** argv)
{
   if(argc != 3)
   {
      cout << "incorrect arguments" << endl <<"args: video_filename frame_list" <<endl;
      return -1;
   }

   string videoFile = argv[1];
   string frameFile = argv[2];
   string vidBase = NameOnly(videoFile);
   
   list<int> frameNums = readFrameFile(frameFile);
   
   VideoCapture vid(videoFile); 
   if(!vid.isOpened()){
	   cout << "Could not open video file" << endl;
	   return -1;
   }
   
   Mat image;
   int idxF = 0; //frame index
   int searchF = frameNums.front();
		         frameNums.pop_front();
   while(!frameNums.empty()){
	   if(!vid.read(image)){
		   cout << "end of video but did not extract all frames" << endl;
		   break;
	   }
	   if(searchF == idxF){
		   //save frame as image
		   string imgName = outFolder + vidBase + "_" + to_string(searchF) + ".jpg";
		   imwrite(imgName, image);
		   cout << "identified frame: "<< searchF << " saved image as: " << imgName << endl;
		  
		   //get next frameId
		   searchF = frameNums.front();
					 frameNums.pop_front();
	   }
	   
	   idxF++;
   }	   
 
}

list<int> readFrameFile(const string &filename){
	list<int> frameNums;
	ifstream frames;
	frames.open(filename.c_str());
	if(!frames){cout << "could not open list of frames" << endl;}
	
	while(!frames.eof()){
		string s;
		getline(frames,s);
		
		if(!s.empty()){
		  int thisFrame = stoi(s);
		  frameNums.push_back(thisFrame);
	    }
		
	}
	return frameNums;
	
}


string NameOnly(const string &fullPath)
{
  regex eBase ("/([^/]+)$");   //matches everything after the last slash 
  smatch m1;

  regex_search(fullPath, m1, eBase);
  //string filenameWExt = m1[1];
  
  string filenameWExt;
  
  if(m1.empty()){
	  filenameWExt = fullPath; //fullPath must have only been a filename;
  } else {
	  filenameWExt = m1[1];
  }
	  
  regex eExt ("(.+)\\."); // match everything except the file extension
  smatch m2;
  regex_search(filenameWExt,m2, eExt); 
  string filename =m2[1];

  return filename; 

}
