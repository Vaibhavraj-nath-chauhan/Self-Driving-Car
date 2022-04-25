// Essential Libraries 
#include<opencv2/opencv.hpp>
#include<raspicam_cv.h>
#include<iostream>
#include<wiringPi.h>

using namespace std;
using namespace cv;
using namespace raspicam;


// Global variables
Mat frame,Matrix,framePers,frameGray,frameThresh,frameEdge,frameFinal,frameFinalDuplicate,frameFinal1,frameFinalDuplicate1;
Mat ROILane,ROILaneEnd;
RaspiCam_Cv Camera;                         
/*
Mat --> class used to store image
*/


stringstream ss;


// List to store similar data types value 
vector<int>histogramLane;
vector<int>histogramLaneEnd;
int LeftLanePos,RightLanePos;
int frameCenter,laneCenter,laneEnd;
int Result=0;


// Help to create prespective view
Point2f Source[] = {Point2f(60,160),Point2f(320,160),Point2f(20,210),Point2f(350,210)};
Point2f Destination[] = {Point2f(120,0),Point2f(320,0),Point2f(120,240),Point2f(340,240)};


// Camera setup 
void Setup(int argc,char **argv,RaspiCam_Cv &Camera){
        Camera.set(CAP_PROP_FRAME_WIDTH,("-w",argc,argv,550));
        Camera.set(CAP_PROP_FRAME_HEIGHT,("-h",argc,argv,240));
        Camera.set(CAP_PROP_BRIGHTNESS,("-br",argc,argv,50));
        Camera.set(CAP_PROP_CONTRAST,("-co",argc,argv,50));
        Camera.set(CAP_PROP_SATURATION,("-sa",argc,argv,50));
        Camera.set(CAP_PROP_GAIN,("-g",argc,argv,50));
        Camera.set(CAP_PROP_FPS,("-fps",argc,argv,0));
}


// Capturing frames
void Capture(){
    Camera.grab();
    Camera.retrieve(frame);
    cvtColor(frame,frame,COLOR_BGR2RGB);
}


// Creating prespective view
void Prespective(){
    line(frame,Source[0],Source[1],Scalar(0,0,255),2);
    line(frame,Source[1],Source[3],Scalar(0,0,255),2);
    line(frame,Source[3],Source[2],Scalar(0,0,255),2);
    line(frame,Source[2],Source[0],Scalar(0,0,255),2);


    Matrix = getPerspectiveTransform(Source,Destination);
             
    warpPerspective(frame,framePers,Matrix,Size(550,240));
}


// Helps to set the black and white envroment of the track
void Threshold(){
    cvtColor(framePers,frameGray,COLOR_RGB2GRAY);                 //Converting to gray image
    inRange(frameGray,180,255,frameThresh);                       //Change the thresh value according to brightness
    inRange(frameGray,160,255,frameGray);
    Canny(frameGray,frameEdge,170,400,3,false);                   //Detect shapre edges and draw them 
    add(frameThresh,frameEdge,frameFinal1);                       //Combinging thresh frame and canny edge 
    cvtColor(frameFinal1,frameFinal,COLOR_GRAY2RGB);          
    cvtColor(frameFinal1,frameFinalDuplicate,COLOR_GRAY2RGB);    //Used for further process
    cvtColor(frameFinal1,frameFinalDuplicate1,COLOR_GRAY2RGB);
}


// Helps to detect the actual postion of track [white lines]
void Histogram(){
    // Creating 550 [width of frame] histogram to detect track lines
    histogramLane.resize(550);
    histogramLane.clear();
    for(int i=0;i<550;i++){
        ROILane = frameFinalDuplicate(Rect(i,140,1,100));
        divide(255,ROILane,ROILane);
        histogramLane.push_back((int)(sum(ROILane)[0]));
    }
    
    // Creating 550 [width of frame] histogtame to detect the line end where we have to stop.
    histogramLaneEnd.resize(550);
    histogramLaneEnd.clear();
    for(int i=0;i<550;i++){
        ROILaneEnd = frameFinalDuplicate1(Rect(i,0,1,240));
        divide(255,ROILaneEnd,ROILaneEnd);
        histogramLaneEnd.push_back((int)(sum(ROILaneEnd)[0]));
    }
    laneEnd = sum(histogramLaneEnd)[0];
    cout<<"Lane End "<<laneEnd<<endl;
}


