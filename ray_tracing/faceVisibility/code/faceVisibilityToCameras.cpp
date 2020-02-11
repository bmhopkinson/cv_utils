#include "nanort_mod.h"
#include "faceVisibilityToCameras.hpp"
#include <iostream>
#include <iomanip>
#include <istream>
#include <string>
#include <cstdio>
#include <cmath>
#include <fstream>
#include <algorithm>
#include <vector>
#include "agisoftio.hpp"
#include "tinyxml2.h"
#include "opencv2/core/core.hpp"

using namespace std;
const float NLINBUF = 0.3f;   //buffer size around image in which nonlinear corrections are considered from pinhole projection

int main(int argc, char** argv)
{
   if(argc != 3)
   {
      cout << "incorrect arguments" << endl <<"args: mesh.off cameras_agisoft.xml" <<endl;
      return -1;
   }
   //assign input arguments
	string meshfile = argv[1];
	const char* camfile = argv[2];	
	
   //load cameras
   // const char* camfile = "0453_04142017_cameras.xml";
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError eLoad = doc.LoadFile(camfile);
    if(eLoad != tinyxml2::XML_SUCCESS){
		cout << "Error could not load camera file!" << endl;
		return -1;
	}
    
    tinyxml2::XMLElement* chunk = doc.FirstChildElement()->FirstChildElement();
    const char* chunkName = chunk->Value();
    
    tinyxml2::XMLElement* sensorsXML = chunk->FirstChildElement();
    tinyxml2::XMLElement* camerasXML = sensorsXML->NextSiblingElement();
    
    vector<SensorAS> sensors;
    vector<CameraAS> cameras;
    readSensors(sensorsXML, sensors);
    cout << "loaded sensors" << endl;
    readCameras(camerasXML, cameras);
    cout << "loaded cameras" << endl;
	
		
	//load mesh
    //string meshfile = "0441_simple_2_model_off.off";
    Mesh mesh;
    bool eLoadMesh = LoadOff(mesh, meshfile.c_str());
    if(!eLoadMesh){ 
		cout << "loading mesh file failed"<<endl;
		return -1;
	}
    std::cout << "mesh faces: "<< mesh.num_faces << " and vertices: " << mesh.num_vertices << std::endl;
   
    //determine face centers
    vector<cv::Mat> Fcent;
    DetermineFaceCenters(Fcent, mesh);
     
     cout << "determined face centers " << endl;
    //project face centers into cameras
    vector< vector<int> > visibleFC(Fcent.size(), vector<int>(cameras.size())); // array indicating whether a face (rows) is visible in a camera (columns) - intialize to zero
    vector< vector< vector<float> > > imCoord(Fcent.size(), vector< vector<float>>(cameras.size(), vector<float>(2) )); //coordinates in image where face center is located. - intialize to zeros;
    for(vector<int>::size_type i = 0; i != cameras.size(); i++){ //loop over cameras
		CameraAS thisCam  = cameras[i];
		SensorAS thisSensor = sensors[cameras[i].sensor_id]; 
		int w = thisSensor.width;
		int h = thisSensor.height;
		for(vector<int>::size_type j = 0; j != Fcent.size(); j++){  //loop over face centers
			imgPt pt;
			ProjectPointToCamera(pt, Fcent[j], thisCam.Tinv, thisSensor.calib);
			if( pt.xpin > (-1.0f*NLINBUF*w) && pt.xpin < ((1.0f+NLINBUF)*w) && pt.ypin > (-1.0f*NLINBUF*h) && pt.ypin < ((1.0f+NLINBUF)*h)){  //first do sanity check to determine if pinhole projection puts world point near image - if so consider nonlinear corrections
				if(pt.x > 0 && pt.x < w && pt.y > 0 && pt.y<h){ //if yes then the point is in the image
					 visibleFC[j][i] = 1;
					 vector<float> xy = {pt.x, pt.y};
					 imCoord[j][i]   =  xy;
				}
			}
			
		} //end loop on faces
	} //end loop on cameras
	
	cout << "finished projecting face centers into cameras" <<endl;
	
    //test for line of site from camera to points
    //build Bounding Volume Hierarchy (BVH)	 	                   
	nanort::BVHBuildOptions<float> build_options; // Use default option
    //build_options.cache_bbox = false;	     
    nanort::TriangleMesh<float>    triangle_mesh(mesh.vertices, mesh.faces, sizeof(float) * 3);
    nanort::TriangleSAHPred<float> triangle_pred(mesh.vertices, mesh.faces, sizeof(float) * 3);
    nanort::BVHAccel<float, nanort::TriangleMesh<float>, nanort::TriangleSAHPred<float>, nanort::TriangleIntersector<> > accel;
    bool ret = false;
    ret = accel.Build(mesh.num_faces, build_options, triangle_mesh, triangle_pred);
    assert(ret);       

    nanort::BVHBuildStatistics stats = accel.GetStatistics();

    printf("  BVH statistics:\n");
    printf("    # of leaf   nodes: %d\n", stats.num_leaf_nodes);
    printf("    # of branch nodes: %d\n", stats.num_branch_nodes);
    printf("  Max tree depth     : %d\n", stats.max_tree_depth);
    float bmin[3], bmax[3];
    accel.BoundingBox(bmin, bmax);
    printf("  Bmin               : %f, %f, %f\n", bmin[0], bmin[1], bmin[2]);
    printf("  Bmax               : %f, %f, %f\n", bmax[0], bmax[1], bmax[2]); 
    
   //test line of sight using BVH
   for(vector<int>::size_type i = 0; i != cameras.size(); i++){ //loop over cameras
		CameraAS thisCam  = cameras[i];
		SensorAS thisSensor = sensors[cameras[i].sensor_id]; 
		float3 org;  //orgin of ray - camera center
		org[0] = thisCam.T.at<float>(0,3);
		org[1] = thisCam.T.at<float>(1,3);
		org[2] = thisCam.T.at<float>(2,3);
		
		for(vector<int>::size_type j = 0; j != Fcent.size(); j++){  //loop over face centers
			if(visibleFC[j][i] == 1){ // only check for line of sight if point might be visible
				 nanort::Ray<float> ray;
                 ray.org[0] = org[0];
                 ray.org[1] = org[1];
                 ray.org[2] = org[2];
                   
                 float3 dest;
                 dest[0] = Fcent[j].at<float>(0);
                 dest[1] = Fcent[j].at<float>(1);
                 dest[2] = Fcent[j].at<float>(2);			
			
			     float3 dir = dest - org;
			     dir.normalize();
			     ray.dir[0] = dir[0];
			     ray.dir[1] = dir[1];
			     ray.dir[2] = dir[2];
			
				 float kFar = 1.0e+30f;
			     ray.min_t = 0.0f;
			     ray.max_t = kFar;    

			      nanort::TriangleIntersector<> triangle_intersector(mesh.vertices, mesh.faces, sizeof(float) * 3);
			      nanort::BVHTraceOptions trace_options;
			      bool hit = accel.Traverse(ray, trace_options, triangle_intersector); //traverse finds closest hit along ray (i.e. direct line of sight - MultiHitTraverse will give all the interesctions
			
			     if(hit){  
					 unsigned int fid = triangle_intersector.intersection.prim_id;
					 unsigned int thisf = static_cast<unsigned int>(j);
					 if(fid != thisf){  //does first face hit (fid) equal current face - if not there is no line of sight for current face-camera pair
						 visibleFC[j][i] = 0; 
						 vector<float> xy = {0.0f, 0.0f};
						 imCoord[j][i] = xy;
					 }
				 }
				
				
			}
        } //end loop on faces
	}// end loop on cameras
	cout << "finished checking for lines of sight" << endl;
	
	FILE* f1=fopen("./output/visibleFC.txt", "w");
    for(int i = 0; i < Fcent.size(); i++){
		for(int j=0; j<cameras.size(); j++){
			fprintf(f1, "%d ", visibleFC[i][j]); 
		}
		fprintf(f1, "\n");
	}
	fclose(f1);
    

    FILE* f2=fopen("./output/imCoord.txt", "w");
    for(int i = 0; i < Fcent.size(); i++){
		for(int j=0; j<cameras.size(); j++){
			fprintf(f2, "%f  %f\t", imCoord[i][j][0], imCoord[i][j][1]); 
		}
		fprintf(f2, "\n");
	}
	fclose(f2);
	
	FILE* f3=fopen("./output/Fcenters.txt", "w");
    for(int i = 0; i < Fcent.size(); i++){
        fprintf(f3, "%f %f %f\n", Fcent[i].at<float>(0),Fcent[i].at<float>(1),Fcent[i].at<float>(2));
	}
	fclose(f2);
	
    cout<<"wrote output files "<<endl;
  delete [] mesh.vertices;  //memory allocated with new in LoadOff() 
  delete [] mesh.faces;
 
  return 0;
}


