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
}
