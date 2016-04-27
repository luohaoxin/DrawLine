package com.example.drawline;

import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.Rect;
import android.graphics.RectF;
import android.util.Log;

public class PathDrawTask implements DrawTask {
	Path mPath;
	float startX;
	float startY;
	float stopX;
	float stopY;
	public Rect rect;

	public PathDrawTask(float startX, float startY, float stopX, float stopY, Path path) {
		// mPath = new Path();
		mPath = path;
		this.startX = startX;
		this.startY = startY;
		this.stopX = stopX;
		this.stopY = stopY;
		
	}

	@Override
	public void draw(Canvas canvas, Paint paint, Canvas canvas2) {
		// canvas2.clipPath(path)
		// canvas.drawPath(mPath, paint);
		// for (int i = 0; i < 15; i++) {
		// canvas.drawPoint(startX+2, stopY+2, paint);
		// }
		// canvas.drawPoint(startX, stopY, paint);
//		 canvas2.drawPath(mPath, paint);
//		canvas.drawLine(startX, startY, stopX, stopY, paint);
		canvas2.drawLine(startX, startY, stopX, stopY, paint);
		canvas.drawBitmap(MyView.cacheBitmap, 0, 0, null);
//		 canvas.drawBitmap(MyView.cacheBitmap, rect, rect, paint);

	}
}
