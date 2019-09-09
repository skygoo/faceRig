//
//  LAppFaceDetect.hpp
//  Demo
//
//  Created by sky on 2019/9/8.
//

#ifndef LAppFaceDetect_hpp
#define LAppFaceDetect_hpp

#include <opencv2/opencv.hpp>
#include <opencv2/face.hpp>

#endif /* LAppFaceDetect_hpp */
using namespace std;
using namespace cv;
using namespace cv::face;

class LAppFaceDetect{
public:
    LAppFaceDetect();
    virtual ~LAppFaceDetect();
    static LAppFaceDetect* GetInstance();
    static void ReleaseInstance();
    
    VideoCapture* camera;
    
    std::string modelPath;
    
    CascadeClassifier faceDetector;
    
    // 创建Facemark类的对象
    Ptr<Facemark> facemark = FacemarkLBF::create();
    
    //人脸容器
    vector<Rect> faces;
    
    // 人脸关键点的容器
    vector< vector<Point2f> > landmarks;
    
    float x_rotate = 0.0f;
    float y_rotate = 0.0f;
    float z_rotate = 0.0f;
    
    float left_eye = 1.0f;
    float right_eye = 1.0f;
    
    float eyebrow_left = 0.0f;
    float eyebrow_right = 0.0f;
    
    float mouth_open = 0.0f;
    
    Mat frame,gray;
    
    void faceDetect();
    
private:
    double meter(double A, double B, double C, double x, double y);
    double eyebrow_move(Point2f &p1, Point2f &p2, double slope, double last, double rate);
    double eye_open(Point2f &p1, Point2f &p2, Point2f &v11, Point2f &v12, Point2f &v21, Point2f &v22, double last, double rate);
    void faceControl(vector<Point2f> face);
    
};

