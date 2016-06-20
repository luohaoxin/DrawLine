package com.example.drawline;

import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;

import analyze.BestShapeFit;
import analyze.PreAnalyzeFit;
import android.annotation.SuppressLint;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.PixelFormat;
import android.graphics.RectF;
import android.util.AttributeSet;
import android.util.Log;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.widget.Toast;
import fit.Drawable;
import fit.ShapeFit;

@SuppressLint("ClickableViewAccessibility")
public class MyView extends SurfaceView implements SurfaceHolder.Callback {

	float preX;
	float preY;
	private Path path;
	final int VIEW_WIDTH = 320;
	final int VIEW_HEIGHT = 480;
	// 定义一个内存中的图片,该图片将作为缓冲区
	public static Bitmap cacheBitmap = null;
	// 定义cacheBitmap上的canvas对象
	Canvas cacheCanvas = null;

	private SurfaceHolder holder;
	private MyThread myThread;
	BlockingQueue<DrawTask> taskQueue = new LinkedBlockingQueue<DrawTask>();
	Paint paint = new Paint(); // 创建画笔
	ShapeFit shapeFit;
	PreAnalyzeFit preAnalyzeFit;
	Context context;

	public MyView(Context context) {

		super(context);
		init(context);
	}

	public MyView(Context context, AttributeSet attrs) {
		super(context, attrs);
		init(context);
	}

	Bitmap bitmap;
	Toast toast;
	int width;

	private void init(Context context) {
		width = getResources().getDisplayMetrics().widthPixels;
		this.context = context;
		toast = Toast.makeText(context, "", Toast.LENGTH_SHORT);
		Log.i("luohaoxin", "surface()");
		bitmap = BitmapFactory.decodeResource(getResources(), R.drawable.ic_launcher);
		cacheBitmap = Bitmap.createBitmap(getResources().getDisplayMetrics().widthPixels,
				getResources().getDisplayMetrics().heightPixels, Config.ARGB_8888);
		cacheCanvas = new Canvas();
		cacheCanvas.setBitmap(cacheBitmap);
		path = new Path();
		paint.setColor(Color.BLUE);
		paint.setAntiAlias(true);
		paint.setStyle(Paint.Style.STROKE);
		paint.setStrokeWidth(3);
		holder = this.getHolder();
		holder.addCallback(this);
		setZOrderOnTop(true);// 设置画布 背景透明
		getHolder().setFormat(PixelFormat.TRANSLUCENT);

		myThread = new MyThread(holder);// 创建一个绘图线程

	}

