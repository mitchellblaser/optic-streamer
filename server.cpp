/*
OpticStreamer
Mitchell Blaser 2021
github.com/mitchellblaser
*/

#include <opencv2/opencv.hpp>
#include <zmqpp/zmqpp.hpp>
#include <fstream>

using namespace std;
using namespace cv;

int main(int argc, char** argv) {
    cout << "OpticStreamer" << endl << "Press ESC to exit GUI, Ctrl+C for cmdline." << endl;
    cout << "Loading Config from " << argv[1] << "." << endl;
    fstream confFile(argv[1]);
    
    VideoCapture cap(0);
    
    // DEFAULT VALUES //
    double width = cap.get(CAP_PROP_FRAME_WIDTH);
    double height = cap.get(CAP_PROP_FRAME_HEIGHT);
    bool showPrev = false;
    string endpoint = "tcp://*:8080";
    int compression = 85;
    ////////////////////

    string line;
    while (getline(confFile, line)) {
        if (line.at(0) != '#') {
            string key = line.substr(0, line.find("="));
            string val = line.substr(line.find("=")+1, line.length());
            // cout << key << " " << val << endl;

            if (key == "TARGET_WIDTH") { width = stoi(val); }
            else if (key == "TARGET_HEIGHT") { height = stoi(val); }
            else if (key == "SHOW_PREVIEW") { showPrev = (val == "True"); }
            else if (key == "STREAM_ENDPOINT") { endpoint = val; }
            else if (key == "COMPRESSION_LEVEL") { compression = stoi(val); }

        }
    }

    if (cap.isOpened() == false) {
        cout << "Error Opening Camera." << endl;
        return -1;
    }

    cap.set(CAP_PROP_FRAME_WIDTH, width);
    cap.set(CAP_PROP_FRAME_HEIGHT, height);

    string win = "OpticStream";
    namedWindow(win);

    vector<int> params;
        params.resize(9, 0);
        params[0] = IMWRITE_JPEG_QUALITY;
        params[1] = compression;
        params[2] = IMWRITE_JPEG_PROGRESSIVE;
        params[3] = 0;
        params[4] = IMWRITE_JPEG_OPTIMIZE;
        params[5] = 1;
        params[6] = IMWRITE_JPEG_RST_INTERVAL;
        params[7] = 0;

    zmqpp::context context;
    zmqpp::socket_type type = zmqpp::socket_type::pub;
    zmqpp::socket socket(context, type);
    socket.bind(endpoint);
    cout << endpoint << endl;

    while (true) {
        Mat frame;
        bool bSuccess = cap.read(frame);

        vector<uint8_t> buffer;
        stringstream ss;
        zmqpp::message m;
        imencode(".jpg", frame, buffer, params);
        // m << "Hello";
        for (auto c : buffer) ss << c;

        m << ss.str();
        socket.send(m);

        if (bSuccess == false) {
            cout << "Camera Disconnected." << endl;
            break;
        }

        if (showPrev) { imshow(win, frame); 
            if (waitKey(10) == 27) {
                cout << "ESC Pressed. Quitting." << endl;
                break;
            }
        }

    }

    // socket.close();
    return 0;
}