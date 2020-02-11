CPPFLAGS=-std=c++11 -O2 -g -I/home/brian/Documents/OpenCV/install/include -I/usr/local/include
LDFLAGS=-g
LDLIBS=-L/usr/local/lib -L/home/brian/Documents/OpenCV/install/lib -lopencv_core -lopencv_imgproc -lopencv_imgcodecs -lopencv_highgui -lopencv_videoio -Wl,-R /home/brian/Documents/OpenCV/install/lib
mono_vfe: mono_vfe.o
	g++ $(LDFLAGS) -o mono_vfe mono_vfe.o $(LDLIBS)

mono_vfe.o: mono_vfe.cpp
	g++ $(CPPFLAGS) -c mono_vfe.cpp
