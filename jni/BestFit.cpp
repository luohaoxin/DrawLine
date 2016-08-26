/*
 * luohaoxin.cpp
 *
 *  Created on: 2016年4月26日
 *      Author: luohaoxin
 */

#include"BestFit.h"
#include<iostream>
#include<math.h>
#include<vector>
#include<list>
#include<deque>
#include "Eigen/Dense"
using Eigen::MatrixXf;
using namespace std;
#define MAX_LINE_ANGLE_DIFFER 25
#define leastLineLongLengthSquare 0.1f
#define maxLineFitListErrorValue 0.0005f
#define maxShapeFitListErrorValue 0.0005f
#define SquareLengthWidthRate 1.3f
BestFit::BestFit() {
}
BestFit::~BestFit()
{
    
}
void BestFit::inputPoint(float x, float y, bool isFinished) {
    //    cout<<"inputpoint:"<<x<<" "<<y<<" "<<isFinished;
    bool canInputAgain = currentLine.inputPoint(x, y);
    //    cout<<" canInputAgain:"<<canInputAgain<<endl;
    if (isFinished || !canInputAgain) {
        if (currentLine.checkLine()) {
            lineList.push_back(currentLine);
            currentLine.reset();
        }
    }
    if (isFinished) {
        //        cout<<"isfinished"<<lineList.size();
        for (int i = 0; i < lineList.size(); i++) {
            LineFit lineFit(lineList[i].inputList);
            lineFit.compute();
            lineFitList.push_back(lineFit);
        }
        while (mergeSimilarLines()) {
        }
    }
}

void BestFit::startPoint(float x, float y) {
    sPoint.x=x;
    sPoint.y=y;
    reset();
    inputPoint(x, y, false);
    ellipseFit.updateMinXY(x, y, true);
    ellipseFit.inputPoint(x, y);
}
void BestFit::updatePoint(float x, float y) {
    inputPoint(x, y, false);
    ellipseFit.updateMinXY(x, y, false);
    ellipseFit.inputPoint(x, y);
}
float * BestFit::finishPoint(float x, float y) {
    fPoint.x=x;
    fPoint.y=y;
    inputPoint(x, y, true);
    ellipseFit.updateMinXY(x, y, false);
    ellipseFit.inputPoint(x, y);
    
    float deltaX=sPoint.x-fPoint.x;
    float deltaY=sPoint.y-fPoint.y;
    if(deltaX*deltaX+deltaY*deltaY>leastLineLongLengthSquare)
    {
        isClosed=false;
    }
    else{
        isClosed=true;
    }
    return getResult();
}
bool BestFit::mergeSimilarLines() {
    bool hasMerged = false;
    if (lineFitList.size() <= 1) {
        hasMerged = false;
    } else if (lineFitList.size() == 2) {
        LineFit one = lineFitList[0];
        LineFit two = lineFitList[1];
        hasMerged = mergeLinesIfNeed(0, 1);
    } else {
        for (int i = 0; i < lineFitList.size(); i++) {
            if (mergeLinesIfNeed(i, i == lineFitList.size() - 1 ? 0 : i + 1)) {
                hasMerged = true;
            }
        }
    }
    return hasMerged;
}

bool BestFit::mergeLinesIfNeed(int oneIndex, int twoIndex) {
    LineFit * one = &(lineFitList[oneIndex]);
    LineFit * two = &(lineFitList[twoIndex]);
    if (fabs((atan(one->a) - atan(two->a))) * 180 / M_PI < MAX_LINE_ANGLE_DIFFER) {
        vector<PointF> mergedPoints;
        one->inputList.insert(one->inputList.end(), two->inputList.begin(),
                              two->inputList.end());
        one->compute();
        lineFitList.erase(lineFitList.begin()+twoIndex);
        return true;
    } else {
        return false;
    }
}

