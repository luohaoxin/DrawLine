package analyze;
import java.util.ArrayList;

import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.PointF;
import fit.Drawable;
import fit.LineFit;
public class TriangleDrawable implements Drawable{
	ArrayList<LineFit> lineFits=new ArrayList<>();
	PointF onePoint;
	PointF twoPoint;
	PointF threePoint;
	public TriangleDrawable(ArrayList<LineFit> lineFits) {
		this.lineFits=lineFits;
		onePoint=lineFits.get(0).intersectPosition(lineFits.get(1));
		twoPoint=lineFits.get(1).intersectPosition(lineFits.get(2));
		threePoint=lineFits.get(2).intersectPosition(lineFits.get(0));
	}
	@Override
	public void draw(Canvas canvas, Paint paint) {
		canvas.drawLine(onePoint.x, onePoint.y, twoPoint.x, twoPoint.y, paint);
		canvas.drawLine(twoPoint.x, twoPoint.y, threePoint.x, threePoint.y, paint);
		canvas.drawLine(threePoint.x, threePoint.y, onePoint.x, onePoint.y, paint);
		
	}

}
