#include <jni.h>
#include <string>
#include <opencv2/opencv.hpp>
#include <srcnn.hpp>

extern "C"{

using namespace cv;
using namespace std;

JNIEXPORT jintArray JNICALL Java_com_zhyfzy_srcnn_ImageProcessor_nativeSrcnnProcess(JNIEnv *env, jclass obj, jintArray buf, jint w, jint h){
    jboolean ptfalse = false;
    jint* srcBuf = env->GetIntArrayElements(buf, &ptfalse);
    if(srcBuf == NULL){
        return 0;
    }
    int size=w * h;

    Mat srcImage(h, w, CV_8UC4, (unsigned char*)srcBuf);

    srcImage = srcnn(srcImage);

    jintArray result = env->NewIntArray(size*2);
    env->SetIntArrayRegion(result, 0, size*2, srcBuf);
    env->ReleaseIntArrayElements(buf, srcBuf, 0);
    return result;
}

}
