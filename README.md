# facerig

> 本工程实现功能是使用摄像头进行人脸检测从而控制live2d模型，工程在MacOS-Sierra 系统编译<br>
> 工程使用OpenCV及其扩展模块进行人脸关键点检测<br>
> cubism-sdk提供了live2d模型展示的demo，并提供了模型控制接口，本工程修改自该工程
> 
> The target of this project is to use the camera to detect face and control the live2d model.<br>
> The opencv and opencv_contrib modules will be used to detect face key point.<br>
> The [live2d-sdk(cubism-sdk)](https://www.live2d.com) provides a demo of live2d model display and can be used to control the model ,such as open mouse .

### preparation

> 开始构建工程前，你需要以下资源<br>
> 你的系统需要提前装好OpenCV模块

> Before build the project, You will need the following resources<br>
> The platfrom need opencv with face module installed

* [live2d sdk](https://www.live2d.com/en/download/cubism-sdk/download-native/)
* [opencv](https://opencv.org/releases/)
* [opencv_contrib](https://github.com/opencv/opencv_contrib)

### build

> 工程使用cmake构建工具，脚本文件将构建Xcode工程<br>
> 构建前需要修改CmakeLists.txt文件中cubism-sdk的目录[live2d dir](https://github.com/skygoo/faceRig/blob/2587b00a21534e60202f2cf85cac1c34d4b73f41/CMakeLists.txt#L30)
> 
> The project uses the cmake build tool, and the script file will build the Xcode project.<br>
> You need to modify the directory of cubism-sdk in the CmakeLists.txt file before building.[live2d dir](https://github.com/skygoo/faceRig/blob/2587b00a21534e60202f2cf85cac1c34d4b73f41/CMakeLists.txt#L30)
> 
> 