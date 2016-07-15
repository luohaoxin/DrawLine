/*
 * ShapeFit.h
 *
 *  Created on: 2016年4月28日
 *      Author: luohaoxin
 */

#ifndef SHAPEFIT_H_
#define SHAPEFIT_H_
#include<vector>
#include "Eigen/Dense"
using Eigen::MatrixXf;
using namespace std;
class PointF {
public:
	float x;
	float y;
	PointF(float x, float y);
	PointF();
    ~PointF();
};
class Line {
public:
	float lastAngle;
	float deltaAngleSum;
	float deltaAngleAbsSum;
	PointF lastPoint;
	vector<PointF> inputList;
	Line();
    Line(const Line & c);
	bool inputPoint(float x, float y);
	bool checkLine();
	void reset();
};
class ShapeFit {
public:
	float errorValue;
	vector<PointF> inputList;
	ShapeFit();
	void inputPoint(float x, float y);
    void compute(){};
    void correct(){};
    void clear(){};
	~ShapeFit();
};
class LineFit: public ShapeFit {
public:
	float a, b;
    bool isVerticalY=false;
    PointF startPoint;
    PointF endPoint;
	void compute();
    void setTwoPoint();
    void setTwoPointWhenVerticalY();
    void correct();
	void clear();
	LineFit();
	LineFit(const LineFit & c);
	LineFit(vector<PointF> input);
	~LineFit();
};
class EllipseFit: public ShapeFit {
public:
    MatrixXf AMatrix;
    MatrixXf BMatrix;
    float xc, yc, a, b, angle;
    float minX,minY,maxX,maxY;
    EllipseFit();
    void updateMinXY(float x,float y,bool isFirst);
    void setInputPoint(vector<PointF> input);
	void compute();
    void correct();
    bool checkEllipse();
	void clear();
	~EllipseFit();
};
float getCorrectAngle(float angle);
bool correctTwoPoints(PointF * pointOne,PointF * pointTwo);
#endif /* SHAPEFIT_H_ */
