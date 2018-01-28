// The contents of this file are in the public domain. See LICENSE_FOR_EXAMPLE_PROGRAMS.txt
/*

    This example program shows how to find frontal human faces in an image and
    estimate their pose.  The pose takes the form of 68 landmarks.  These are
    points on the face such as the corners of the mouth, along the eyebrows, on
    the eyes, and so forth.  
    

    This example is essentially just a version of the face_landmark_detection_ex.cpp
    example modified to use OpenCV's VideoCapture object to read from a camera instead 
    of files.


    Finally, note that the face detector is fastest when compiled with at least
    SSE2 instructions enabled.  So if you are using a PC with an Intel or AMD
    chip then you should enable at least SSE2 instructions.  If you are using
    cmake to compile this program you can enable them by using one of the
    following commands when you create the build project:
        cmake path_to_dlib_root/examples -DUSE_SSE2_INSTRUCTIONS=ON
        cmake path_to_dlib_root/examples -DUSE_SSE4_INSTRUCTIONS=ON
        cmake path_to_dlib_root/examples -DUSE_AVX_INSTRUCTIONS=ON
    This will set the appropriate compiler options for GCC, clang, Visual
    Studio, or the Intel compiler.  If you are using another compiler then you
    need to consult your compiler's manual to determine how to enable these
    instructions.  Note that AVX is the fastest but requires a CPU from at least
    2011.  SSE4 is the next fastest and is supported by most current machines.  
*/
#define ON_PI

#include <dlib/opencv.h>
#include <opencv2/highgui/highgui.hpp>
#include <stdexcept>
#ifndef ON_PI
#include <dlib/gui_widgets.h>
#endif
//#include <opencv2/core/cvstd.hpp>
#include <opencv2/text/ocr.hpp>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/svm_threaded.h>
#include <dlib/string.h>
#include <dlib/data_io.h>
#include<dlib/cmd_line_parser.h>
#include <iostream>
#include <dlib/image_transforms.h>
#include <fstream>
#include "grpc_stuff.cpp"
using namespace dlib;
using namespace std;
using namespace cv;
using namespace cv::text;

string escape(string input) {
  string output;
  for (int j = 0; j < input.length(); ++j) {
    if ('\n' == input[j]) {
      output.push_back('\\');
      output.push_back('n');
      } else if ('\t' == input[j]) {
        output.push_back('\\');
        output.push_back('t');
      } else {
        output.push_back(input[j]);
      }
  }
  return output;
}

int get_speed(string input)
{
  string numbers;
  for (int j = 0; j < input.length(); ++j) {
    if ('0' <= input[j] && input[j] <= '9') {
      numbers.push_back(input[j]);
    }
  }
  int speed = atoi(numbers.c_str());
  if (speed % 5 == 0 && 5 <= speed && speed <= 85) {
    return speed;
  } else {
    return 0;
  }
}

int main() {
    try {
        cv::VideoCapture cap(0);
        if (!cap.isOpened()) {
          throw runtime_error("Unable to connect to camera");
        }

        //use pyramid down scanner
        typedef scan_fhog_pyramid<pyramid_down<1> > image_scanner_type;

       // frontal_face_detector detector = get_frontal_face_detector();
        shape_predictor pose_model;
        object_detector<image_scanner_type> detector;
    
        // look for the object_detector.svm
        std::ifstream fin ("object_detector.svm", std::ios::binary); 
        deserialize(detector,fin);

        std::ofstream log ("strings.out");

        Ptr<OCRTesseract> ocr = OCRTesseract::create();

        // Grab and process frames until the main window is closed by the user.
#ifndef ON_PI
        image_window win;
        while(!win.is_closed())
#else
        StateClient state_client (grpc::CreateChannel(
            "localhost:50051", 
            grpc::InsecureChannelCredentials()
        ));
        while(true)
#endif
        {
            // Grab a frame
            cv::Mat temp;
            cap.read(temp);

            // Turn OpenCV's Mat into something dlib can deal with.  Note that this just
            // wraps the Mat object, it doesn't copy anything.  So cimg is only valid as
            // long as temp is valid.  Also don't do anything to temp that would cause it
            // to reallocate the memory which stores the image as that will make cimg
            // contain dangling pointers.  This basically means you shouldn't modify temp
            // while using cimg.
            cv_image<rgb_pixel> cimg(temp);

            // Detect speed limit sign
            std::vector<dlib::rectangle> rects = detector(cimg);
            array2d<unsigned char> t_image;
            array2d<unsigned char> cropped_image;

#ifndef ON_PI
            // Display it all on the screen
            win.clear_overlay();
#endif
            if (rects.size() > 0) {
              // get rectangle of sign
              extract_image_chip(cimg,rects[0],cropped_image);

              // up scale image
              pyramid_up(cropped_image);

#ifndef ON_PI
              win.set_image(cropped_image);
#endif

              Mat cropped_mat (toMat(cropped_image));
              string ocr_output;
              ocr->run(cropped_mat, ocr_output);

              log << escape(ocr_output) << std::endl;

              int speed = get_speed(ocr_output);
              if (speed == 0) {
                cout << "*" << std::flush;
              } else {
#ifdef ON_PI
                state_client.speed_limit_update(speed);
#endif
                cout << endl << speed << endl;
              }
            } else {
              cout << "." << std::flush;
            }
        }
    }
    catch(serialization_error& e)
    {
        cout << "You need dlib's default face landmarking model file to run this example." << endl;
        cout << "You can get it from the following URL: " << endl;
        cout << "   http://dlib.net/files/shape_predictor_68_face_landmarks.dat.bz2" << endl;
        cout << endl << e.what() << endl;
    }
    catch(exception& e)
    {
        cout << e.what() << endl;
    }
}
