package com.zhyfzy.srcnn;


import android.graphics.Bitmap;


class ImageProcessor {

    private Bitmap bitmap;
    private int upScale = 2;

    void loadBitmap(Bitmap image){
        bitmap = image;
    }
    void setUpScale(int us){upScale = us;}

    Bitmap srcnnProcess(){
        if (bitmap == null) return null;
        int w = bitmap.getWidth();
        int h = bitmap.getHeight();
        int[] pixels = new int[w*h];
        bitmap.getPixels(pixels, 0, w, 0, 0, w, h);
        int[] resultInt = nativeSrcnnProcess(pixels, w, h,upScale);
        Bitmap resultImg = Bitmap.createBitmap(w*upScale, h*upScale, Bitmap.Config.ARGB_8888);
        resultImg.setPixels(resultInt, 0, w*upScale, 0, 0, w*upScale, h*upScale);
        return resultImg;
    }

    static {
        System.loadLibrary("native-lib");
    }

    public static native int[] nativeSrcnnProcess(int[] pixels, int w, int h, int upScale);

}
