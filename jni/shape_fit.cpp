/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include <string.h>
#include <jni.h>
#include"BestFit.h"
#include <android/log.h>
#define TAG "ShapeFit"
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, TAG, __VA_ARGS__)
#ifdef __cplusplus

extern "C" {

#endif
using namespace std;

static BestFit bestfit;

/*
 * Class:     analyze_BestShapeFit
 * Method:    startPoint
 * Signature: (FF)V
 */
void Java_analyze_BestShapeFit_startPoint(JNIEnv *env, jobject thiz, jfloat x,
		jfloat y) {
//	LOGV("best fit begin");
	LOGV("startPoint(%f,%f);", x, y);
	bestfit.startPoint(x, y);
//	LOGV("startPoint");
}

/*
 * Class:     analyze_BestShapeFit
 * Method:    updatePoint
 * Signature: (FF)V
 */
void Java_analyze_BestShapeFit_updatePoint(JNIEnv *env, jobject thiz, jfloat x,
		jfloat y) {
	LOGV("updatePoint(%f,%f);", x, y);
	bestfit.updatePoint(x, y);
}

/*
 * Class:     analyze_BestShapeFit
 * Method:    finishPoint
 * Signature: (FF)Ljava/lang/String;
 */
jfloatArray Java_analyze_BestShapeFit_finishPoint(JNIEnv *env, jobject thiz,
		jfloat x, jfloat y) {
	LOGV("finishPoint(%f,%f);", x, y);
	float * data = bestfit.finishPoint(x, y);

	jfloatArray result;
	int type = (int) (data[0]);
	LOGV("shape fit finish:%d", type);
	int size = 1;
	switch (type) {
	case 1:
		result = env->NewFloatArray(5);
		size = 5;
		break;
	case 2:
		result = env->NewFloatArray(6);
		size = 6;
		break;
	case 3:
		result = env->NewFloatArray(7);
		size = 7;
		break;
	case 4:
		result = env->NewFloatArray(9);
		size = 9;
		break;
	case 0:
		result = env->NewFloatArray(1);
		size = 1;
		break;
	default:
		result = env->NewFloatArray(1);
		size = 1;
		break;
	}
	env->SetFloatArrayRegion(result, 0, size, data);
	delete[] data;
	return result;
}
#ifdef __cplusplus

}

#endif
