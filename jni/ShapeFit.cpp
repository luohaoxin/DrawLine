/*
 * ShapeFit.cpp
 *
 *  Created on: 2016年4月28日
 *      Author: luohaoxin
 */
#include "ShapeFit.h"
#include <iostream>
#include<math.h>
#include "Eigen/Dense"
using namespace std;
//line
#define acceptDeltaAngleSumValue 25
#define acceptDeltaAnlgeAbsSumValue 15
#define leastLineLongLengthSquare 0.0005f
//Ellipse
#define maxEllipseError 0.00007
#define maxEllipseErrorForBig 0.0001
#define EllipseBigBorder 0.2
PointF::PointF(float x, float y) {
    this->x = x;
    this->y = y;
}

PointF::PointF() {
    this->x = 0;
    this->y = 0;
}
PointF::~PointF() {
    cout<<"PointF::~PointF()"<<endl;
}
Line::Line() {
    lastAngle = 0;
    deltaAngleSum = 0;
    deltaAngleAbsSum = 0;
    inputList.clear();
}
Line::Line(const Line & c){
    this->lastAngle=c.lastAngle;
    this->deltaAngleSum=c.deltaAngleSum;
    this->deltaAngleAbsSum=c.deltaAngleAbsSum;
    this->lastPoint=c.lastPoint;
    this->inputList=c.inputList;
}
/*
 * true 表示可以继续接受input point来形成更长的直线
 */
bool Line::inputPoint(float x, float y) {
    PointF inputPoint(x, y);
    if (inputList.size() == 0) {
        inputList.push_back(inputPoint);
        lastPoint = inputPoint;
        return true;
    } else if (inputList.size() == 1) {
        float dx = inputPoint.x - lastPoint.x;
        float dy = inputPoint.y - lastPoint.y;
        if (dx == 0 && dy == 0) {
            ;
        } else {
            float angle = asin(dy / sqrt((dx * dx + dy * dy))) * 180 / M_PI;
            lastAngle = angle;
            inputList.push_back(inputPoint);
            lastPoint = inputPoint;
        }
        return true;
    } else {
        float dx = inputPoint.x - lastPoint.x;
        float dy = inputPoint.y - lastPoint.y;
        if (dx == 0 && dy == 0) {
            return true;
        } else {
            float angle = asin(dy / sqrt((dx * dx + dy * dy))) * 180 / M_PI;
            float deltaAngle = angle - lastAngle;
            float deltaAnlgeAbs = fabs(deltaAngle);
            if (deltaAnlgeAbs < acceptDeltaAnlgeAbsSumValue) {
                deltaAngleSum += deltaAngle;
                deltaAngleAbsSum += deltaAnlgeAbs;
                if (fabs(deltaAngleSum) < acceptDeltaAngleSumValue) {
                    lastAngle = angle;
                    inputList.push_back(inputPoint);
                    lastPoint = inputPoint;
                    return true;
                } else {
                    deltaAngleSum -= deltaAngle;
                    deltaAngleAbsSum -= deltaAnlgeAbs;
                    return false;
                }
            } else {
                return false;
            }
            
        }
        
    }
}
bool Line::checkLine() {
//    cout<<"run checkline";
    if (inputList.size() > 2) {
        PointF first = inputList[0];
        PointF last = inputList[inputList.size() - 1];
        float deltaX = first.x - last.x;
        float deltaY = first.y - last.y;
        if (deltaX * deltaX + deltaY * deltaY > leastLineLongLengthSquare) {
            return true;
        } else {
            reset();
            return false;
        }
    } else {
        reset();
        return false;
    }
}
void Line::reset() {
    inputList.clear();
}
ShapeFit::ShapeFit() {
    errorValue = 0;
}
void ShapeFit::inputPoint(float x, float y) {
    PointF point(x, y);
    inputList.push_back(point);
}
ShapeFit::~ShapeFit() {
    inputList.clear();
    cout<<"ShapeFit::~ShapeFit()"<<endl;
}
LineFit::LineFit(vector<PointF> input)
{
    this->inputList=input;
}
LineFit::LineFit(const LineFit & c)
{
    this->errorValue=c.errorValue;
    this->inputList=c.inputList;
    this->a=c.a;
    this->b=c.b;
    this->startPoint.x=c.startPoint.x;
    this->startPoint.y=c.startPoint.y;
    this->endPoint.x=c.endPoint.x;
    this->endPoint.y=c.endPoint.y;
}