//functions 
static std::istream &safeGetline(std::istream &is, std::string &t) {
  t.clear();

  // The characters in the stream are read one-by-one using a std::streambuf.
  // That is faster than reading them one-by-one using the std::istream.
  // Code that uses streambuf this way must be guarded by a sentry object.
  // The sentry object performs various tasks,
  // such as thread synchronization and updating the stream state.

  std::istream::sentry se(is, true);
  std::streambuf *sb = is.rdbuf();

  for (;;) {
    int c = sb->sbumpc();
    switch (c) {
      case '\n':
        return is;
      case '\r':
        if (sb->sgetc() == '\n') sb->sbumpc();
        return is;
      case EOF:
        // Also handle the case when the last line has no line ending
        if (t.empty()) is.setstate(std::ios::eofbit);
        return is;
      default:
        t += static_cast<char>(c);
    }
  }
}


std::string trimNewLine(std::string line){
	  // Trim newline '\r\n' or '\n'
    if (line.size() > 0) {
      if (line[line.size() - 1] == '\n')
        line.erase(line.size() - 1);
    }
    if (line.size() > 0) {
      if (line[line.size() - 1] == '\r')
        line.erase(line.size() - 1);
    }
    return line;

}

bool LoadOff(Mesh &mesh, const char *filename){
//bool LoadOff(
   std::ifstream ifs(filename);
	if (!ifs){
		std::cout << "could not open mesh file" << std::endl;
		return false;
	}
	
	//read first line. should be 'OFF'
	std::string linebuf;
	safeGetline(ifs, linebuf);
	linebuf = trimNewLine(linebuf);
	if(linebuf != "OFF") {
		std::cout << "not a standard OFF file" <<std::endl;
		return false;
    }
	
	//second line number of vertices and faces
	safeGetline(ifs, linebuf);
	linebuf = trimNewLine(linebuf);
	int nV, nF;
	std::sscanf(linebuf.c_str(), "%d %d %*d",&nV, &nF);
	std::cout << "mesh has " << nV << " vertices and " << nF << " faces." << std::endl; 
	
	
	//load vertices
    float* vertices = new float[3*nV]; 
	for(int i = 0; i<nV; i++){
		safeGetline(ifs, linebuf);
	    linebuf = trimNewLine(linebuf);
	    float x, y, z;
	    std::sscanf(linebuf.c_str(), "%f %f %f", &x, &y, &z);
	    vertices[3*i + 0] = x;
	    vertices[3*i + 1] = y;
	    vertices[3*i + 2] = z;
	}
	
	//load triangular faces
    unsigned int* faces = new unsigned int[3*nF];
	for(int i = 0; i < nF; i++){
	    safeGetline(ifs, linebuf);
	    linebuf = trimNewLine(linebuf);
	    unsigned int f0, f1, f2;
	    std::sscanf(linebuf.c_str(), "%*d %d %d %d", &f0, &f1, &f2);
	    faces[3*i + 0] = f0;
	    faces[3*i + 1] = f1;
	    faces[3*i + 2] = f2;
	}

    mesh.vertices = vertices;
    mesh.faces = faces; 
	mesh.num_vertices = nV;
	mesh.num_faces    = nF;
	
	return true;
}

