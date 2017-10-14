#include <jni.h>
#include <string>
#include <opencv2/opencv.hpp>
#include <srcnn.hpp>
#include "android/log.h"

extern "C"{

using namespace cv;
using namespace std;

JNIEXPORT jintArray JNICALL Java_com_zhyfzy_srcnn_ImageProcessor_nativeSrcnnProcess(JNIEnv *env, jclass obj, jintArray buf, jint w, jint h, jint upScale){
    jboolean ptfalse = false;
    jint* srcBuf = env->GetIntArrayElements(buf, &ptfalse);
    if(srcBuf == NULL){
        return 0;
    }
    int srcSize=w * h;
    Mat srcImage(h, w, CV_8UC4, (unsigned char*)srcBuf);
    Mat sImgBGR;
    cvtColor(srcImage, sImgBGR, CV_BGRA2BGR);

    Mat oImgBGR = srcnn(sImgBGR, upScale);

    Mat oImgBGRA;
    cvtColor(oImgBGR, oImgBGRA, CV_BGR2BGRA);
    int outSize = oImgBGRA.size().height * oImgBGRA.size().width;
    cv::MatIterator_<int> it;
    int* outBuf = new int [outSize * 4];
    int head = 0;
    for (it = oImgBGRA.begin<int>(); it != oImgBGRA.end<int>(); it++){
        outBuf[head++] = *it;
    }
    jintArray result = env->NewIntArray(head);
    env->SetIntArrayRegion(result, 0, head, outBuf);
    env->ReleaseIntArrayElements(buf, srcBuf, 0);
    delete[] outBuf;
    return result;
}

}
