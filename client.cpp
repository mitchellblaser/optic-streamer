/*
OpticStreamer
Mitchell Blaser 2021
github.com/mitchellblaser
*/

#include <opencv2/opencv.hpp>
#include <zmqpp/zmqpp.hpp>
#include <iostream>

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

    while (true) {
        string buffer;
        cv::Mat frame;

        if (socket.receive(buffer) == 1) {    
            vector<uint8_t> data(buffer.begin(), buffer.end());

            frame = cv::Mat(data, true);
            frame = imdecode(frame, cv::IMREAD_COLOR);
            imshow(win, frame);
        }
        else {
            imshow(win, ncFrame);
        }
        
        if (waitKey(10) == 27) {
            cout << "ESC Pressed. Quitting." << endl;
            break;
        }
    }
    return 0;
}
