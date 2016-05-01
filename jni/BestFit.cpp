/*
 * luohaoxin.cpp
 *
 *  Created on: 2016年4月26日
 *      Author: luohaoxin
 */

#include"BestFit.h"
BestFit::BestFit() {
	a = 0;
}
void BestFit::startPoint(float x,float y)
{
	a=0;
}
void BestFit::updatePoint(float x,float y)
{
	a++;
}
float BestFit::finishPoint(float x,float y)
{
	a++;
	return a;
}
