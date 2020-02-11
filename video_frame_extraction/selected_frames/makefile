CPPFLAGS=-std=c++11 -g -I/usr/local/include 
LDFLAGS=-g
LDLIBS=-L/usr/local/lib -lopencv_core -lopencv_imgproc -lopencv_features2d -lopencv_imgcodecs -lopencv_highgui -lopencv_calib3d -lopencv_video -lopencv_videoio -Wl,-rpath /usr/local/lib 

extract_selected_frames: extract_selected_frames.o
	g++ $(LDFLAGS) -o extract_selected_frames extract_selected_frames.o $(LDLIBS)

extract_selected_frames.o: extract_selected_frames.cpp
	g++ $(CPPFLAGS) -c extract_selected_frames.cpp
