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
#include"luo.h"
/* This is a trivial JNI example where we use a native method
 * to return a new VM String. See the corresponding Java source
 * file located at:
 *
 *   apps/samples/hello-jni/project/src/com/example/hellojni/HelloJni.java
 */
#ifdef __cplusplus

extern "C" {

#endif
using namespace std;
luohaoxin l;
luohaoxin * lpoint;
/*
 * Class:     analyze_BestShapeFit
 * Method:    startPoint
 * Signature: (FF)V
 */
void Java_analyze_BestShapeFit_startPoint(JNIEnv *env, jobject thiz, jfloat x,
		jfloat y) {
	lpoint=new luohaoxin();
}

/*
 * Class:     analyze_BestShapeFit
 * Method:    updatePoint
 * Signature: (FF)V
 */
void Java_analyze_BestShapeFit_updatePoint(JNIEnv *env, jobject thiz, jfloat x,
		jfloat y) {

}

/*
 * Class:     analyze_BestShapeFit
 * Method:    finishPoint
 * Signature: (FF)Ljava/lang/String;
 */
jstring Java_analyze_BestShapeFit_finishPoint(JNIEnv *env, jobject thiz,
		jfloat x, jfloat y) {
	int a = lpoint->getInt();
	if (a == 123) {
		//		return (env)->NewStringUTF(env, "123");
		return env->NewStringUTF("12131213!");
	} else {
		return env->NewStringUTF("Hello World JNI!");
	}
}
#ifdef __cplusplus

}

#endif
