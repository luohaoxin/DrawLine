/*
 * luo.h
 *
 *  Created on: 2016年4月26日
 *      Author: luohaoxin
 */

#ifndef BESTFIT_H_
#define BESTFIT_H_
#include "ShapeFit.h"
#include <vector>
#include <list>
class BestFit {
public:
	BestFit();
	vector<Line> lineList;
	Line currentLine;
	vector<LineFit> lineFitList;
    EllipseFit ellipseFit;
    float * getTriangle();
    float * getRectangle();
    PointF getCrossPoint(LineFit * oneLine,LineFit * twoLine);
	void startPoint(float x, float y);
	void updatePoint(float x, float y);
	float * finishPoint(float x, float y);
	void inputPoint(float x, float y, bool isFinished);
	float * getResult();
    void reset();
    ~BestFit();
private:
	bool mergeSimilarLines();
	bool mergeLinesIfNeed(int oneIndex, int twoIndex);
};

#endif /* BESTFIT_H_ */
