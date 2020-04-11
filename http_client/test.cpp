#include "http_connector.hpp"
#include "ipcamera_operator.hpp"
#include "3rdparty/nlohmann/json.hpp"
#include <dirent.h>
#include <vector>
#include <time.h>
#include <cstdlib>
#define CAMERA_IP "192.168.1.14"
#define SERVER_IP "171.220.244.235"

// int ids[]={123213,312,312,3,312,31122,123,412341,453,55234,8757,31231};
// int ids[]={10086};

// using namespace std;

const char *path = "/media/xiaoyao/软件/work/codes/get_face_data/BingImage/";
DIR *dir=opendir(path);

int main(int argc, char* argv[])
{
    // int ids[10] = { 0 };
    // if (argc > 1) {
    //     for (int i = 2; i < argc; i++) {
    //         ids[i] = atoi(argv[i]);
    //     }
    // }
    struct dirent *ptr;
    while((ptr = readdir(dir)) != NULL){
        if(ptr->d_name[0] == '.')
            continue;
        std::string sub_path=path;
        sub_path.append(ptr->d_name);
        std::string user_ID = ptr->d_name;
        std::cout << "===============================================\n";
        std::cout << "sub_path is: " << sub_path << std::endl;
        DIR *sub_dir=opendir(sub_path.c_str());
        struct dirent *sub_ptr;
        std::vector<std::string> sub_image_names;
        while((sub_ptr = readdir(sub_dir)) != NULL){
            if(sub_ptr->d_name[0] == '.')
            continue;
            sub_image_names.push_back(sub_path+"/"+sub_ptr->d_name);
            // printf("the image ---> %s\n", (sub_path+"/"+sub_ptr->d_name).c_str());
        }
        #if 1
        // 需要给定的摄像机URL地址
        // 补全摄像机RTSP地址
        // std::string rtsp = gz_ag::_getDeviceCommand(CAMERA_IP);
        // string url = "rtsp://admin:@192.168.1.10:554/h264/ch1/main/av_stream";
        // std::vector<std::string> file_path;
        // getFiles(path, file_path);
        // cv::Mat img = cv::imread(img_file);
        // if (img.empty()) {
        //     std::cout << "empty image: " << img_file << std::endl;
        //     return -1;
        // }
        // std::cout << "current image: " << img_file << std::endl;
        // Mat img =_getOneImg(cap);
        std::vector<cv::Mat> mat_buffer/* = gz_ag::_getSeveralImg(cap, 5)*/;
        for (int i = 0; i < 5; i++){
            cv::Mat img_temp = cv::imread(sub_image_names[i]);
            mat_buffer.push_back(img_temp);
        }
        // for (int i = 0; i < 5; i++) mat_buffer.push_back(img);
        std::cout << "Size of mat_buffer: " << mat_buffer.size() << std::endl;
        if (mat_buffer.size() < 0) {std::cout << "mat_buffer is zero"; return 0;}
        gz_ag::HttpConnector http(SERVER_IP, 8522);
        char* message_from_server = NULL;
        std::vector<std::string> img_paths;
        // int i = 1;
        // for (cv::Mat img : mat_buffer) {
        //     if (!img.empty())
        //     {
        //         http.init();
        //         // http.send_cvMat(img, &message_from_server);
        //         std::string encode_str;
        //         http.imageToBase64(img, encode_str);
        //         nlohmann::json j;
        //         j["img_base64"] = encode_str;
        //         j["usr_ID"] = id;
        //         j["img_num"] = i;
        //         http.send_json(j.dump(), &message_from_server);
        //         std::cout << "What get from server: " << (message_from_server == NULL ? "\n" : message_from_server);
        //         std::cout << std::endl;

        //         img_paths.push_back(std::string(message_from_server));
        //         i++;
        //     }
        // }
        http.init();
        nlohmann::json j;
        // j["img_base64"] = encode_str;
        // j["usr_ID"] = argv[id];a
        std::vector<std::string> b_arr;
        for (cv::Mat img : mat_buffer) {
            if (!img.empty())
            {
                // http.send_cvMat(img, &message_from_server);
                std::string encode_str;
                http.imageToBase64(img, encode_str);
                b_arr.push_back(encode_str);
            }
        }
        j["img0_base64"] = b_arr[0];
        j["img1_base64"] = b_arr[1];
        j["user_ID"] = user_ID;
        j["img_num"] = 2;
        std::cout << "start to upload the images" << std::endl;
        http.send_json(j.dump(), &message_from_server, "/face_register");
        std::cout << "What get from server: " << (message_from_server == NULL ? "\n" : message_from_server);
        std::cout << std::endl;

        free(message_from_server);

        for (std::string path : img_paths) {
            std::cout << "[INFO] Path In Server: " << path << std::endl;
        }
        #endif // SEND
    }

    

    return 0;
}
