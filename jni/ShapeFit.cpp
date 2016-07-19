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
#include <jni.h>
#include <android/log.h>
#define TAG "EllipseFit"
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, TAG, __VA_ARGS__)
using namespace std;
//line
#define acceptDeltaAngleSumValue 25
#define acceptDeltaAnlgeAbsSumValue 15
#define leastLineLongLengthSquare 0.0005f
//Ellipse
#define maxEllipseError 0.00007
#define maxEllipseErrorForBig 0.0001
#define EllipseBigBorder 0.2
#define EllipseLengthWidthRate 1.3
PointF::PointF(float x, float y) {
	this->x = x;
	this->y = y;
}

PointF::PointF() {
	this->x = 0;
	this->y = 0;
}
PointF::~PointF() {
	cout << "PointF::~PointF()" << endl;
}
Line::Line() {
	lastAngle = 0;
	deltaAngleSum = 0;
	deltaAngleAbsSum = 0;
	inputList.clear();
}
Line::Line(const Line & c) {
	this->lastAngle = c.lastAngle;
	this->deltaAngleSum = c.deltaAngleSum;
	this->deltaAngleAbsSum = c.deltaAngleAbsSum;
	this->lastPoint = c.lastPoint;
	this->inputList = c.inputList;
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
	lastAngle = 0;
	deltaAngleSum = 0;
	deltaAngleAbsSum = 0;
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
	cout << "ShapeFit::~ShapeFit()" << endl;
}
LineFit::LineFit(vector<PointF> input) {
	this->inputList = input;
}
LineFit::LineFit(const LineFit & c) {
	this->errorValue = c.errorValue;
	this->inputList = c.inputList;
	this->a = c.a;
	this->b = c.b;
	this->startPoint.x = c.startPoint.x;
	this->startPoint.y = c.startPoint.y;
	this->endPoint.x = c.endPoint.x;
	this->endPoint.y = c.endPoint.y;
}