float * BestFit::getLine(){
    float * result;
    LineFit lineFit(ellipseFit.inputList);
    lineFit.compute();
    lineFit.setTwoPoint();
    lineFit.correct();
    if(isBeyondBond(lineFit.startPoint.x, lineFit.startPoint.y)||
       isBeyondBond(lineFit.endPoint.x, lineFit.endPoint.y)){
        result = NULL;
        return result;
    }
    
    result = new float[5];
    result[0] = 1;
    result[1] = lineFit.startPoint.x;
    result[2] = lineFit.startPoint.y;
    result[3] = lineFit.endPoint.x;
    result[4] = lineFit.endPoint.y;
    return result;
    
}
float * BestFit::getTriangle()
{
    float * result;
    PointF onePoint=getCrossPoint(&lineFitList[0], &lineFitList[1]);
    PointF twoPoint=getCrossPoint(&lineFitList[1], &lineFitList[2]);
    PointF threePoint=getCrossPoint(&lineFitList[2], &lineFitList[0]);
    if(isBeyondBond(onePoint.x, onePoint.y)||
       isBeyondBond(twoPoint.x, twoPoint.y)||
       isBeyondBond(threePoint.x, threePoint.y)){
        result = NULL;
        return result;
    }
    bool hasCorrect1=correctTwoPoints(&onePoint, &twoPoint);
    bool hasCorrect2=correctTwoPoints(&twoPoint, &threePoint);
    bool hasCorrect3=correctTwoPoints(&threePoint, &onePoint);
    result=new float[7];
    result[0]=3;
    result[1]=onePoint.x;
    result[2]=onePoint.y;
    result[3]=twoPoint.x;
    result[4]=twoPoint.y;
    result[5]=threePoint.x;
    result[6]=threePoint.y;
    return result;
}
float * BestFit::getRectangle()
{
    float * result;
    PointF onePoint=getCrossPoint(&lineFitList[0], &lineFitList[1]);
    PointF twoPoint=getCrossPoint(&lineFitList[1], &lineFitList[2]);
    PointF threePoint=getCrossPoint(&lineFitList[2], &lineFitList[3]);
    PointF fourPoint=getCrossPoint(&lineFitList[3], &lineFitList[0]);
    if(isBeyondBond(onePoint.x, onePoint.y)||
       isBeyondBond(twoPoint.x, twoPoint.y)||
       isBeyondBond(threePoint.x, threePoint.y)||
       isBeyondBond(fourPoint.x, fourPoint.y)){
        result = NULL;
        return result;
    }
    
    bool hasCorrect1=correctTwoPoints(&onePoint, &twoPoint);
    bool hasCorrect2=correctTwoPoints(&twoPoint, &threePoint);
    bool hasCorrect3=correctTwoPoints(&threePoint, &fourPoint);
    bool hasCorrect4=correctTwoPoints(&fourPoint, &onePoint);
    if(hasCorrect1&&hasCorrect2&&hasCorrect3&&hasCorrect4){
        float distance1=(twoPoint.y-onePoint.y)*(twoPoint.y-onePoint.y)+(twoPoint.x-onePoint.x)*(twoPoint.x-onePoint.x);
        distance1=sqrt(distance1);
        float distance2=(threePoint.y-twoPoint.y)*(threePoint.y-twoPoint.y)+(threePoint.x-twoPoint.x)*(threePoint.x-twoPoint.x);
        distance2=sqrt(distance2);
        float rate;
        float targetDistance=(distance1+distance2)/2;
        if(distance1>=distance2){
            rate=distance1/distance2;
        }else{
            rate=distance2/distance1;
        }
        if(rate<SquareLengthWidthRate){
            if(onePoint.x==twoPoint.x){
                onePoint.y=(onePoint.y-twoPoint.y)*targetDistance/distance1+twoPoint.y;
                fourPoint.y=onePoint.y;
                threePoint.x=(threePoint.x-twoPoint.x)*targetDistance/distance2+twoPoint.x;
                fourPoint.x=threePoint.x;
            }else{
                onePoint.x=(onePoint.x-twoPoint.x)*targetDistance/distance1+twoPoint.x;
                fourPoint.x=onePoint.x;
                threePoint.y=(threePoint.y-twoPoint.y)*targetDistance/distance2+twoPoint.y;
                fourPoint.y=threePoint.y;
            }
        }
    }
    result=new float[9];
    result[0]=4;
    result[1]=onePoint.x;
    result[2]=onePoint.y;
    result[3]=twoPoint.x;
    result[4]=twoPoint.y;
    result[5]=threePoint.x;
    result[6]=threePoint.y;
    result[7]=fourPoint.x;
    result[8]=fourPoint.y;
    return result;
}
PointF BestFit::getCrossPoint(LineFit * oneLine,LineFit * twoLine)
{
    //    float[][] Afloat = { { a, -1 }, { other.a, -1 } };
    //    float[][] Bfloat = { { -b }, { -other.b } };
    //    Matrix AMatrix = new Matrix(Afloat);
    //    Matrix BMatrix = new Matrix(Bfloat);
    //    Matrix XMatrix = AMatrix.solve(BMatrix);
    Eigen::MatrixXf AMatrix(2,2);
    AMatrix(0,0)=oneLine->a;
    AMatrix(0,1)=-1;
    AMatrix(1,0)=twoLine->a;
    AMatrix(1,1)=-1;
    Eigen::MatrixXf BMatrix(2,1);
    BMatrix(0,0)=-oneLine->b;
    BMatrix(1,0)=-twoLine->b;
    Eigen::MatrixXf XMatrix=AMatrix.inverse()*BMatrix;
    PointF result;
    result.x=XMatrix(0,0);
    result.y=XMatrix(1,0);
    return result;
}
float BestFit::getLineFitListErrorValue(){
    float errorSum=0;
    float error=0;
    float a,b;
    float temp;
    for (int i=0;i<ellipseFit.inputList.size();i++) {
        PointF point=ellipseFit.inputList[i];
        float x=point.x;
        float y=point.y;
        
        
        for (int j=0; j<lineFitList.size(); j++) {
            a=lineFitList[j].a;
            b=lineFitList[j].b;
            if(j==0)
            {
                error=(a*x-y+b)*(a*x-y+b)/(a*a+1);
            }
            else{
                temp=(a*x-y+b)*(a*x-y+b)/(a*a+1);
                error=error<temp? error:temp;
            }
        }
        errorSum+=error;
        
    }
    return errorSum/ellipseFit.inputList.size();
}
bool BestFit::isBeyondBond(float x,float y){
    float acceptDeltaX=(ellipseFit.maxX-ellipseFit.minX)/2;
    float acceptDeltaY=(ellipseFit.maxY-ellipseFit.minY)/2;
    if(x>ellipseFit.maxX&&x-ellipseFit.maxX>acceptDeltaX){
        return true;
    }
    if(x<ellipseFit.minX&&ellipseFit.minX-x>acceptDeltaX){
        return true;
    }
    if(y>ellipseFit.maxY&&y-ellipseFit.maxY>acceptDeltaY){
        return true;
    }
    if(y<ellipseFit.minY&&ellipseFit.minY-y>acceptDeltaY){
        return true;
    }
    return false;
}
//remeber to delete [] result ,after you use result
float * BestFit::getResult() {
    float * result;
    float lineerror=getLineFitListErrorValue();
    if (lineFitList.size()
        == 1&&getLineFitListErrorValue()<maxLineFitListErrorValue) {
        result=getLine();
        if(result){
            cout<<"line";
            return result;
        }
    }
    if (lineFitList.size()
        == 4&&getLineFitListErrorValue()<maxShapeFitListErrorValue) {
        result = getRectangle();
        if(result){
            cout << "rectangle";
            return result;
        }
    }
    if (lineFitList.size()
        == 3&&getLineFitListErrorValue()<maxShapeFitListErrorValue) {
        result = getTriangle();
        if(result){
            cout << "triangle";
            return result;
        }
    }
    ellipseFit.compute();
    
    
    if (ellipseFit.checkEllipse()) {
        float x1=ellipseFit.xc+ellipseFit.a*cos(ellipseFit.angle);
        float y1=ellipseFit.yc+ellipseFit.a*sin(ellipseFit.angle);
        float x2=ellipseFit.xc-ellipseFit.a*cos(ellipseFit.angle);
        float y2=ellipseFit.yc-ellipseFit.a*sin(ellipseFit.angle);
        float x3=ellipseFit.xc+ellipseFit.b*cos(ellipseFit.angle);
        float y3=ellipseFit.yc+ellipseFit.b*sin(ellipseFit.angle);
        float x4=ellipseFit.xc-ellipseFit.b*cos(ellipseFit.angle);
        float y4=ellipseFit.yc-ellipseFit.b*sin(ellipseFit.angle);
        if(isBeyondBond(x1, y1)||
           isBeyondBond(x2, y2)||
           isBeyondBond(x3, y3)||
           isBeyondBond(x4, y4)){
            result = new float[1];
            result[0] = 0;
            cout << "not fit";
        }else{
            ellipseFit.correct();
            result = new float[6];
            result[0] = 2;
            result[1] = ellipseFit.xc;
            result[2] = ellipseFit.yc;
            result[3] = ellipseFit.a;
            result[4] = ellipseFit.b;
            result[5] = ellipseFit.angle;
            cout << "ellipse";
        }
    } else {
        
        result = new float[1];
        result[0] = 0;
        cout << "not fit";
        
    }
    return result;
}
void BestFit::reset(){
    lineFitList.clear();
    lineList.clear();
    ellipseFit.clear();
    
}
int main(int argc, char *argv[]) {
    BestFit fit;
    float * result;
    fit.startPoint(0.686187,0.416736);
    fit.updatePoint(0.685924,0.416736);
    fit.updatePoint(0.681967,0.426759);
    fit.updatePoint(0.678538,0.445222);
    fit.updatePoint(0.678011,0.483467);
    fit.updatePoint(0.680121,0.498237);
    fit.updatePoint(0.683814,0.507732);
    fit.updatePoint(0.688561,0.513271);
    fit.updatePoint(0.697001,0.517228);
    fit.updatePoint(0.706760,0.516964);
    fit.updatePoint(0.717575,0.511952);
    fit.updatePoint(0.726542,0.503776);
    fit.updatePoint(0.735510,0.489533);
    fit.updatePoint(0.741576,0.470806);
    fit.updatePoint(0.744741,0.449442);
    fit.updatePoint(0.745533,0.430451);
    fit.updatePoint(0.744214,0.415681);
    fit.updatePoint(0.741313,0.405658);
    fit.updatePoint(0.737620,0.400647);
    fit.updatePoint(0.729971,0.396954);
    fit.updatePoint(0.720212,0.396427);
    fit.updatePoint(0.709398,0.399328);
    fit.updatePoint(0.700694,0.403812);
    fit.updatePoint(0.693045,0.411197);
    fit.updatePoint(0.685660,0.424649);
    
    
    result=fit.finishPoint(0.685660,0.424649);
    
    cout<<"yuyuyuyuyuyuyuyuyuyuyuyuyuyu"<<result[0]<<endl;
    delete [] result;
    
    
//    for (int i=0; i<100000; i++) {
//        fit.startPoint(0.1,0.1);
//        fit.updatePoint(0.2,0.2);
//        fit.updatePoint(0.21,0.22);
//        fit.updatePoint(0.314,0.32);
//        fit.updatePoint(0.41,0.424);
//        fit.updatePoint(0.514,0.52);
//        fit.updatePoint(0.55, 0.554);
//        result= fit.finishPoint(0.554, 0.454);
//        delete [] result;
//    }
    //    for (int i=0; i<100000; i++) {
    //        fit.startPoint(0.1,0.1);
    //        fit.updatePoint(0.2,0.2);
    //        fit.updatePoint(0.21,0.22);
    //        fit.updatePoint(0.314,0.32);
    //        fit.updatePoint(0.41,0.424);
    //        fit.updatePoint(0.514,0.52);
    //        fit.updatePoint(0.55, 0.554);
    //        result= fit.finishPoint(0.554, 0.454);
    //        delete [] result;
    //    }
    //    fit.startPoint(0.1,0.1);
    //    fit.updatePoint(0.2,0.2);
    //    fit.updatePoint(0.21,0.22);
    //    fit.updatePoint(0.314,0.32);
    //    fit.updatePoint(0.41,0.424);
    //    fit.updatePoint(0.514,0.52);
    //    fit.updatePoint(0.55, 0.554);
    //    result= fit.finishPoint(0.554, 0.454);
    
    
    //    fit.startPoint(1, 1);
    //    fit.updatePoint(3, 3);
    //    fit.updatePoint(10, 10);
    //    fit.updatePoint(20, 20);
    //    fit.updatePoint(50, 50);
    //    fit.updatePoint(70, 70);
    //    fit.updatePoint(100, 101);
    //    float * result= fit.finishPoint(175, 175);
    //
    //    cout<<"result:"<<result[0]<<" "<<result[1]<<endl;
    
            //    m.inverse();
    //    vector<PointF> luo;
    //    vector<PointF> mergedPoints;
    //    PointF p1(100, 100);
    //    mergedPoints.push_back(p1);
    //    p1.x=200;
    //    luo=mergedPoints;
    //    luo[0].x=200;
    MatrixXf m(5, 5);
        cout<<m;
    
}

