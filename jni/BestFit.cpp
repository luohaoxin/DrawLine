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
#define maxLineFitListErrorValue 0.0005
#define maxShapeFitListErrorValue 0.0005
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
    LineFit lineFit(ellipseFit.inputList);
    lineFit.compute();
    lineFit.setTwoPoint();
    lineFit.correct();
    
    
    float * result = new float[5];
    result[0] = 1;
    result[1] = lineFit.startPoint.x;
    result[2] = lineFit.startPoint.y;
    result[3] = lineFit.endPoint.x;
    result[4] = lineFit.endPoint.y;
    return result;
    
}
float * BestFit::getTriangle()
{
    float * result=new float[7];
    PointF onePoint=getCrossPoint(&lineFitList[0], &lineFitList[1]);
    PointF twoPoint=getCrossPoint(&lineFitList[1], &lineFitList[2]);
    PointF threePoint=getCrossPoint(&lineFitList[2], &lineFitList[0]);
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
    float * result=new float[9];
    PointF onePoint=getCrossPoint(&lineFitList[0], &lineFitList[1]);
    PointF twoPoint=getCrossPoint(&lineFitList[1], &lineFitList[2]);
    PointF threePoint=getCrossPoint(&lineFitList[2], &lineFitList[3]);
    PointF fourPoint=getCrossPoint(&lineFitList[3], &lineFitList[0]);
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
//remeber to delete [] result ,after you use result
float * BestFit::getResult() {
    float * result;
    float lineerror=getLineFitListErrorValue();
    //    LOGV("getLineFitListErrorValue:%f", getLineFitListErrorValue());
    ellipseFit.compute();
    //    LOGV("ellipseFitErrorValue:%f", ellipseFit.errorValue);
    //    if(!isClosed)
    //    {
    //maybe a line
    if (lineFitList.size()
        == 1&&getLineFitListErrorValue()<maxLineFitListErrorValue) {
        result=getLine();
        cout<<"line";
        return result;
    }
    //    }
    //    else{
    if (lineFitList.size()
        == 4&&getLineFitListErrorValue()<maxShapeFitListErrorValue) {
        result = getRectangle();
        cout << "rectangle";
        return result;
    }
    if (lineFitList.size()
        == 3&&getLineFitListErrorValue()<maxShapeFitListErrorValue) {
        result = getTriangle();
        cout << "triangle";
        return result;
    }
    
    if (ellipseFit.checkEllipse()) {
        ellipseFit.correct();
        result = new float[6];
        result[0] = 2;
        result[1] = ellipseFit.xc;
        result[2] = ellipseFit.yc;
        result[3] = ellipseFit.a;
        result[4] = ellipseFit.b;
        result[5] = ellipseFit.angle;
        cout << "ellipse";
    } else {
        
        result = new float[1];
        result[0] = 0;
        cout << "not fit";
        
    }
    
    
    
    
    //    float * result;
    //    if(!isClosed)
    //    {
    //        //maybe a line
    //        if (lineFitList.size() == 1&&getLineFitListErrorValue()<maxLineFitListErrorValue) {
    //            LineFit lineFit(ellipseFit.inputList);
    //            lineFit.compute();
    //            result=new float[5];
    //            result[0]=1;
    //            result[1]=lineFit.startPoint.x;
    //            result[2]=lineFit.startPoint.y;
    //            result[3]=lineFit.endPoint.x;
    //            result[4]=lineFit.endPoint.y;
    //        }
    //        else{
    //            result=new float[1];
    //            result[0]=0;
    //            cout<<"not fit";
    //
    //        }
    //    }
    //    else{
    //        if(lineFitList.size() == 4&&getLineFitListErrorValue()<maxShapeFitListErrorValue)
    //        {
    //            result=getRectangle();
    //            cout<<"rectangle";
    //            return result;
    //        }
    //        if(lineFitList.size() == 3&&getLineFitListErrorValue()<maxShapeFitListErrorValue)
    //        {
    //            result=getTriangle();
    //            cout<<"triangle";
    //            return result;
    //        }
    //        ellipseFit.compute();
    //        if(ellipseFit.checkEllipse())
    //        {
    //            ellipseFit.correct();
    //            result=new float[6];
    //            result[0]=2;
    //            result[1]=ellipseFit.xc;
    //            result[2]=ellipseFit.yc;
    //            result[3]=ellipseFit.a;
    //            result[4]=ellipseFit.b;
    //            result[5]=ellipseFit.angle;
    //            cout<<"ellipse";
    //        }
    //        else{
    //
    //
    //                result=new float[1];
    //                result[0]=0;
    //                cout<<"not fit";
    //
    //        }
    //
    //
    //    }
    return result;
}
void BestFit::reset(){
    lineFitList.clear();
    lineList.clear();
    ellipseFit.clear();
    ellipseFit.minX=0;
    ellipseFit.minY=0;
}
int main(int argc, char *argv[]) {
    BestFit fit;
    float * result;
    fit.startPoint(0.813889,0.786111);
    fit.updatePoint(0.811111,0.782407);
    fit.updatePoint(0.800000,0.770370);
    fit.updatePoint(0.772222,0.747222);
    fit.updatePoint(0.722222,0.719444);
    fit.updatePoint(0.650926,0.693519);;
    fit.updatePoint(0.568519,0.673148);
    fit.updatePoint(0.486111,0.661111);
    fit.updatePoint(0.406481,0.656482);
    fit.updatePoint(0.333333,0.659259);
    fit.updatePoint(0.266667,0.670370);
    fit.updatePoint(0.212037,0.688889);
    fit.updatePoint(0.172222,0.710185);
    fit.updatePoint(0.140741,0.746296);
    fit.updatePoint(0.127778,0.785185);
    fit.updatePoint(0.133333,0.822222);
    fit.updatePoint(0.154630,0.864815);
    fit.updatePoint(0.191667,0.898148);
    fit.updatePoint(0.253704,0.938889);
    fit.updatePoint(0.326852,0.966667);
    fit.updatePoint(0.405556,0.985185);
    fit.updatePoint(0.483333,0.987037);
    fit.updatePoint(0.563889,0.987037);
    fit.updatePoint(0.646296,0.975000);
    fit.updatePoint(0.717593,0.954630);
    fit.updatePoint(0.762963,0.937037);
    fit.updatePoint(0.799074,0.915741);
    fit.updatePoint(0.819444,0.893519);
    fit.updatePoint(0.833333,0.873148);
    fit.updatePoint(0.839815,0.789815);
    fit.updatePoint(0.836111,0.782407);
    result=fit.finishPoint(0.836111,0.782407);
    
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
    
    //    MatrixXf m(2,2);
    //    m(0,0) = 2;
    //    m(0,1) = 1;
    //    m(1,0) = 1;
    //    m(1,1) = 2;
    //    m=m.inverse();
    //    Eigen::MatrixXf m2(2,1);
    //    m2(0,0)=3;
    //    m2(1,0)=3;
    //    m2=m*m2;
    //    std::cout << m2<< endl;
    //    m.inverse();
    //    vector<PointF> luo;
    //    vector<PointF> mergedPoints;
    //    PointF p1(100, 100);
    //    mergedPoints.push_back(p1);
    //    p1.x=200;
    //    luo=mergedPoints;
    //    luo[0].x=200;
    int * aa;
    aa=new int(2);
    cout << *aa;
}