void LineFit::compute() {
	cout << "linefit compute:" << endl;
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
	errorValue = e / inputList.size();
	cout << "a:" << a << "b:" << b << "  e:" << (e / inputList.size()) << " n:"
			<< inputList.size() << endl;
}
void LineFit::setTwoPoint() {
	PointF first = inputList[0];
	PointF last = inputList[inputList.size() - 1];
	float deltaY;
	float deltaX;
	deltaY = fabs(first.x * a + b - first.y);
	deltaX = fabs(first.x - (first.y - b) / a);
	if (deltaY < deltaX) {
		startPoint.x = first.x;
		startPoint.y = first.x * a + b;
	} else {
		startPoint.x = (first.y - b) / a;
		startPoint.y = first.y;
	}

	deltaY = fabs(last.x * a + b - last.y);
	deltaX = fabs(last.x - (last.y - b) / a);
	if (deltaY < deltaX) {
		endPoint.x = last.x;
		endPoint.y = last.x * a + b;
	} else {
		endPoint.x = (last.y - b) / a;
		endPoint.y = last.y;
	}
}
void LineFit::setTwoPointWhenVerticalY() {
	PointF first = inputList[0];
	PointF last = inputList[inputList.size() - 1];
	startPoint.x = -b / a;
	startPoint.y = first.y;

	endPoint.x = -b / a;
	endPoint.y = last.y;

}
void LineFit::correct() {
	correctTwoPoints(&startPoint, &endPoint);
//    float angle=atan(a)*180/M_PI;
//    angle=getCorrectAngle(angle);
//    if (angle==90||angle==-90) {
//        isVerticalY=true;
//        startPoint.x=(startPoint.x+endPoint.x)/2;
//        endPoint.x=startPoint.x;
//    }else if(angle==0||angle==180){
//        startPoint.y=(startPoint.y+endPoint.y)/2;
//        endPoint.y=startPoint.y;
//        isVerticalY=false;
//        
//    }
}
void LineFit::clear() {
	inputList.clear();
}
LineFit::~LineFit() {
	cout << "LineFit::~LineFit()" << endl;
}
EllipseFit::EllipseFit() {

}
void EllipseFit::setInputPoint(vector<PointF> input) {
	this->inputList = input;
}
void EllipseFit::updateMinXY(float x, float y, bool isFirst) {
	if (isFirst) {
		minX = x;
		minY = y;
		maxX = x;
		maxY = y;
	} else {
		minX = minX < x ? minX : x;
		minY = minY < y ? minY : y;
		maxX = maxX > x ? maxX : x;
		maxY = maxY > y ? maxY : y;
	}
}
void EllipseFit::compute() {
	//x^2+Bxy+Cy^2+Dx+Ey+F=0
	MatrixXf AMatrix(5, 5);
	MatrixXf BMatrix(5, 1);
	AMatrix(0, 0) = 0;
	AMatrix(0, 1) = 0;
	AMatrix(0, 2) = 0;
	AMatrix(0, 3) = 0;
	AMatrix(0, 4) = 0;
	AMatrix(1, 0) = 0;
	AMatrix(1, 1) = 0;
	AMatrix(1, 2) = 0;
	AMatrix(1, 3) = 0;
	AMatrix(1, 4) = 0;
	AMatrix(2, 0) = 0;
	AMatrix(2, 1) = 0;
	AMatrix(2, 2) = 0;
	AMatrix(2, 3) = 0;
	AMatrix(2, 4) = 0;
	AMatrix(3, 0) = 0;
	AMatrix(3, 1) = 0;
	AMatrix(3, 2) = 0;
	AMatrix(3, 3) = 0;
	AMatrix(3, 4) = 0;
	AMatrix(4, 0) = 0;
	AMatrix(4, 1) = 0;
	AMatrix(4, 2) = 0;
	AMatrix(4, 3) = 0;
	AMatrix(4, 4) = 0;
	BMatrix(0, 0) = 0;
	BMatrix(1, 0) = 0;
	BMatrix(2, 0) = 0;
	BMatrix(3, 0) = 0;
	BMatrix(4, 0) = 0;
	float x, xx, xxx, xxxx;
	float y, yy, yyy, yyyy;
	for (int i = 0; i < inputList.size(); ++i) {

		x = inputList[i].x - minX;
		xx = x * x;
		xxx = xx * x;
		xxxx = xxx * x;
		y = inputList[i].y - minY;
		yy = y * y;
		yyy = yy * y;
		yyyy = yyy * y;
		AMatrix(0, 0) += xx * yy;
		AMatrix(0, 1) += x * yyy;
		AMatrix(0, 2) += xx * y;
		AMatrix(0, 3) += x * yy;
		AMatrix(0, 4) += x * y;
		AMatrix(1, 0) += x * yyy;
		AMatrix(1, 1) += yyyy;
		AMatrix(1, 2) += x * yy;
		AMatrix(1, 3) += yyy;
		AMatrix(1, 4) += yy;
		LOGV("AMatrix(2, 0) before:%f", AMatrix(2, 0));
		LOGV("xx:%f y:%f", xx, y);
		AMatrix(2, 0) += xx * y;
		LOGV("AMatrix(2, 0) after:%f", AMatrix(2, 0));
		AMatrix(2, 1) += x * yy;
		AMatrix(2, 2) += xx;
		AMatrix(2, 3) += x * y;
		AMatrix(2, 4) += x;
		AMatrix(3, 0) += x * yy;
		AMatrix(3, 1) += yyy;
		AMatrix(3, 2) += x * y;
		AMatrix(3, 3) += y * y;
		AMatrix(3, 4) += y;
		AMatrix(4, 0) += x * y;
		AMatrix(4, 1) += y * y;
		AMatrix(4, 2) += x;
		AMatrix(4, 3) += y;
		AMatrix(4, 4) += 1;
		BMatrix(0, 0) += -xxx * y;
		BMatrix(1, 0) += -xx * yy;
		BMatrix(2, 0) += -xxx;
		BMatrix(3, 0) += -xx * y;
		BMatrix(4, 0) += -xx;

	}
	LOGV("AMatrix:");
	LOGV(
			"%f %f %f %f %f", AMatrix(0, 0), AMatrix(0, 1), AMatrix(0, 2), AMatrix(0, 3), AMatrix(0, 4));
	LOGV(
			"%f %f %f %f %f", AMatrix(1, 0), AMatrix(1, 1), AMatrix(1, 2), AMatrix(1, 3), AMatrix(1, 4));
	LOGV(
			"%f %f %f %f %f", AMatrix(2, 0), AMatrix(2, 1), AMatrix(2, 2), AMatrix(2, 3), AMatrix(2, 4));
	LOGV(
			"%f %f %f %f %f", AMatrix(3, 0), AMatrix(3, 1), AMatrix(3, 2), AMatrix(3, 3), AMatrix(3, 4));
	LOGV(
			"%f %f %f %f %f", AMatrix(4, 0), AMatrix(4, 1), AMatrix(4, 2), AMatrix(4, 3), AMatrix(4, 4));
	LOGV("BMatrix:");
	LOGV(
			"%f %f %f %f %f", BMatrix(0, 0), BMatrix(1, 0), BMatrix(2, 0), BMatrix(3, 0), BMatrix(4, 0));
	Eigen::MatrixXf XMatrix = AMatrix.inverse() * BMatrix;
	cout << "compute " << "X:" << XMatrix;
	float B = XMatrix(0, 0);
	float C = XMatrix(1, 0);
	float D = XMatrix(2, 0);
	float E = XMatrix(3, 0);
	float F = XMatrix(4, 0);
	LOGV("B:%f C:%f D:%f E:%f F:%f", B, C, D, E, F);
	MatrixXf m(2, 2);
	m(0, 0) = 1;
	m(0, 1) = 2;
	m(1, 0) = 3;
	m(1, 1) = 1;

	Eigen::MatrixXf m2(2, 1);
	m2(0, 0) = 5;
	m2(1, 0) = 5;

	Eigen::MatrixXf m3 = m.inverse() * m2;
	LOGV("m1:%f m2:%f", m3(0,0), m3(1,0));

	xc = (B * E - 2 * C * D) / (4 * C - B * B) + minX;
	yc = (B * D - 2 * E) / (4 * C - B * B) + minY;

	float fenzi = 2 * (B * D * E - C * D * D - E * E + 4 * F * C - B * B * F);
	float fenmu = (B * B - 4 * C) * (C - sqrt(B * B + (1 - C) * (1 - C)) + 1);
	float femmu2 = (B * B - 4 * C) * (C + sqrt(B * B + (1 - C) * (1 - C)) + 1);
	a = sqrt(fabs(fenzi / fenmu));
	b = sqrt(fabs(fenzi / femmu2));

	// angle=Math.atan(B/(1-C))/2/Math.PI*180;
	angle = atan(sqrt((a * a - b * b * C) / (a * a * C - b * b)) + 0.0001)
			* 180/ M_PI;
	if (B > 0) {
		angle = 180 - angle;
	}
	cout << "compute:" << "xc:" << xc << "yc:" << yc << "a:" << a << "b:" << b
			<< "angle:" << angle;

	float eX = 0; //this error value is based on x coordinate
	float eY = 0; //this error value is based on y coordinate
	float tempX, tempY;
	for (int i = 0; i < inputList.size(); ++i) {
		x = inputList[i].x - minX;
		y = inputList[i].y - minY;
		tempX = x * x + B * x * y + C * y * y + D * x + E * y + F;
		tempY = tempX / C;
		eX += (tempX * tempX);
		eY += (tempY * tempY);
	}
	errorValue = (eX < eY ? eX : eY) / inputList.size();
	cout << "compute:" << "e/n:" << errorValue << "n:" << inputList.size();
	LOGV("e/n:%f n:%d", errorValue, inputList.size());
	// 判断是否是椭圆，有可能是双曲线
	if (B * B - 4 * C < 0 && (D * D / 4 + E * E / 4 / C - F > 0)) {
		LOGV("BCDEF check is a Ellipse");
		cout << "compute:" << "is a Ellipse";
	} else {
		LOGV("BCDEF check not a Ellipse");
		cout << "compute:" << "is not a Ellipse";
		errorValue = 10E20;
	}

}
void EllipseFit::correct() {
	if (a / b < EllipseLengthWidthRate) {
		a = (a + b) / 2;
		b = a;
	}
	angle = getCorrectAngle(angle);
}
bool EllipseFit::checkEllipse() {
	if (errorValue
			< (a * b > EllipseBigBorder ?
					maxEllipseErrorForBig : maxEllipseError)) {
		return true;

	} else {
		return false;
	}
}
void EllipseFit::clear() {
	errorValue = 0;
	minX = 0;
	minY = 0;
	maxX = 0;
	maxY = 0;
	inputList.clear();
}
EllipseFit::~EllipseFit() {
	cout << "EllipseFit::~EllipseFit()" << endl;
}
float getCorrectAngle(float angle) {
	if (abs(angle + 90) < acceptDeltaAnlgeAbsSumValue) {
		return -90;
	}
	if (abs(angle - 90) < acceptDeltaAnlgeAbsSumValue) {
		return 90;
	} else if (abs(angle - 0) < acceptDeltaAnlgeAbsSumValue) {
		return 0;

	} else if (abs(angle - 180) < acceptDeltaAnlgeAbsSumValue) {
		return 180;
	}
	return angle;
}
bool correctTwoPoints(PointF * pointOne, PointF * pointTwo) {
	bool result = false;
	float angle = atan(
			(pointTwo->y - pointOne->y) / (pointTwo->x - pointOne->x))
			* 180/M_PI;
	angle = getCorrectAngle(angle);
	if (angle == 90 || angle == -90) {
		result = true;
		pointOne->x = (pointOne->x + pointTwo->x) / 2;
		pointTwo->x = pointOne->x;
	} else if (angle == 0 || angle == 180) {
		result = true;
		pointOne->y = (pointOne->y + pointTwo->y) / 2;
		pointTwo->y = pointOne->y;
	}
	return result;
}
