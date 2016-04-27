package fit;

import java.util.ArrayList;

import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.PointF;

public abstract class ShapeFit implements Drawable{
	public double errorValue=0;
	public ArrayList<PointF> inputList = new ArrayList<PointF>();
	public void inputPoint(float x,float y)
	{
		inputList.add(new PointF(x,y));
	}
	
	public abstract void compute();
	public abstract void clear();
	public abstract void draw(Canvas canvas,Paint paint);
	
}
