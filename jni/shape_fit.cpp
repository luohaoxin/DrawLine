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
// for __android_log_print(ANDROID_LOG_INFO, "YourApp", "formatted message");
#include <android/log.h>
#define TAG "ShapeFit"
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, TAG, __VA_ARGS__)
#ifdef __cplusplus

extern "C" {

#endif
using namespace std;

luohaoxin l;
//luohaoxin * lpoint;
/*
 * Class:     analyze_BestShapeFit
 * Method:    startPoint
 * Signature: (FF)V
 */
void Java_analyze_BestShapeFit_startPoint(JNIEnv *env, jobject thiz, jfloat x,
		jfloat y) {
//	lpoint=new luohaoxin();
//	lpoint->a=0;
//	(*lpoint).a++;
	l.a=0;
	l.a++;
	LOGV("startPoint");
}

/*
 * Class:     analyze_BestShapeFit
 * Method:    updatePoint
 * Signature: (FF)V
 */
void Java_analyze_BestShapeFit_updatePoint(JNIEnv *env, jobject thiz, jfloat x,
		jfloat y) {
//	lpoint->a++;
//	(*lpoint).a++;
	l.a++;
	LOGV("updatePoint");
}

/*
 * Class:     analyze_BestShapeFit
 * Method:    finishPoint
 * Signature: (FF)Ljava/lang/String;
 */
jfloat Java_analyze_BestShapeFit_finishPoint(JNIEnv *env, jobject thiz,
		jfloat x, jfloat y) {
//	lpoint->a++;
	l.a++;
	LOGV("finishPoint");
	return l.a;
}
#ifdef __cplusplus

}

#endif
