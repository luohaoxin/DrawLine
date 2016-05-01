/*
 * luo.h
 *
 *  Created on: 2016年4月26日
 *      Author: luohaoxin
 */

#ifndef BESTFIT_H_
#define BESTFIT_H_

class BestFit {
public:
	BestFit();
	float a;
	void startPoint(float x,float y);
	void updatePoint(float x,float y);
	float finishPoint(float x,float y);
};

#endif /* BESTFIT_H_ */