	@Override
	public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
		// TODO Auto-generated method stub
		Log.i("luohaoxin", "surfaceChanged");

	}

	@Override
	public void surfaceCreated(SurfaceHolder holder) {
		Log.i("luohaoxin", "surfaceCreated");
		Canvas canvas = null;
		try {
			synchronized (holder) {
				canvas = holder.lockCanvas();// 锁定画布，一般在锁定后就可以通过其返回的画布对象Canvas，在其上面画图等操作了。
				canvas.drawColor(Color.TRANSPARENT);// 设置画布背景颜色

			}
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			if (canvas != null) {
				holder.unlockCanvasAndPost(canvas);// 结束锁定画图，并提交改变。
			}
		}
		myThread.isRun = true;
		if (isfirst)
			myThread.start();
		isfirst = false;
	}

	boolean isfirst = true;

	@Override
	public void surfaceDestroyed(SurfaceHolder holder) {
		Log.i("luohaoxin", "surfaceDestroyed");
		myThread.isRun = false;
	}

	@Override
	public boolean onTouchEvent(MotionEvent event) {
		// 获取拖动事件发生的位置
		float x = event.getX();
		float y = event.getY();
		Log.i("luohaoxin", "onTouchEvent:" + event.getActionMasked() + " " + event.getPointerCount());
		switch (event.getAction()) {
		case MotionEvent.ACTION_DOWN:
			BestShapeFit.startPoint(x / width, y / width);
			path.moveTo(x, y);
			// shapeFit = new EllipseFit();
			// shapeFit.inputPoint(x, y);
			// preAnalyzeFit = new PreAnalyzeFit();
			// preAnalyzeFit.inputPoint(x, y, false);
			preX = x;
			preY = y;
			break;
		case MotionEvent.ACTION_MOVE:
			BestShapeFit.updatePoint(x / width, y / width);
			path.quadTo((preX + x) / 2, (preY + y) / 2, x, y);
			// shapeFit.inputPoint(x, y);
			// preAnalyzeFit.inputPoint(x, y, false);
			taskQueue.add(new PathDrawTask(preX, preY, x, y, path));
			preX = x;
			preY = y;
			break;
		case MotionEvent.ACTION_UP:
			float[] line = BestShapeFit.finishPoint(x / width, y / width);
			String text = "show";
			for (int i = 0; i < line.length; i++) {
				text = text + " " + line[i] + " ";
			}
			toast.setText(text);
			toast.show();

			path.quadTo((preX + x) / 2, (preY + y) / 2, x, y);
			taskQueue.add(new PathDrawTask(preX, preY, x, y, path));
			path = new Path();

			// shapeFit.inputPoint(x, y);
			// shapeFit.compute();
			// preAnalyzeFit.inputPoint(x, y, true);
			// Drawable preDrawable = preAnalyzeFit.getResultDrawable();
			// if (preDrawable instanceof LineFit) {
			//
			// } else {
			// shapeFit.compute();
			// if (shapeFit.errorValue < 10E7) {
			// preDrawable = shapeFit;
			// }
			// }
			Drawable preDrawable = getResultDrawable(line);
			if (preDrawable != null) {
				taskQueue.add(new FitTask(preDrawable));
			}

			break;
		}
		// 返回true表明处理方法已经处理该事件
		return true;
	}

	public Drawable getResultDrawable(final float[] result) {
		Drawable resultDrawable = null;
		int type = (int) (result[0]);
		// Toast.makeText(getContext(), "" + result.length, 1000).show();
		switch (type) {
		case 1:
			resultDrawable = new Drawable() {
				@Override
				public void draw(Canvas canvas, Paint paint) {
					canvas.drawLine(result[1] * width, result[2] * width, result[3] * width, result[4] * width, paint);

				}
			};
			break;
		case 2:
			resultDrawable = new Drawable() {
				@Override
				public void draw(Canvas canvas, Paint paint) {
					float xc = result[1] * width;
					float yc = result[2] * width;
					float a = result[3] * width;
					float b = result[4] * width;
					float angle = result[5];
					canvas.save();
					canvas.rotate(angle, xc, yc);
					RectF oval = new RectF(xc - a, yc - b, xc + a, yc + b);
					canvas.drawOval(oval, paint);
					canvas.restore();
				}
			};
			break;
		case 3:
			resultDrawable = new Drawable() {
				@Override
				public void draw(Canvas canvas, Paint paint) {
					canvas.drawLine(result[1] * width, result[2] * width, result[3] * width, result[4] * width, paint);
					canvas.drawLine(result[3] * width, result[4] * width, result[5] * width, result[6] * width, paint);
					canvas.drawLine(result[5] * width, result[6] * width, result[1] * width, result[2] * width, paint);
				}
			};
			break;
		case 4:
			resultDrawable = new Drawable() {
				@Override
				public void draw(Canvas canvas, Paint paint) {
					canvas.drawLine(result[1] * width, result[2] * width, result[3] * width, result[4] * width, paint);
					canvas.drawLine(result[3] * width, result[4] * width, result[5] * width, result[6] * width, paint);
					canvas.drawLine(result[5] * width, result[6] * width, result[7] * width, result[8] * width, paint);
					canvas.drawLine(result[7] * width, result[8] * width, result[1] * width, result[2] * width, paint);
				}
			};
			break;
		case 0:

			break;
		default:

			break;
		}
		return resultDrawable;
	}

	@Override
	public void draw(Canvas canvas) {
		// TODO Auto-generated method stub
		super.draw(canvas);
	}

	// 线程内部类
	class MyThread extends Thread {
		private SurfaceHolder holder;
		public boolean isRun;

		public MyThread(SurfaceHolder holder) {
			this.holder = holder;
			isRun = true;
		}

		@Override
		public void run() {
			while (isRun) {
				Canvas canvas = null;
				try {
					DrawTask task = taskQueue.take();
					long begin = System.currentTimeMillis();
					canvas = holder.lockCanvas();// 锁定画布，一般在锁定后就可以通过其返回的画布对象Canvas，在其上面画图等操作了。
					task.draw(canvas, paint, cacheCanvas);
					Log.i("luohaoxin", "drawCost:" + (System.currentTimeMillis() - begin));
				} catch (Exception e) {
					// TODO: handle exception
					e.printStackTrace();
				} finally {
					if (canvas != null) {
						holder.unlockCanvasAndPost(canvas);// 结束锁定画图，并提交改变。
					}
				}
			}
		}
	}
}
