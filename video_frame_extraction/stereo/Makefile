CPPFLAGS=-std=c++11 -O2 -g -I/home/brian/Documents/OpenCV/install/include -I/usr/local/include
LDFLAGS=-g
LDLIBS=-L/usr/local/lib -L/home/brian/Documents/OpenCV/install/lib -lopencv_core -lopencv_imgproc -lopencv_imgcodecs -lopencv_highgui -lopencv_videoio -Wl,-R /home/brian/Documents/OpenCV/install/lib
stereo_vfe: main.o
	g++ $(LDFLAGS) -o stereo_vfe main.o $(LDLIBS)

main.o: main.cpp
	g++ $(CPPFLAGS) -c main.cpp
