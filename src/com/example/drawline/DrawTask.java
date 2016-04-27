package com.example.drawline;

import android.graphics.Canvas;
import android.graphics.Paint;

public interface DrawTask {
	public void draw(Canvas canvas, Paint paint,Canvas cacheCanvas);
}
