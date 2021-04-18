/*
OpticStreamer
Mitchell Blaser 2021
github.com/mitchellblaser
*/

#include <opencv2/opencv.hpp>
#include <zmqpp/zmqpp.hpp>
#include <iostream>
#include <chrono>

using namespace std;
using namespace cv;

int main(int argc, char** argv) {
    cout << "OpticStreamer Viewer" << endl << "Mitch Blaser 2021." << endl;

    int port = stoi(argv[2]);
    string addr = argv[1];

    cout << "Using Address " << addr << ":" << port << "." << endl;

    string endpoint = "tcp://" + addr + ":" + to_string(port);

    zmqpp::context context;
    zmqpp::socket_type type = zmqpp::socket_type::sub;
    zmqpp::socket socket(context, type);
    socket.set(zmqpp::socket_option::receive_timeout, 1000);
    socket.subscribe("");
    socket.connect(endpoint);

    string win = "OpticStream Viewer";
    namedWindow(win, WINDOW_NORMAL);
    resizeWindow(win, stoi(argv[3]), stoi(argv[4]));

    cv::Mat ncFrame;
    ncFrame = imread("testimage.jpg");
    putText(ncFrame, "Link DOWN. FPS:"+to_string(0)+" FRC:"+to_string(0), Point(10, 25), FONT_HERSHEY_DUPLEX, 
    0.6, Scalar(200, 200, 250), 1, 8);

    int FrameCounter = 0;
    int FPS[3] = {0, 0, 0};
    int AvgFPS = 0;
    long long Timer = chrono::system_clock::now().time_since_epoch() / chrono::milliseconds(1);
    long long TimerCache = 0;

    while (true) {
        string buffer;
        cv::Mat frame;

        Timer = chrono::system_clock::now().time_since_epoch() / chrono::milliseconds(1);
        if (Timer-TimerCache > 1000) {
            FPS[2] = FPS[1];
            FPS[1] = FPS[0];
            FPS[0] = FrameCounter;
            FrameCounter = 0;
            TimerCache = Timer;
            AvgFPS = (FPS[0] + FPS[1] + FPS[2])/3;
        }


        if (socket.receive(buffer) == 1) {
            vector<uint8_t> data(buffer.begin(), buffer.end());

            frame = cv::Mat(data, true);
            frame = imdecode(frame, cv::IMREAD_COLOR);

            putText(frame, "Link UP. FPS:"+to_string(AvgFPS)+" FRC:"+to_string(FrameCounter), Point(10, 25), FONT_HERSHEY_DUPLEX, 
            0.6, Scalar(200, 200, 250), 1, 8);

            imshow(win, frame);
            FrameCounter++;
        }
        else {
            imshow(win, ncFrame);
            FrameCounter = 0;
        }
        
        if (waitKey(1) == 27) {
            cout << "ESC Pressed. Quitting." << endl;
            break;
        }
    }
    return 0;
}
