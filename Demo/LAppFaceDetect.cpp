//
//  LAppFaceDetect.cpp
//  Demo
//  edit from https://github.com/HTTdesu/SimpleFacerig/blob/master/SimpleFacerig/main.cpp
//  Created by sky on 2019/9/8.
//

#include "LAppFaceDetect.hpp"
#include "LAppDefine.hpp"
#include "LAppDelegate.hpp"
#include "LAppPal.hpp"

using namespace LAppDefine;

namespace {
    LAppFaceDetect* s_instance = NULL;
};

LAppFaceDetect* LAppFaceDetect::GetInstance()
{
    if (s_instance == NULL)
    {
        s_instance = new LAppFaceDetect();
    }
    
    return s_instance;
};

void LAppFaceDetect::ReleaseInstance()
{
    if (s_instance != NULL)
    {
        delete s_instance;
    }
    
    s_instance = NULL;
};

LAppFaceDetect::LAppFaceDetect(){
    camera = new VideoCapture(0);
    camera->set(CAP_PROP_FRAME_WIDTH, 640);
    camera->set(CAP_PROP_FRAME_HEIGHT, 480);
    if (!camera->isOpened())
    {
        cerr << "Unable to connect to camera" << endl;
    }

    //模型文件路径
    modelPath = "/Users/sky/CProject/Demo/Res/";
    
//LAppPal::PrintLog("[APP]create buffer: %s ", modelPath);
//    cout<< modelPath;
    
    faceDetector.load(modelPath+"haarcascade_frontalface_alt2.xml");
    // 加载人脸检测器模型
    facemark->loadModel(modelPath+"lbfmodel.yaml");
    
};

LAppFaceDetect::~LAppFaceDetect(){
    delete facemark;
};

double LAppFaceDetect::meter(double A, double B, double C, double x, double y)
{
    double diversion = A * x + B * y + C;
    return diversion / sqrt(pow(A, 2) + pow(B, 2));
}

double LAppFaceDetect::eyebrow_move(Point2f &p1, Point2f &p2, double slope, double last, double rate)
{
    double bias = p1.y - slope * p1.x;
    double distance = sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
    double diversion = meter(slope, -1, bias, p2.x, p2.y);
    double result = (diversion / distance - 0.45) * 6;
    result = last * (1 - rate) + result * rate;
    return result;
}

double LAppFaceDetect::eye_open(Point2f &p1, Point2f &p2, Point2f &v11, Point2f &v12, Point2f &v21, Point2f &v22, double last, double rate)
{
    double distance = sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
    double slope = (double)(p2.y - p1.y) / (double)(p2.x - p1.x);
    double bias = p2.y - slope * p2.x;
    double d1 = abs(meter(slope, -1, bias, v11.x, v11.y));
    double d2 = abs(meter(slope, -1, bias, v12.x, v12.y));
    double diversion = d1 > d2 ? d1 : d2;
    d1 = abs(meter(slope, -1, bias, v21.x, v21.y));
    d2 = abs(meter(slope, -1, bias, v22.x, v22.y));
    diversion += d1 > d2 ? d1 : d2;
    double ratio = (diversion / distance - 0.18) * 8;
    ratio = ((int)(ratio * 10.0)) / 10.0;
    ratio = last * (1 - rate) + ratio * rate;
    return ratio;
}

void LAppFaceDetect::faceControl(vector<Point2f> face){
    int div_x = face.at(16).x - face.at(0).x;
    int div_y = face.at(16).y - face.at(0).y;
    double center_x = face.at(0).x + div_x / 2.0;
    double center_y = face.at(0).y + div_y / 2.0;
    double slope = (double)(div_y) / (double)(div_x);
    double bias = center_y - slope * center_x;
    double x_proj = (slope * (face.at(30).y - bias) + face.at(30).x) / (1 + pow(slope, 2));
    double y_proj = slope * x_proj + bias;
    double diversion = sqrt(pow(x_proj - face.at(0).x, 2) + pow(y_proj - face.at(0).y, 2));
    double distance = sqrt(pow(face.at(16).x - face.at(0).x, 2) + pow(face.at(16).y - face.at(0).y, 2));
    
    double rate = 0.5;
    // Ax+By+C/sqrt(A^2+B^2)
    x_rotate = x_rotate * (1 - rate) + asin(diversion / distance - 0.5) * 3.14 * 40.0 * rate;
    
    // nose to eye around 1/6 head
    diversion = meter(slope, -1, bias, face.at(30).x, face.at(30).y);
    diversion = diversion + 1.0 / 6 * distance;
    y_rotate = y_rotate * (1 - rate) + asin(diversion / distance) * 3.14 * 40.0 * rate;
    
    z_rotate = z_rotate * (1 - rate) + atan(slope) * 3.14 * 40 * rate;
    
    // eye
    left_eye = eye_open(face.at(36), face.at(39), face.at(37), face.at(38), face.at(40), face.at(41), left_eye, rate);
    right_eye = eye_open(face.at(42), face.at(45), face.at(43), face.at(44), face.at(46), face.at(47), right_eye, rate);
    
    // eyebrow
    eyebrow_left = eyebrow_move(face.at(17), face.at(19), slope, eyebrow_left, rate);
    eyebrow_right = eyebrow_move(face.at(26), face.at(24), slope, eyebrow_right, rate);
    
    // mouth
    diversion = sqrt(pow(face.at(62).x - face.at(66).x, 2) + pow(face.at(62).y - face.at(66).y, 2));
    distance = sqrt(pow(face.at(60).x - face.at(64).x, 2) + pow(face.at(60).y - face.at(64).y, 2));
    mouth_open = (diversion / distance - 0.15) * 2;
};

void LAppFaceDetect::faceDetect(){
    cout<<"faceDetect"<< endl;
    *camera >> frame;
    faces.clear();
    landmarks.clear();
    // 人脸检测
    faceDetector.detectMultiScale(gray, faces, 1.1, 3, 0, Size(160, 120), Size(320, 240));
    // 将视频帧转换至灰度图, 因为Face Detector的输入是灰度图
    cvtColor(frame, gray, COLOR_BGR2GRAY);
    bool success = facemark->fit(frame,faces,landmarks);
    if(success){
        // 如果成功, 在视频帧上绘制关键点
        if (!landmarks.empty()) {
            drawFacemarks(frame, landmarks[0], Scalar(0, 0, 255));
            faceControl(landmarks[0]);
        }else{
            //重置
            x_rotate = 0.0f;
            y_rotate = 0.0f;
            z_rotate = 0.0f;
            left_eye = 1.0f;
            right_eye = 1.0f;
            eyebrow_left = 0.0f;
            eyebrow_right = 0.0f;
            mouth_open = 0.0f;
        }
    }
    
    imshow("Facial Landmark Detection", frame);
};

