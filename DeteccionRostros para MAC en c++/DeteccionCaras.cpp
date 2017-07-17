//
//  main.cpp
//  FaceDetection
//
//  Created by mac on 27/09/15.
//  Copyright © 2015 Marco Ornelas. All rights reserved.
//


#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/videoio/videoio.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/objdetect/objdetect.hpp>

using namespace std;
using namespace cv;


const string WindowName = "Detección de Caras";

class CascadeDetectorAdapter: public DetectionBasedTracker::IDetector
{
    public:
        CascadeDetectorAdapter(cv::Ptr<cv::CascadeClassifier> detector):
        IDetector(),
        Detector(detector) //Constructor
        {
            CV_Assert(detector);
        }
    
        void detect(const cv::Mat &Image, std::vector<cv::Rect> &objects)
        {
            Detector->detectMultiScale(Image, objects, scaleFactor, minNeighbours, 0, minObjSize, maxObjSize);
        }
    
        virtual ~CascadeDetectorAdapter() //Destructor
        {}
    
    private:
        CascadeDetectorAdapter();
        Ptr<CascadeClassifier> Detector;
};
/**********************************************************************************************************************/
/**********************************************************************************************************************/
int mainFace()
{
    namedWindow(WindowName);
    VideoCapture VideoStream(0);
    
    if (!VideoStream.isOpened())
    {
        printf("Error: No se puede abrir la secuencia de vídeo de la cámara\n");
        return 1;
    }
    
    string cascadeFrontalfilename = "lbpcascade_frontalface.xml"; //Cargando los patrones de los rostros
    Ptr<CascadeClassifier> cascade = makePtr<CascadeClassifier>(cascadeFrontalfilename);
    Ptr<DetectionBasedTracker::IDetector> MainDetector = makePtr<CascadeDetectorAdapter>(cascade);
    cascade = makePtr<CascadeClassifier>(cascadeFrontalfilename);
    Ptr<DetectionBasedTracker::IDetector> TrackingDetector = makePtr<CascadeDetectorAdapter>(cascade);
    DetectionBasedTracker::Parameters params;
    DetectionBasedTracker Detector(MainDetector, TrackingDetector, params);
    
    if (!Detector.run())
    {
        printf("Error: Al iniciar la detección.\n");
        return 2;
    }
    
    Mat ReferenceFrame;
    Mat GrayFrame;
    vector<Rect> Faces;
    
    while(true)
    {
        VideoStream >> ReferenceFrame;
        cvtColor(ReferenceFrame, GrayFrame, COLOR_RGB2GRAY);
        Detector.process(GrayFrame);
        Detector.getObjects(Faces);
        
        for (size_t i = 0; i < Faces.size(); i++)
            rectangle(ReferenceFrame, Faces[i], Scalar(0,255,0),3);  //Caracteristicas del rectagulo, grosor y color
        
        imshow(WindowName, ReferenceFrame);
        if (waitKey(30) >= 0) break;
    }
    Detector.stop();
    return 0;
}

const static int SENSITIVITY_VALUE = 20; //Valor de sensibilidad
const static int BLUR_SIZE = 10;  // tamaño del suavizado de la  imagen para eliminar el posible ruido
int theObject[2] = {0,0};
Rect objectBoundingRectangle = Rect(0,0,0,0);//rectángulo que bordea del objeto,utilizaa el centro de este como su posición.

/**********************************************************************************************************************/
/**********************************************************************************************************************/

int main(int , char** )
{
    //motionTracking();
    mainFace();
    return 0;
}





