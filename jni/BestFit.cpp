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
using Eigen::MatrixXd;
using namespace std;
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
    reset();
    inputPoint(x, y, false);
    ellipseFit.clear();
    ellipseFit.inputPoint(x, y);
}
void BestFit::updatePoint(float x, float y) {
    inputPoint(x, y, false);
    ellipseFit.inputPoint(x, y);
}
float * BestFit::finishPoint(float x, float y) {
    inputPoint(x, y, true);
    ellipseFit.inputPoint(x, y);
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
    if (fabs((atan(one->a) - atan(two->a))) * 180 / M_PI < 15) {
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
    //    double[][] Afloat = { { a, -1 }, { other.a, -1 } };
    //    double[][] Bfloat = { { -b }, { -other.b } };
    //    Matrix AMatrix = new Matrix(Afloat);
    //    Matrix BMatrix = new Matrix(Bfloat);
    //    Matrix XMatrix = AMatrix.solve(BMatrix);
    Eigen::MatrixXd AMatrix(2,2);
    AMatrix(0,0)=oneLine->a;
    AMatrix(0,1)=-1;
    AMatrix(1,0)=twoLine->a;
    AMatrix(1,1)=-1;
    Eigen::MatrixXd BMatrix(2,1);
    BMatrix(0,0)=-oneLine->b;
    BMatrix(1,0)=-twoLine->b;
    Eigen::MatrixXd XMatrix=AMatrix.inverse()*BMatrix;
    PointF result;
    result.x=XMatrix(0,0);
    result.y=XMatrix(1,0);
    return result;
}
float * BestFit::getResult() {
    float * result;
    if (lineFitList.size() == 1) {
        LineFit lineFit=lineFitList[0];
        result=new float[5];
        result[0]=1;
        result[1]=lineFit.startPoint.x;
        result[2]=lineFit.startPoint.y;
        result[3]=lineFit.endPoint.x;
        result[4]=lineFit.endPoint.y;
    }else{
        ellipseFit.compute();
        if(ellipseFit.errorValue< 10E7)
        {
            result=new float[6];
            result[0]=2;
            result[1]=ellipseFit.xc;
            result[2]=ellipseFit.yc;
            result[3]=ellipseFit.a;
            result[4]=ellipseFit.b;
            result[5]=ellipseFit.angle;
            cout<<"ellipse";
        }
        else{
            if(lineFitList.size() == 3)
            {
                result=getTriangle();
                cout<<"triangle";
            }
            else if(lineFitList.size() == 4)
            {
                result=getRectangle();
                cout<<"rectangle";
            }
            else{
                result=new float[0];
                result[0]=0;
                cout<<"not fit";
            }
        }
        
    }
    return result;
}
void BestFit::reset(){
    lineFitList.clear();
    lineList.clear();
}
int main(int argc, char *argv[]) {
    
//    BestFit fit;
    //    fit.startPoint(50, 100);
    //    fit.updatePoint(25, 75);
    //    fit.updatePoint(0, 50);
    //    fit.updatePoint(25, 25);
    //    fit.updatePoint(50, 0);
    //    fit.updatePoint(75, 25);
    //    fit.updatePoint(100, 50);
    //    float * result= fit.finishPoint(75, 75);
    
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
    
    //    MatrixXd m(2,2);
    //    m(0,0) = 2;
    //    m(0,1) = 1;
    //    m(1,0) = 1;
    //    m(1,1) = 2;
    //    m=m.inverse();
    //    Eigen::MatrixXd m2(2,1);
    //    m2(0,0)=3;
    //    m2(1,0)=3;
    //    m2=m*m2;
    //    std::cout << m2<< endl;
    //    m.inverse();
    vector<PointF> luo;
    vector<PointF> mergedPoints;
    PointF p1(100, 100);
    mergedPoints.push_back(p1);
    p1.x=200;
    luo=mergedPoints;
    luo[0].x=200;
//    delete p1;
    cout << mergedPoints[0].x;
}

