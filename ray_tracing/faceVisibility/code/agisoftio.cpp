#include "agisoftio.hpp"
#include "tinyxml2.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <sstream>
#include <cstring>
#include "opencv2/core/core.hpp"

using namespace std;
using namespace tinyxml2;


bool readCameras(XMLElement* camerasXML, vector<CameraAS> &cameras){
	for( XMLElement* thisCamXML = camerasXML->FirstChildElement(); thisCamXML != NULL; thisCamXML = thisCamXML->NextSiblingElement("camera"))
	{
		CameraAS c;
		vector<float> Ttemp;
		//basic data
		thisCamXML->QueryIntAttribute("id", &c.id);
		thisCamXML->QueryIntAttribute("sensor_id", &c.sensor_id);
		thisCamXML->QueryBoolAttribute("enabled" , &c.enabled);
		const char* cLabel = thisCamXML->Attribute("label");
		c.label = cLabel; //convert c-string to c++ std::string
		
		//transform (inverse camera matrix)
		XMLElement* tran = thisCamXML->FirstChildElement("transform");
		if(tran !=NULL){  //some cameras do not get positioned, if so they are not useful
			const char* tran_cstr = tran->GetText();
			string tranString = tran_cstr;
			stringstream ss(tranString);
			ss.precision(12);  //this doesn't quite seem to work....limits of float?
			float Telm;
			while(ss  >> Telm){
				Ttemp.push_back(Telm);
			};
			
			if(Ttemp.size() != 16){
				cout << "error camera transform matrix does not have 16 elements" << endl;
				return false;
			}
			else {
				cv::Mat matSq(4, 4, CV_32FC1, Ttemp.data());
				matSq.copyTo(c.T);	
				c.Tinv = c.T.inv();			
			} 
			
			
			cameras.push_back(c);
		} //end if on transform
		
	}	
}

bool readSensors( XMLElement* sensorsXML, vector<SensorAS> &sens){
      for( XMLElement* thisSensXML = sensorsXML->FirstChildElement(); thisSensXML != NULL; thisSensXML = thisSensXML->NextSiblingElement()){
		
		SensorAS s;
		thisSensXML->QueryIntAttribute("id",&s.id);
		XMLElement* res = thisSensXML->FirstChildElement();  
        res->QueryIntAttribute("width" , &s.width );
        res->QueryIntAttribute("height", &s.height);
        
        //standard calibration coefficients
        XMLElement* calib = res->NextSiblingElement()->NextSiblingElement(); //calibration 
        XMLElement* fx = calib->FirstChildElement()->NextSiblingElement();
        fx->QueryFloatText(&s.calib.fx);
        XMLElement* fy = fx->NextSiblingElement();
        fy->QueryFloatText(&s.calib.fy);
        XMLElement* cx = fy->NextSiblingElement();
        cx->QueryFloatText(&s.calib.cx);
        XMLElement* cy = cx->NextSiblingElement();
        cy->QueryFloatText(&s.calib.cy);
        
        //optional distortion, etc coefficients
        
        XMLElement* dc = cy->NextSiblingElement();
        while(dc != 0){
			const char* ElmName = dc->Value();
			//match possible elements and assign to appropriate calib variable
	        if(strcmp(ElmName,"skew") == 0) {
				dc->QueryFloatText(&s.calib.skew);
			} 
			else if(strcmp(ElmName, "k1") == 0 ){
				dc->QueryFloatText(&s.calib.k1);
			}
			else if(strcmp(ElmName, "k2") == 0){
				dc->QueryFloatText(&s.calib.k2);
			}
			else if(strcmp(ElmName, "k3") == 0){
				dc->QueryFloatText(&s.calib.k3);
			}
			else if(strcmp(ElmName, "p1") == 0){
				dc->QueryFloatText(&s.calib.p1);
			}
			else if(strcmp(ElmName, "p2") == 0){
				dc->QueryFloatText(&s.calib.p2);
			}

			//get next element
			dc = dc->NextSiblingElement();
		}
        
        
        sens.push_back(s);
   }
}


