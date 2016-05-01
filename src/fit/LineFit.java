package fit;

import java.util.ArrayList;

import Jama.Matrix;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.PointF;
import android.util.Log;

public class LineFit extends ShapeFit {
	public float a, b;
	ArrayList<Float> result = new ArrayList<Float>();

	public LineFit() {

	}

	public LineFit(ArrayList<PointF> input) {
		inputList = input;
	}

	@Override
	public void inputPoint(float x, float y) {
		// TODO Auto-generated method stub
		super.inputPoint(x, y);
	}

	@Override
	public void compute() {
		float t1 = 0, t2 = 0, t3 = 0, t4 = 0;
		float e = 0;
		for (int i = 0; i < inputList.size(); ++i) {
			t1 += inputList.get(i).x * inputList.get(i).x;
			t2 += inputList.get(i).x;
			t3 += inputList.get(i).x * inputList.get(i).y;
			t4 += inputList.get(i).y;
		}
		a = (t3 * inputList.size() - t2 * t4) / (t1 * inputList.size() - t2 * t2); // 求得β1
		b = (t1 * t4 - t2 * t3) / (t1 * inputList.size() - t2 * t2); // 求得β2
		float temp;
		for (int i = 0; i < inputList.size(); ++i) {
			temp = inputList.get(i).y - b - a * inputList.get(i).x;
			e += (temp * temp);
		}
		errorValue = e / inputList.size();
		Log.i("compute", "a:" + a + "b:" + b + "  e:" + (e / inputList.size()) + "n:" + inputList.size());
		result = new ArrayList<Float>();
		PointF first = inputList.get(0);
		PointF last = inputList.get(inputList.size() - 1);
		float deltaY;
		float deltaX;
		deltaY = Math.abs(first.x * a + b - first.y);
		deltaX = Math.abs(first.x - (first.y - b) / a);
		if (deltaY < deltaX) {
			result.add(first.x);
			result.add(first.x * a + b);
		} else {
			result.add((first.y - b) / a);
			result.add(first.y);
		}

		deltaY = Math.abs(last.x * a + b - last.y);
		deltaX = Math.abs(last.x - (last.y - b) / a);
		if (deltaY < deltaX) {
			result.add(last.x);
			result.add(last.x * a + b);
		} else {
			result.add((last.y - b) / a);
			result.add(last.y);
		}
	}

	@Override
	public void clear() {
		inputList = new ArrayList<PointF>();
	}

	@Override
	public void draw(Canvas canvas, Paint paint) {

		canvas.drawLine(result.get(0), result.get(1), result.get(2), result.get(3), paint);
	}

	public PointF intersectPosition(LineFit other) {
		double[][] Afloat = { { a, -1 }, { other.a, -1 } };
		double[][] Bfloat = { { -b }, { -other.b } };
		Matrix AMatrix = new Matrix(Afloat);
		Matrix BMatrix = new Matrix(Bfloat);
		Matrix XMatrix = AMatrix.solve(BMatrix);
		return new PointF((float) (XMatrix.getArray()[0][0]), (float) (XMatrix.getArray()[1][0]));
	}
}
