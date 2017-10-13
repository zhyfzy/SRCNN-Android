package com.zhyfzy.srcnn;

import android.app.Activity;
import android.app.ProgressDialog;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

public class srcnn extends Activity implements View.OnClickListener{

    private ImageProcessor imageProcessor;
    final int PHOTO_SRCNN_PROCESS = 0;
    private ImageView imageViewOriginImage;
    private ImageView imageViewProcessedImage;
    Bitmap image;
    Uri uri; // the uri of image you select from gallery
    ProgressDialog progressDialog;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_srcnn);
        Button buttonSrcnnProcess = (Button) findViewById(R.id.buttonSrcnnProcess);
        buttonSrcnnProcess.setOnClickListener(this);
        imageViewOriginImage = (ImageView) findViewById(R.id.imageViewOriginImage);
        imageViewProcessedImage = (ImageView) findViewById(R.id.imageViewProcessedImage);

        imageProcessor = new ImageProcessor();
    }

    @Override
    public void onClick(View v){
        switch (v.getId()){
            case R.id.buttonSrcnnProcess:
                selectImageAndProcess(PHOTO_SRCNN_PROCESS);
                break;
            default:
                break;
        }
    }

    private void selectImageAndProcess(int requestCode){
        Intent intent = new Intent(Intent.ACTION_PICK);
        intent.setType("image/*");
        startActivityForResult(intent, requestCode);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data){
        if (resultCode == RESULT_OK){
            switch (requestCode){
                case PHOTO_SRCNN_PROCESS:
                    uri = data.getData();
                    srcnnProcess(uri);
                    break;
                default:
                    break;
            }
        }
    }

    private void srcnnProcess(Uri uri){
        imageViewOriginImage.setImageURI(uri);
        new Thread(new threadSrcnn()).start();
        progressDialog = new ProgressDialog(this);
        progressDialog.setProgressStyle(ProgressDialog.STYLE_SPINNER);
        progressDialog.setTitle("Processing...");
        progressDialog.setMessage("Please wait...");
        progressDialog.setCancelable(false);
        progressDialog.show();
    }

    public void saveMyBitmap(Bitmap mBitmap, String bitName){
        File f = new File(Environment.getExternalStorageDirectory() + "/" +  bitName + ".png");
        Log.i(" ", f.getAbsolutePath());
        try {
            f.createNewFile();
        } catch (IOException e) {
            // TODO Auto-generated catch block
        }
        FileOutputStream fOut = null;
        try {
            fOut = new FileOutputStream(f);
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        }
        mBitmap.compress(Bitmap.CompressFormat.PNG, 100, fOut);
        try {
            fOut.flush();
        } catch (IOException e) {
            e.printStackTrace();
        }
        try {
            fOut.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    private class threadSrcnn implements Runnable{
        @Override
        public void run() {
            try{
                InputStream stream = getContentResolver().openInputStream(uri);
                Bitmap image = BitmapFactory.decodeStream(stream);
                imageProcessor.loadBitmap(image);
                Bitmap resultBitmap = imageProcessor.srcnnProcess();
                Message msg = Message.obtain();
                msg.obj = resultBitmap;
                msg.what = 0;
                handleSrcnn.sendMessage(msg);

            }catch(IOException e){
                e.getStackTrace();
            }
        }
    }

    private Handler handleSrcnn = new Handler(){
        @Override
        public void handleMessage(Message msg){
            Bitmap resultBitmap = (Bitmap)msg.obj;
            imageViewProcessedImage.setImageBitmap(resultBitmap);
            saveMyBitmap(resultBitmap, "Output");
            progressDialog.dismiss();
        }
    };
}
