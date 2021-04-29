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

    // DEFAULT VALUES //
    VideoCapture cap;
    int capturedev = 0;
    double width = cap.get(CAP_PROP_FRAME_WIDTH);
    double height = cap.get(CAP_PROP_FRAME_HEIGHT);
    int fpstarget = 30;
    bool showPrev = false;
    string endpoint = "tcp://*:5801";
    int compression = 85;
    ////////////////////

    double temp_width = 0;
    double temp_height = 0;

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
            else if (key == "TARGET_FPS") { fpstarget = stoi(val); }
            else if (key == "CAPTURE_DEVICE") { capturedev = stoi(val); }

        }
    }

    cap = cap = VideoCapture(capturedev);

    if (cap.isOpened() == false) {
        cout << "Error Opening Camera." << endl;
        return -1;
    }

    cap.set(CAP_PROP_FRAME_WIDTH, width);
    cap.set(CAP_PROP_FRAME_HEIGHT, height);
    cap.set(CAP_PROP_FPS, fpstarget);

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
    cout << "Starting Socket on " << endpoint << "." << endl;

    Mat frame;
    Mat rsframe;
    Size rsSize(width, height);

    while (true) {
        bool bSuccess = cap.read(frame);

        vector<uint8_t> buffer;
        stringstream ss;
        zmqpp::message m;

        temp_width = cap.get(CAP_PROP_FRAME_WIDTH);
        temp_height = cap.get(CAP_PROP_FRAME_HEIGHT);

        if (temp_width != width && temp_height != height) {
            resize(frame, rsframe, rsSize);
            imencode(".jpg", rsframe, buffer, params);
        } else {
            imencode(".jpg", frame, buffer, params);
        }
        
        for (auto c : buffer) ss << c;
        m << ss.str();

        socket.send(m);

        if (showPrev) { imshow(win, rsframe);
            if (waitKey(10) == 27) {
                cout << "ESC Pressed. Quitting." << endl;
                break;
            }
        }

    }

    // socket.close();
    return 0;
}
