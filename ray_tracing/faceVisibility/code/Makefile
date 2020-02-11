CPPFLAGS=-std=c++11 -O2 -g -I../../nanort-master/ -I../../nanort-master/examples/common/ -I/usr/local/include -I/home/brian/Documents/OpenCV/install/include
LDFLAGS=-g
LDLIBS=-L/usr/local/lib -L/home/brian/Documents/OpenCV/install/lib -lopencv_core -lopencv_imgproc -lopencv_features2d -lopencv_imgcodecs -lopencv_highgui -lopencv_calib3d -Wl,-rpath /home/brian/Documents/OpenCV/install/lib
faceVisibilityToCameras: faceVisibilityToCameras.o agisoftio.o tinyxml2.o
	g++ $(LDFLAGS) -o faceVisibilityToCameras faceVisibilityToCameras.o agisoftio.o tinyxml2.o $(LDLIBS)

faceVisibilityToCameras.o: faceVisibilityToCameras.cpp
	g++ $(CPPFLAGS) -c faceVisibilityToCameras.cpp

agisoftio.o: agisoftio.cpp
	g++ $(CPPFLAGS) -c agisoftio.cpp

tinyxml2.o: tinyxml2.cpp
	g++ $(CPPFLAGS) -c tinyxml2.cpp
