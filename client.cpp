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
    socket.subscribe("");
    socket.connect(endpoint);

    string win = "OpticStream Viewer";
    namedWindow(win);

    while (true) {
        string buffer;
        socket.receive(buffer);
        
        vector<uint8_t> data(buffer.begin(), buffer.end());

        cv::Mat frame(data, true);
        frame = imdecode(frame, cv::IMREAD_COLOR);

        imshow(win, frame);
        if (waitKey(10) == 27) {
            cout << "ESC Pressed. Quitting." << endl;
            break;
        }
    }
    return 0;
}
