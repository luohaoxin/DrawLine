package fit;

import java.util.ArrayList;

import Jama.Matrix;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.PointF;
import android.graphics.RectF;
import android.util.Log;

public class EllipseFit extends ShapeFit {
	Matrix AMatrix, BMatrix;
	double[][] Afloat = new double[5][5];
	double[][] Bfloat = new double[5][1];

	double xc, yc, a, b, angle;

	@Override
	public void inputPoint(float x, float y) {
		inputList.add(new PointF(x, y));
	}

	@Override
	public void compute() {
		float x, xx, xxx, xxxx;
		float y, yy, yyy, yyyy;
		for (int i = 0; i < inputList.size(); ++i) {
			x = inputList.get(i).x;
			xx = x * x;
			xxx = xx * x;
			xxxx = xxx * x;
			y = inputList.get(i).y;
			yy = y * y;
			yyy = yy * y;
			yyyy = yyy * y;
			Afloat[0][0] += xx * yy;
			Afloat[0][1] += x * yyy;
			Afloat[0][2] += xx * y;
			Afloat[0][3] += x * yy;
			Afloat[0][4] += x * y;
			Afloat[1][0] += x * yyy;
			Afloat[1][1] += yyyy;
			Afloat[1][2] += x * yy;
			Afloat[1][3] += yyy;
			Afloat[1][4] += yy;
			Afloat[2][0] += xx * y;
			Afloat[2][1] += x * yy;
			Afloat[2][2] += xx;
			Afloat[2][3] += x * y;
			Afloat[2][4] += x;
			Afloat[3][0] += x * yy;
			Afloat[3][1] += yyy;
			Afloat[3][2] += x * y;
			Afloat[3][3] += y * y;
			Afloat[3][4] += y;
			Afloat[4][0] += x * y;
			Afloat[4][1] += y * y;
			Afloat[4][2] += x;
			Afloat[4][3] += y;
			Afloat[4][4] += 1;
			Bfloat[0][0] += -xxx * y;
			Bfloat[1][0] += -xx * yy;
			Bfloat[2][0] += -xxx;
			Bfloat[3][0] += -xx * y;
			Bfloat[4][0] += -xx;

		}
		AMatrix = new Matrix(Afloat);
		BMatrix = new Matrix(Bfloat);
		Matrix XMatrix = AMatrix.solve(BMatrix);
		Log.i("compute", "X:" + XMatrix.toString());
		double[][] result = XMatrix.getArray();
		double B = result[0][0];
		double C = result[1][0];
		double D = result[2][0];
		double E = result[3][0];
		double F = result[4][0];
		xc = (B * E - 2 * C * D) / (4 * C - B * B);
		yc = (B * D - 2 * E) / (4 * C - B * B);

		double fenzi = 2 * (B * D * E - C * D * D - E * E + 4 * F * C - B * B * F);
		double fenmu = (B * B - 4 * C) * (C - Math.sqrt(B * B + (1 - C) * (1 - C)) + 1);
		double femmu2 = (B * B - 4 * C) * (C + Math.sqrt(B * B + (1 - C) * (1 - C)) + 1);
		a = Math.sqrt(Math.abs(fenzi / fenmu));
		b = Math.sqrt(Math.abs(fenzi / femmu2));

		// angle=Math.atan(B/(1-C))/2/Math.PI*180;
		angle = Math.atan(Math.sqrt((a * a - b * b * C) / (a * a * C - b * b)) + 0.0001) * 180 / Math.PI;
		if (B > 0) {
			angle = 180 - angle;
		}
		Log.i("compute", "xc:" + xc + "yc:" + yc + "a:" + a + "b:" + b + "angle:" + angle);

		double e = 0;
		double temp;
		for (int i = 0; i < inputList.size(); ++i) {
			x = inputList.get(i).x;
			y = inputList.get(i).y;
			temp = x * x + B * x * y + C * y * y + D * x + E * y + F;
			e += (temp * temp);
		}
		errorValue = e / inputList.size();
		Log.e("compute", "e/n:" + (e / inputList.size()) + "n:" + inputList.size());

		// 判断是否是椭圆，有可能是双曲线
		if (B * B - 4 * C < 0 && (D * D / 4 + E * E / 4 / C - F > 0)) {
			Log.e("compute", "is a Ellipse");
		} else {
			Log.e("compute", "not a Ellipse");
			errorValue = 10E10;
		}

	}

	@Override
	public void clear() {
		inputList = new ArrayList<PointF>();
	}

	@Override
	public void draw(Canvas canvas, Paint paint) {

		canvas.save();
		canvas.rotate((float) angle, (float) xc, (float) yc);
		RectF oval = new RectF((float) (xc - a), (float) (yc - b), (float) (xc + a), (float) (yc + b));
		canvas.drawOval(oval, paint);
		canvas.restore();

		;
	}
}
