package analyze;

import java.util.ArrayList;

import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.PointF;
import fit.Drawable;
import fit.LineFit;

public class PreAnalyzeFit implements Drawable {
	ArrayList<PointF> inputList = new ArrayList<PointF>();
	ArrayList<Line> lineList = new ArrayList<Line>();
	Line currentLine = new Line();
	ArrayList<LineFit> lineFitList = new ArrayList<LineFit>();

	public void inputPoint(float x, float y, boolean isFinished) {
		boolean canInputAgain = currentLine.inputPoint(x, y);
		if (isFinished || !canInputAgain) {
			if (currentLine.checkLine()) {
				lineList.add(currentLine);
				currentLine = new Line();
			}
		}
		if (isFinished) {
			LineFit lineFit;
			for (int i = 0; i < lineList.size(); i++) {
				lineFit = new LineFit(lineList.get(i).inputList);
				lineFit.compute();
				lineFitList.add(lineFit);
			}
			while (mergeSimilarLines()) {
			}
		}

	}

	@Override
	public void draw(Canvas canvas, Paint paint) {

		for (int i = 0; i < lineFitList.size(); i++) {
			lineFitList.get(i).draw(canvas, paint);
		}

	}

	private boolean mergeSimilarLines() {
		boolean hasMerged = false;
		if (lineFitList.size() <= 1) {
			hasMerged = false;
		} else if (lineFitList.size() == 2) {
			hasMerged = mergeLinesIfNeed(lineFitList.get(0), lineFitList.get(1));
		} else {
			for (int i = 0; i < lineFitList.size(); i++) {
				if (mergeLinesIfNeed(lineFitList.get(i),
						i == lineFitList.size() - 1 ? lineFitList.get(0) : lineFitList.get(i + 1))) {
					hasMerged = true;
				}
			}
		}
		return hasMerged;
	}

	private boolean mergeLinesIfNeed(LineFit one, LineFit two) {
		if (Math.abs((Math.atan(one.a) - Math.atan(two.a))) * 180 / Math.PI < 15) {
			mergeLines(one, two);
			return true;
		} else {
			return false;
		}
	}

	private void mergeLines(LineFit one, LineFit two) {
		ArrayList<PointF> mergedPoints = new ArrayList<PointF>();
		mergedPoints.addAll(one.inputList);
		mergedPoints.addAll(two.inputList);
		LineFit mergedLineFit = new LineFit(mergedPoints);
		mergedLineFit.compute();
		lineFitList.set(lineFitList.indexOf(one), mergedLineFit);
		lineFitList.remove(two);
	}

	public Drawable getResultDrawable() {
		if (lineFitList.size() == 1) {
			return lineFitList.get(0);
		}
		if (lineFitList.size() == 3) {
			return new TriangleDrawable(lineFitList);
		}
		if (lineFitList.size() == 4) {
			return new RectangleDrawable(lineFitList);
		}
		return null;
	}
}