void SaveFaceImage(const char* filename, std::vector<int> data, int h, int w)
{
  FILE* f=fopen(filename, "w");
  if(!f){
    printf("Error: Couldnt open output file %s\n", filename);
    return;
  }
  for(int i = 0; i<h; i++){
	  for(int j = 0; j<w; j++){
		 int offset = j + i*w;
		 fprintf(f, "%d ",data[offset]);
	  }
	  fprintf(f,"\n");
  }
  fclose(f);
}


void ProjectPointToCamera(imgPt &pt, cv::Mat wpt, cv::Mat Tinv,calibration calib){
	
	//uses agisoft photoscan camera model (pretty standard camera model)
	
	cv::Mat one = cv::Mat::ones(1,1,CV_32FC1);
	wpt.push_back(one); //convert to homogeneous point 
	cv::Mat cpt = Tinv * wpt; //transform from world coordinates to camera coordiante system
	float xinh = cpt.at<float>(0)/cpt.at<float>(2); //scale x,y position by depth (inhomogeneous coordinates)
	float yinh = cpt.at<float>(1)/cpt.at<float>(2);
	float r   = sqrt(pow(xinh, 2) + pow(yinh, 2)); //radial distance from center of camera
	
	//project as pinhole camera (used for sanity check as nonlinear corrections lead to world points way outside of view projecting into camera
	pt.xpin = calib.cx + xinh*calib.fx;
	pt.ypin = calib.cy + yinh*calib.fy;
	
	//full camera model projection with radial distortion 
	float xtemp = xinh*(1+calib.k1*pow(r,2) + calib.k2*pow(r,4) + calib.k3*pow(r,6)) 
	              + (calib.p2*(pow(r,2) + 2.0f*pow(xinh,2)) + 2.0f*calib.p1*xinh*yinh);
	float ytemp = yinh*(1+calib.k1*pow(r,2) + calib.k2*pow(r,4) + calib.k3*pow(r,6)) 
				  + (calib.p1*(pow(r,2) + 2.0f*pow(yinh,2)) + 2.0f*calib.p2*xinh*yinh);
	pt.x = calib.cx + xtemp*calib.fx;
	pt.y = calib.cy + ytemp*calib.fy;
	
	
}

void DetermineFaceCenters(vector<cv::Mat> &Fcent, Mesh &mesh){
	cv::Mat p0 = cv::Mat(3, 1, CV_32FC1, float(0));
	cv::Mat p1 = cv::Mat(3, 1, CV_32FC1, float(0));
	cv::Mat p2 = cv::Mat(3, 1, CV_32FC1, float(0));
	
	for(size_t i = 0; i < mesh.num_faces; i++){
	
		int f0, f1, f2;
	    f0 = mesh.faces[3*i + 0];
	    f1 = mesh.faces[3*i + 1];
	    f2 = mesh.faces[3*i + 2];
	    
	    for(int j = 0; j < 3; j++){
			p0.at<float>(j,0) = mesh.vertices[3*f0 + j];
			p1.at<float>(j,0) = mesh.vertices[3*f1 + j];
			p2.at<float>(j,0) = mesh.vertices[3*f2 + j];
		}
		
		cv::Mat Fc = (p0 + p1 + p2)/3.0f;
		Fcent.push_back(Fc.clone());
		
	}
}