// Help to mark the left and right lane of the track 
void LaneFinder(){
    vector<int>::iterator LeftPtr;
    LeftPtr= max_element(histogramLane.begin(),histogramLane.begin()+260);    //Finding max intensity for left lane 
    /*
        as for black it will retrun 0 and for white it will have value more than zero
        when we get max intensity we mark that point to our left line 
        adding 260 help t mark only left lane. 
        we can have value according to our frame width
    */
    // Detecting left lane postion
    LeftLanePos = distance(histogramLane.begin(),LeftPtr);

    // Same apply for right lane 
    vector<int>::iterator RightPtr;
    RightPtr= max_element(histogramLane.begin()+260,histogramLane.end());
    RightLanePos = distance(histogramLane.begin(),RightPtr);

    // Drawing both the lanes
    line(frameFinal,Point2f(LeftLanePos,0),Point2f(LeftLanePos,240),Scalar(0,255,0),2);
    line(frameFinal,Point2f(RightLanePos,0),Point2f(RightLanePos,240),Scalar(0,255,0),2);
}


// Help to direct the motors to be in lane center [(left lane + right lane)/2]
void LaneCenter(){
    laneCenter = (RightLanePos - LeftLanePos)/2 + LeftLanePos;
    frameCenter = 226;   // Hard code 
    line(frameFinal,Point2f(laneCenter,0),Point2f(laneCenter,240),Scalar(0,255,0),3);
    line(frameFinal,Point2f(frameCenter,0),Point2f(frameCenter,240),Scalar(255,0,0),3);
    Result = laneCenter - frameCenter;    // Help to be one center lane 
}


// Passing message to ardino 
void ArdinoConnect(){
    // If we hit laneEnd more than 5500 car will stop moving 
    if (laneEnd>5500){
        // passing binary value to ardino 
            digitalWrite(21,1);
            digitalWrite(22,1);
            digitalWrite(23,1); //decimal =7
            digitalWrite(24,0);
            
            cout<<"LaneEnd"<<endl;
            
        }
    // Forward function
     else if(Result==0){
            digitalWrite(21,0);
            digitalWrite(22,0);
            digitalWrite(23,0); //decimal =0
            digitalWrite(24,0);
            cout<<"Forward"<<endl;
        }
    // Right conditions 
        else if(Result > 0 && Result<10){
            digitalWrite(21,1);
            digitalWrite(22,0);
            digitalWrite(23,0);  //decimal =1
            digitalWrite(24,0);
            cout<<"Small Right"<<endl;
        }
        else if(Result >= 10 && Result<20){
            digitalWrite(21,0);
            digitalWrite(22,1);
            digitalWrite(23,0);  //decimal =2
            digitalWrite(24,0);
            cout<<"Medium Right"<<endl;
        }
        else if(Result>20){
            digitalWrite(21,1);
            digitalWrite(22,1);
            digitalWrite(23,0);  //decimal =3
            digitalWrite(24,0);
            cout<<"Major Right"<<endl;
        }
    // Left conditions
        else if(Result < 0 && Result>-10){
            digitalWrite(21,0);
            digitalWrite(22,0);
            digitalWrite(23,1);  //decimal =4
            digitalWrite(24,0);
            cout<<"Small Left"<<endl;
        }
        else if(Result <= -10 && Result>-20){
            digitalWrite(21,1);
            digitalWrite(22,0);
            digitalWrite(23,1);  //decimal =5
            digitalWrite(24,0);
            cout<<"Medium Left"<<endl;
        }
        else if(Result<-20){
            digitalWrite(21,0);
            digitalWrite(22,1);
            digitalWrite(23,1);  //decimal =6
            digitalWrite(24,0);
            cout<<"Major Left"<<endl;
        }
}



int main(int argc,char **argv){
    
    // init wiring pi
    wiringPiSetup();
    // Setting the output pins of pi 
    pinMode(21,OUTPUT);
    pinMode(22,OUTPUT);
    pinMode(23,OUTPUT);
    pinMode(24,OUTPUT);

    //Setting camera views
    Setup(argc,argv,Camera);
    cout<<"Conntecting to camera"<<endl;
    if(!Camera.open()){
        cout<<"Failed to connect"<<endl;
        return -1;
 
    }
    Camera.set(cv::CAP_PROP_FORMAT, CV_32F);
    cout<<"Camera ID="<<Camera.getId()<<endl;
    while(1){
        Capture();
        Prespective();
        Threshold();
        Histogram();
        LaneFinder();
        LaneCenter();
        ArdinoConnect(); 
        
        ss.str(" ");
        ss.clear();
        ss<<"Result="<<Result;
        putText(frame,ss.str(),Point2f(1,50),0,1,Scalar(0,0,255),2);
        
        // Displaying the frames 
        namedWindow("Original",WINDOW_KEEPRATIO);
        moveWindow("Original",50,100);
        resizeWindow("Original",640,480);
        imshow("Original",frame);
        

        namedWindow("frameFinal",WINDOW_KEEPRATIO);
        moveWindow("frameFinal",700,100);
        resizeWindow("frameFinal",640,480);
        imshow("frameFinal",frameFinal);
        waitKey(1);
    }
    return 0;
}