void LineFit::compute() {
    cout<<"linefit compute:"<<endl;
    float t1 = 0, t2 = 0, t3 = 0, t4 = 0;
    float e = 0;
    for (int i = 0; i < inputList.size(); ++i) {
        t1 += inputList[i].x * inputList[i].x;
        t2 += inputList[i].x;
        t3 += inputList[i].x * inputList[i].y;
        t4 += inputList[i].y;
    }
    a = (t3 * inputList.size() - t2 * t4) / (t1 * inputList.size() - t2 * t2);
    b = (t1 * t4 - t2 * t3) / (t1 * inputList.size() - t2 * t2);
    float temp;
    for (int i = 0; i < inputList.size(); ++i) {
        temp = inputList[i].y - b - a * inputList[i].x;
        e += (temp * temp);
    }
    errorValue=e / inputList.size();
    cout<<"a:" <<a <<"b:" <<b<< "  e:"<<(e / inputList.size())<<" n:"<<inputList.size()<<endl;
    PointF first = inputList[0];
    PointF last = inputList[inputList.size() - 1];
    float deltaY;
    float deltaX;
    deltaY = fabs(first.x * a + b - first.y);
    deltaX = fabs(first.x - (first.y - b) / a);
    if (deltaY < deltaX) {
        startPoint.x=first.x;
        startPoint.y=first.x * a + b;
    } else {
        startPoint.x=(first.y - b) / a;
        startPoint.y=first.y;
    }
    
    deltaY = fabs(last.x * a + b - last.y);
    deltaX = fabs(last.x - (last.y - b) / a);
    if (deltaY < deltaX) {
        endPoint.x=last.x;
        endPoint.y=last.x * a + b;
    } else {
        endPoint.x=(last.y - b) / a;
        endPoint.y=last.y;    }
}
void LineFit::correct(){
    float angle=atan(a)*180/M_PI;
    PointF cPoint((startPoint.x+endPoint.x)/2,(startPoint.y+endPoint.y)/2);
    if(abs(angle-90)<acceptDeltaAnlgeAbsSumValue)
    {
        
    }else if(abs(angle-0)<acceptDeltaAnlgeAbsSumValue){
        
        
    }else if(abs(angle-180)<acceptDeltaAnlgeAbsSumValue)
    {
        
    }
}
void LineFit::clear() {
    inputList.clear();
}
LineFit::~LineFit() {
    cout<<"LineFit::~LineFit()"<<endl;
}
EllipseFit::EllipseFit(){
    AMatrix.resize(5, 5);
    BMatrix.resize(5, 1);
}
void EllipseFit::setInputPoint(vector<PointF> input){
    this->inputList=input;
}
void EllipseFit::compute() {
    //x^2+Bxy+Cy^2+Dx+Ey+F=0
    float x, xx, xxx, xxxx;
    float y, yy, yyy, yyyy;
    for (int i = 0; i < inputList.size(); ++i) {
        x = inputList[i].x;
        xx = x * x;
        xxx = xx * x;
        xxxx = xxx * x;
        y = inputList[i].y;
        yy = y * y;
        yyy = yy * y;
        yyyy = yyy * y;
        AMatrix(0,0) += xx * yy;
        AMatrix(0,1) += x * yyy;
        AMatrix(0,2) += xx * y;
        AMatrix(0,3) += x * yy;
        AMatrix(0,4) += x * y;
        AMatrix(1,0) += x * yyy;
        AMatrix(1,1) += yyyy;
        AMatrix(1,2) += x * yy;
        AMatrix(1,3) += yyy;
        AMatrix(1,4) += yy;
        AMatrix(2,0) += xx * y;
        AMatrix(2,1) += x * yy;
        AMatrix(2,2) += xx;
        AMatrix(2,3) += x * y;
        AMatrix(2,4) += x;
        AMatrix(3,0) += x * yy;
        AMatrix(3,1) += yyy;
        AMatrix(3,2) += x * y;
        AMatrix(3,3) += y * y;
        AMatrix(3,4) += y;
        AMatrix(4,0) += x * y;
        AMatrix(4,1) += y * y;
        AMatrix(4,2) += x;
        AMatrix(4,3) += y;
        AMatrix(4,4) += 1;
        BMatrix(0,0) += -xxx * y;
        BMatrix(1,0) += -xx * yy;
        BMatrix(2,0) += -xxx;
        BMatrix(3,0) += -xx * y;
        BMatrix(4,0) += -xx;
        
    }
    Eigen::MatrixXf XMatrix = AMatrix.inverse()*BMatrix;
    cout<<"compute "<<"X:"<<XMatrix;
    float B = XMatrix(0,0);
    float C = XMatrix(1,0);
    float D = XMatrix(2,0);
    float E = XMatrix(3,0);
    float F = XMatrix(4,0);
    xc = (B * E - 2 * C * D) / (4 * C - B * B);
    yc = (B * D - 2 * E) / (4 * C - B * B);
    
    float fenzi = 2 * (B * D * E - C * D * D - E * E + 4 * F * C - B * B * F);
    float fenmu = (B * B - 4 * C) * (C - sqrt(B * B + (1 - C) * (1 - C)) + 1);
    float femmu2 = (B * B - 4 * C) * (C + sqrt(B * B + (1 - C) * (1 - C)) + 1);
    a = sqrt(fabs(fenzi / fenmu));
    b = sqrt(fabs(fenzi / femmu2));
    
    // angle=Math.atan(B/(1-C))/2/Math.PI*180;
    angle = atan(sqrt((a * a - b * b * C) / (a * a * C - b * b)) + 0.0001) * 180 / M_PI;
    if (B > 0) {
        angle = 180 - angle;
    }
    cout<<"compute:"<< "xc:" <<xc<<"yc:" <<yc<<"a:" <<a <<"b:"<< b<< "angle:"<<angle;
    
    float e = 0;
    float temp;
    for (int i = 0; i < inputList.size(); ++i) {
        x = inputList[i].x;
        y = inputList[i].y;
        temp = x * x + B * x * y + C * y * y + D * x + E * y + F;
        e += (temp * temp);
    }
    errorValue = e / inputList.size();
    cout<<"compute:"<<"e/n:" <<(e / inputList.size())<<"n:"<<inputList.size();
    // 判断是否是椭圆，有可能是双曲线
    if (B * B - 4 * C < 0 && (D * D / 4 + E * E / 4 / C - F > 0)) {
        
        cout<<"compute:"<<"is a Ellipse";
    } else {
        cout<<"compute:"<<"is not a Ellipse";
        errorValue = 10E20;
    }
    
}
void EllipseFit::correct(){
    if(a/b<1.3)
    {
        a=(a+b)/2;
        b=a;
    }
    if(abs(xc-0.5)<0.1)
    {
        xc=0.5;
    }
    if(abs(angle-90)<acceptDeltaAnlgeAbsSumValue)
    {
        angle=90;
    }else if(abs(angle-0)<acceptDeltaAnlgeAbsSumValue){
        angle=0;
        
    }else if(abs(angle-180)<acceptDeltaAnlgeAbsSumValue)
    {
        angle=180;
    }
}
bool EllipseFit::checkEllipse(){
    if(errorValue<(a*b>EllipseBigBorder? maxEllipseErrorForBig:maxEllipseError))
    {
        return true;
    }
    else {
        return false;
    }
}
void EllipseFit::clear() {
    AMatrix=AMatrix*0;
    BMatrix=BMatrix*0;
    inputList.clear();
}
EllipseFit::~EllipseFit() {
cout<<"EllipseFit::~EllipseFit()"<<endl;
}
