package analyze;

public class BestShapeFit {

	public static native void startPoint(float x, float y);

	public static native void updatePoint(float x, float y);

	public static native float[] finishPoint(float x, float y);

	/*
	 * this is used to load the 'hello-jni' library on application startup. The
	 * library has already been unpacked into
	 * /data/data/com.example.hellojni/lib/libhello-jni.so at installation time
	 * by the package manager.
	 */
	static {
		System.loadLibrary("shape_fit");
	}
	// Pseudocode for robustly computing the closest ellipse point and distance
	// to a query point. It
	// is required that e0 >= e1 > 0, y0 >= 0, and y1 >= 0.
	// e0,e1 = ellipse dimension 0 and 1, where 0 is greater and both are
	// positive.
	// y0,y1 = initial point on ellipse axis (center of ellipse is 0,0)
	// x0,x1 = intersection point

}
