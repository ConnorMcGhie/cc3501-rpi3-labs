#include <opencv2/opencv.hpp>
#include <sys/time.h>

// HSV threshold values - adjusted live via trackbars
int hMin = 0, hMax = 179;
int sMin = 0, sMax = 255;
int vMin = 0, vMax = 255;

//Morphology kernel size - adjustable via trackbar too
int morphSize = 3;

int main()
{
    // Open the video camera.
    std::string pipeline = "libcamerasrc"
        " ! video/x-raw, width=800, height=600" // camera needs to capture at a higher resolution
        " ! videoconvert"
        " ! videoscale"
        " ! video/x-raw, width=400, height=300" // can downsample the image after capturing
        " ! videoflip method=rotate-180" // remove this line if the image is upside-down
        " ! appsink drop=true max_buffers=2";
        
    cv::VideoCapture cap(pipeline, cv::CAP_GSTREAMER);
    if(!cap.isOpened()) {
        printf("Could not open camera.\n");
        return 1;
    }

    // Create windos
    cv::namedWindow("Camera", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Thresholded", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Controls", cv::WINDOW_AUTOSIZE);
    
    // Create trackbars (sliders) for interactive HSV threshold adjustment
    cv::createTrackbar("H min", "Controls", &hMin, 179);
    cv::createTrackbar("H max", "Controls", &hMax, 179);
    cv::createTrackbar("S min", "Controls", &sMin, 255);
    cv::createTrackbar("S max", "Controls", &sMax, 255);
    cv::createTrackbar("V min", "Controls", &vMin, 255);
    cv::createTrackbar("V max", "Controls", &vMax, 255);
    
    
    cv::Mat frame, hsv, mask;

    // Measure the frame rate - initialise variables
    int frame_id = 0;
    timeval start, end;
    gettimeofday(&start, NULL);

    for(;;) {
        if (!cap.read(frame)) {
            printf("Could not read a frame.\n");
            break;
        }
        
        // Convert to HSV colour space
        cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);
        
        // Threshold based on teh slider-controlled HSV range
        cv::inRange(hsv,
                    cv::Scalar(hMin, sMin, vMin),
                    cv::Scalar(hMax, sMax, vMax),
                    mask);
                    
        printf("frame: %dx%d type=%d | mask: %dx%d nonzero=%d\n",
                frame.cols, frame.rows, frame.type(),
                mask.cols, mask.rows, cv::countNonZero(mask));
                    
        // Morphological open then close to clean up the thresholded image
        int size = std::max(1, morphSize);
        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE,
                                                    cv::Size(2 * size + 1, 2 * size + 1));
        cv::morphologyEx(mask, mask, cv::MORPH_OPEN, kernel);
        cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, kernel);
        
        // Locate the object using image moments -> centre of mass
        cv::Moments m = cv::moments(mask, true);
        if (m.m00 > 0) {
            double cx = m.m10 / m.m00;
            double cy = m.m01 / m.m00;
            
            printf("Object detected at (x=%.1f, y=%.1f), area=%.0f px\n", cx, cy, m.m00);
            
            // Draw a marker on the camera view at the detected centroid
            cv::circle(frame, cv::Point((int)cx, (int)cy), 8, cv::Scalar(0, 0, 255), 2);
            cv::line(frame, cv::Point((int)cx -12, (int)cy), cv::Point((int)cx +12, (int)cy), cv::Scalar(0, 0, 255), 1);
            cv::line(frame, cv::Point((int)cx, (int)cy -12), cv::Point((int)cx, (int)cy + 12), cv::Scalar(0, 0, 255), 1);
            
        } else {
            printf("No object detected.\n");
        }
            
        
        //show frame
        cv::imshow("Camera", frame);
        cv::waitKey(1);

        // Measure the frame rate
        frame_id++;
        if (frame_id >= 30) {
            gettimeofday(&end, NULL);
            double diff = end.tv_sec - start.tv_sec + (end.tv_usec - start.tv_usec)/1000000.0;
            printf("30 frames in %f seconds = %f FPS\n", diff, 30/diff);
            frame_id = 0;
            gettimeofday(&start, NULL);
        }
    }

    // Free the camera 
    cap.release();
    return 0;
}

