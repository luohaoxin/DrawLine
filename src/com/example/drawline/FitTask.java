package com.example.drawline;

import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import fit.Drawable;
import fit.ShapeFit;

public class FitTask implements DrawTask {
	Drawable shapeFit;

	public FitTask(Drawable shapeFit) {
		this.shapeFit = shapeFit;
	}

	@Override
	public void draw(Canvas canvas, Paint paint, Canvas cacheCanvas) {
		Paint paint2 = new Paint();
		paint2.setColor(Color.WHITE);
		cacheCanvas.drawPaint(paint2);
		shapeFit.draw(cacheCanvas, paint);
		canvas.drawBitmap(MyView.cacheBitmap, 0, 0, null);
	}

}
