#include <string>
#include "opencv2/core/core.hpp"
#include "agisoftio.hpp"


//structures 
typedef struct {
  size_t num_vertices;
  size_t num_faces;
  float *vertices;              /// [xyz] * num_vertices
  float *facevarying_normals;   /// [xyz] * 3(triangle) * num_faces
  float *facevarying_tangents;  /// [xyz] * 3(triangle) * num_faces
  float *facevarying_binormals; /// [xyz] * 3(triangle) * num_faces
  float *facevarying_uvs;       /// [xyz] * 3(triangle) * num_faces
  float *facevarying_vertex_colors;   /// [xyz] * 3(triangle) * num_faces
  unsigned int *faces;         /// triangle x num_faces
  unsigned int *material_ids;   /// index x num_faces
} Mesh;


struct float3 {
  float3() {}
  float3(float xx, float yy, float zz) {
    x = xx;
    y = yy;
    z = zz;
  }
  float3(const float *p) {
    x = p[0];
    y = p[1];
    z = p[2];
  }

  float3 operator*(float f) const { return float3(x * f, y * f, z * f); }
  float3 operator-(const float3 &f2) const {
    return float3(x - f2.x, y - f2.y, z - f2.z);
  }
  float3 operator*(const float3 &f2) const {
    return float3(x * f2.x, y * f2.y, z * f2.z);
  }
  float3 operator+(const float3 &f2) const {
    return float3(x + f2.x, y + f2.y, z + f2.z);
  }
  float3 &operator+=(const float3 &f2) {
    x += f2.x;
    y += f2.y;
    z += f2.z;
    return (*this);
  }
  float3 operator/(const float3 &f2) const {
    return float3(x / f2.x, y / f2.y, z / f2.z);
  }
  float operator[](int i) const { return (&x)[i]; }
  float &operator[](int i) { return (&x)[i]; }

  float3 neg() { return float3(-x, -y, -z); }

  float length() { return sqrtf(x * x + y * y + z * z); }

  void normalize() {
    float len = length();
    if (fabs(len) > 1.0e-6) {
      float inv_len = 1.0 / len;
      x *= inv_len;
      y *= inv_len;
      z *= inv_len;
    }
  }

  float x, y, z;
  // float pad;  // for alignment
};


//inline float3 operator*(float f, const float3 &v) {
//  return float3(v.x * f, v.y * f, v.z * f);
//}

inline float3 vcross(float3 a, float3 b) {
  float3 c;
  c[0] = a[1] * b[2] - a[2] * b[1];
  c[1] = a[2] * b[0] - a[0] * b[2];
  c[2] = a[0] * b[1] - a[1] * b[0];
  return c;
}


typedef struct{
	float x, y, xpin, ypin;
	
} imgPt;

typedef struct{
	float x, y, z;
} worldPt;


//function declaractions
std::string trimNewLine(std::string line);
static std::istream &safeGetline(std::istream &is, std::string &t); 
bool LoadOff(Mesh &mesh, const char *filename);
void SaveFaceImage(const char* filename, std::vector<int> data, int h, int w);
void ProjectPointToCamera(imgPt &pt, cv::Mat wpt, cv::Mat Tinv, calibration calib);
void DetermineFaceCenters(vector<cv::Mat> &Fcent, Mesh &mesh);
