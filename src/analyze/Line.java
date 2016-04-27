package analyze;

import java.util.ArrayList;

import android.graphics.PointF;
import android.util.Log;

public class Line {
	String TAG = "Line";
	float acceptDeltaAngleSumValue = 25;
	float acceptErrorValue = 15;
	float leastLineLongLengthSquare=100*100;//px
	PointF lastPoint = null;
	double lastAngle;
	double deltaAngleSum;
	double deltaAngleAbsSum;

	ArrayList<PointF> inputList = new ArrayList<PointF>();

	/*
	 * true 表示可以继续接受input point来形成更长的直线
	 */
	public boolean inputPoint(float x, float y) {
		Log.i(TAG, "x:"+x+"y:"+y);
		PointF inputPoint = new PointF(x, y);
		if (inputList.size() == 0) {
			inputList.add(inputPoint);
			lastPoint = inputPoint;
			return true;
		} else if (inputList.size() == 1) {
			float dx = inputPoint.x - lastPoint.x;
			float dy = inputPoint.y - lastPoint.y;
			if (dx == 0 && dy == 0) {
				;
			} else {
				double angle = Math.asin(dy / Math.sqrt((dx * dx + dy * dy))) * 180 / Math.PI;
				lastAngle = angle;
				inputList.add(inputPoint);
				lastPoint = inputPoint;
			}
			return true;
		} else {
			float dx = inputPoint.x - lastPoint.x;
			float dy = inputPoint.y - lastPoint.y;
			if (dx == 0 && dy == 0) {
				return true;
			} else {
				double angle = Math.asin(dy / Math.sqrt((dx * dx + dy * dy))) * 180 / Math.PI;
				double deltaAngle = angle - lastAngle;
				double deltaAnlgeAbs = Math.abs(deltaAngle);
				Log.i(TAG, "dx:"+dx+"dy:"+dy+"deltaAngle"+deltaAngle);
				if (deltaAnlgeAbs < acceptErrorValue) {
					Log.i(TAG, "acceptErrorValue accept");
					deltaAngleSum += deltaAngle;
					deltaAngleAbsSum += deltaAnlgeAbs;
					if (Math.abs(deltaAngleSum) < acceptDeltaAngleSumValue) {
						Log.i(TAG, "acceptDeltaAngleSumValue accept");
						lastAngle = angle;
						inputList.add(inputPoint);
						lastPoint = inputPoint;
						return true;
					} else {
						Log.i(TAG, "acceptDeltaAngleSumValue reject");
						deltaAngleSum -= deltaAngle;
						deltaAngleAbsSum -= deltaAnlgeAbs;
						return false;
					}
				} else {
					Log.i(TAG, "acceptErrorValue reject");
					return false;
				}

			}
			
		}

	}

	public boolean checkLine() {
		
		if(inputList.size()>2)
		{
			PointF first=inputList.get(0);
			PointF last=inputList.get(inputList.size()-1);
			float deltaX=first.x-last.x;
			float deltaY=first.y-last.y;
			if(deltaX*deltaX+deltaY*deltaY>leastLineLongLengthSquare)
			{
				Log.e(TAG, "a line success");
				return true;
			}
			else {
				Log.i(TAG, "sum distance too short"+(deltaX*deltaX+deltaY*deltaY));
				clear();
				return false;
			}
			
		}
		else {
			Log.i(TAG, "points too few");
			clear();
			return false;
		}
	}
	private void clear()
	{
		lastPoint = null;
		lastAngle=0;
		deltaAngleSum=0;
		deltaAngleAbsSum=0;

		inputList = new ArrayList<PointF>();

	}
}
