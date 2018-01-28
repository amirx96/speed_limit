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
//#define ON_PI

#include <dlib/opencv.h>
#include <opencv2/highgui/highgui.hpp>
#ifndef ON_PI
#include <dlib/gui_widgets.h>
#endif
#include <opencv2/core/cvstd.hpp>
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
using namespace dlib;
using namespace std;
using namespace cv;
using namespace cv::text;

int i = 0;

int main()
{
    
    try
    {
        cv::VideoCapture cap(0);
        if (!cap.isOpened())
        {
            cerr << "Unable to connect to camera" << endl;
            return 1;
        }

#ifndef ON_PI
        image_window win;
#endif
        //use pyramid down scanner
        typedef scan_fhog_pyramid<pyramid_down<1> > image_scanner_type;


        
       // frontal_face_detector detector = get_frontal_face_detector();
        shape_predictor pose_model;
        object_detector<image_scanner_type> detector;
    
    // look for the object_detector.svm
      std::ifstream fin("object_detector.svm",std::ios::binary); 
      deserialize(detector,fin);

      Ptr<OCRTesseract> ocr = OCRTesseract::create();

        // Grab and process frames until the main window is closed by the user.
#ifndef ON_PI
      while(!win.is_closed())
#else
        while(true)
#endif
        {
            // Grab a frame
            cv::Mat temp;
            if (!cap.read(temp))
            {
                break;
            }
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


            // Display it all on the screen
#ifndef ON_PI
            win.clear_overlay();
#endif
            if( rects.size() > 0) {
                extract_image_chip(cimg,rects[0],cropped_image);
#ifndef ON_PI
                win.set_image(cropped_image);
#endif

                Mat cropped_mat (toMat(cropped_image));
                string ocr_output;
                ocr->run(cropped_mat, ocr_output);

                cout << "hit [" << ocr_output << "]" << endl;
            } else {
if (i++ % 50 == 0) {
                cout << "miss" << endl;
                }
            }
        //    win.add_overlay(rects,rgb_pixel(0,255,0));
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


