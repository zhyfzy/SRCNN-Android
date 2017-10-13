/* Program 	: Image Super-Resolution using deep Convolutional Neural Networks
 * Author  	: Wang Shu
 * Date		: Sun 13 Sep, 2015
 * Descrip.	:
* */

#ifndef _SRCNN_HPP
#define _SRCNN_HPP

#include <iostream>
#include <iomanip>

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "android/log.h"

using namespace std;
using namespace cv;

/* Marco Definition */
#define IMAGE_WIDTH		960		// the image width
#define IMAGE_HEIGHT		540		// the image height
#define UP_SCALE		2		// the scale of super-resolution
#define CONV1_FILTERS		64		// the first convolutional layer
#define CONV2_FILTERS		32		// the second convolutional layer
#define DEBUG			1		// show the debug info
//#define DISPLAY			0		// display the temp image
//#define SAVE			0		// save the temp image
//#define CUBIC			1

/* Load the convolutional data */
#include "convdata.h"

/* Function Declaration */
void Convolution99(Mat& src, Mat& dst, float kernel[9][9], float bias);
void Convolution11(vector<Mat>& src, Mat& dst, float kernel[CONV1_FILTERS], float bias);
void Convolution55(vector<Mat>& src, Mat& dst, float kernel[32][5][5], float bias);

/***
 * FuncName	: main
 * Function	: the entry of the program
 * Parameter	: pImgOrigin -- the input image.
 * Output	: pImgBGROut -- the output image.
***/
Mat srcnn(Mat pImgOrigin)
{
	/* Read the original image */
    __android_log_print(ANDROID_LOG_INFO,"JNI","Read the Original Image Successfully ...");
#ifdef SAVE
	imwrite("/Sdcard/Result/Origin.bmp", pImgOrigin);
#endif

	/* Convert the image from BGR to YCrCb Space */
	Mat pImgYCrCb;
	cvtColor(pImgOrigin, pImgYCrCb, CV_BGR2YCrCb);
#ifdef DEBUG
    __android_log_print(ANDROID_LOG_INFO,"JNI","Convert the Image to YCrCb Sucessfully ...");
#endif

	/* Split the Y-Cr-Cb channel */
	vector<Mat> pImgYCrCbCh(3);
	split(pImgYCrCb, pImgYCrCbCh);
#ifdef DISPLAY
	imshow("Luma", pImgYCrCbCh[0]);
#endif
#ifdef SAVE
	imwrite("/Sdcard/Result/Luma.bmp", pImgYCrCbCh[0]);
#endif
#ifdef DEBUG
    __android_log_print(ANDROID_LOG_INFO,"JNI","Spliting the Y-Cr-Cb Channel ...");
#endif

	/* Resize the Y-Cr-Cb Channel with Bicubic Interpolation */
	vector<Mat> pImg(3);
	for (int i = 0; i < 3; i++)
	{
		resize(pImgYCrCbCh[i], pImg[i], pImgYCrCbCh[i].size()*UP_SCALE, 0, 0, CV_INTER_CUBIC);
	}
#ifdef DISPLAY
	imshow("LumaCubic", pImg[0]);
#endif
#ifdef SAVE
	imwrite("/Sdcard/Result/LumaCubic.bmp", pImg[0]);
#endif
#ifdef DEBUG
    __android_log_print(ANDROID_LOG_INFO,"JNI","Completed Bicubic Interpolation ...");
#endif

#ifdef CUBIC
	/* Output the Cubic Inter Result (Optional) */
	Mat pImgCubic;
	resize(pImgYCrCb, pImgCubic, pImgYCrCb.size()*UP_SCALE, 0, 0, CV_INTER_CUBIC);
	cvtColor(pImgCubic, pImgCubic, CV_YCrCb2BGR);
#ifdef DISPLAY
	imshow("Cubic", pImgCubic);
#endif
//#ifdef SAVE
	imwrite("/Sdcard/Result/Cubic.bmp", pImgCubic);
//#endif
#ifdef DEBUG
    __android_log_print(ANDROID_LOG_INFO,"JNI","Completed Bicubic Version (Optional) ...");
#endif
#endif

	/******************* The First Layer *******************/
	vector<Mat> pImgConv1(CONV1_FILTERS);
	for (int i = 0; i < CONV1_FILTERS; i++)
	{
		pImgConv1[i].create(pImg[0].size(), CV_32F);
		Convolution99(pImg[0], pImgConv1[i], weights_conv1_data[i], biases_conv1[i]);
#ifdef DEBUG
        __android_log_print(ANDROID_LOG_INFO,"JNI","Convolutional Layer I : %d %d/64 Cell Completed ...", setw(2),i+1);
#endif
	}
#ifdef DISPLAY
	imshow("Conv1", pImgConv1[8]);
#endif
#ifdef SAVE
	imwrite("/Sdcard/Result/Conv1.bmp", pImgConv1[8]);
#endif
#ifdef DEBUG
    __android_log_print(ANDROID_LOG_INFO,"JNI","Convolutional Layer I : 100% Complete ...");
#endif
	
	/******************* The Second Layer *******************/
	vector<Mat> pImgConv2(CONV2_FILTERS);
	for (int i = 0; i < CONV2_FILTERS; i++)
	{
		pImgConv2[i].create(pImg[0].size(), CV_32F);
		Convolution11(pImgConv1, pImgConv2[i], weights_conv2_data[i], biases_conv2[i]);
#ifdef DEBUG
        __android_log_print(ANDROID_LOG_INFO,"JNI","Convolutional Layer II : %d %d/32 Cell Completed ...", setw(2),i+1);
#endif
	}
#ifdef DISPLAY
	imshow("Conv2", pImgConv2[31]);
#endif
#ifdef SAVE
	imwrite("/Sdcard/Result/Conv2.bmp", pImgConv2[31]);
#endif
#ifdef DEBUG
    __android_log_print(ANDROID_LOG_INFO,"JNI","Convolutional Layer II : 100% Complete ...");
#endif

	/******************* The Third Layer *******************/
	Mat pImgConv3;
	pImgConv3.create(pImg[0].size(), CV_8U);
	Convolution55(pImgConv2, pImgConv3, weights_conv3_data, biases_conv3);
#ifdef DISPLAY
	imshow("Conv3", pImgConv3);
#endif
#ifdef SAVE
	imwrite("/Sdcard/Result/Conv3.bmp", pImgConv3);
#endif
#ifdef DEBUG
    __android_log_print(ANDROID_LOG_INFO,"JNI","Convolutional Layer III : 100% Complete ...");
#endif
	
	/* Merge the Y-Cr-Cb Channel into an image */
	Mat pImgYCrCbOut;
	merge(pImg, pImgYCrCbOut);
#ifdef DEBUG
    __android_log_print(ANDROID_LOG_INFO,"JNI","Merge Image Complete...");
#endif

	/* Convert the image from YCrCb to BGR Space */
	Mat pImgBGROut;
	cvtColor(pImgYCrCbOut, pImgBGROut, CV_YCrCb2BGR);
#ifdef DISPLAY
	imshow("Output", pImgBGROut);
#endif
#ifdef SAVE
	imwrite("/Sdcard/Result/Output.bmp", pImgBGROut);
#endif
#ifdef DEBUG
    __android_log_print(ANDROID_LOG_INFO,"JNI","Convert the Image to BGR Sucessfully ...");
#endif

	//cvWaitKey();

	return pImgBGROut;
}

/***
 * FuncName	: Convolution99
 * Function	: Complete one cell in the first Convolutional Layer
 * Parameter	: src - the original input image
 *		  dst - the output image
 *		  kernel - the convolutional kernel
 *		  bias - the cell bias
 * Output	: <void>
***/
void Convolution99(Mat& src, Mat& dst, float kernel[9][9], float bias)
{
	/* Expand the src image */
	Mat src2;
	src2.create(Size(src.cols + 8, src.rows + 8), CV_8U);
	
	for (int row = 0; row < src2.rows; row++)
	{
		for (int col = 0; col < src2.cols; col++)
		{
			int tmpRow = row - 4;
			int tmpCol = col - 4;

			if (tmpRow < 0)
				tmpRow = 0;
			else if (tmpRow >= src.rows)
				tmpRow = src.rows - 1;

			if (tmpCol < 0)
				tmpCol = 0;
			else if (tmpCol >= src.cols)
				tmpCol = src.cols - 1;

			src2.at<unsigned char>(row, col) = src.at<unsigned char>(tmpRow, tmpCol);
		}
	}
#ifdef DISPLAY
	//imshow("Src2", src2);
#endif

	/* Complete the Convolution Step */
	for (int row = 0; row < dst.rows; row++)
	{
		for (int col = 0; col < dst.cols; col++)
		{
			/* Convolution */
			float temp = 0;
			for (int i = 0; i < 9; i++)
			{
				for (int j = 0; j < 9; j++)
				{
					temp += kernel[i][j] * src2.at<unsigned char>(row + i, col + j);
				}
			}
			temp += bias;

			/* Threshold */
			temp = (temp >= 0) ? temp : 0;

			dst.at<float>(row, col) = temp;
		}
	}

	return;
}

/***
 * FuncName	: Convolution11
 * Function	: Complete one cell in the second Convolutional Layer
 * Parameter	: src - the first layer data
 *		  dst - the output data
 *		  kernel - the convolutional kernel
 *		  bias - the cell bias
 * Output	: <void>
***/
void Convolution11(vector<Mat>& src, Mat& dst, float kernel[CONV1_FILTERS], float bias)
{
	for (int row = 0; row < dst.rows; row++)
	{
		for (int col = 0; col < dst.cols; col++)
		{
			/* Process with each pixel */
			float temp = 0;
			for (int i = 0; i < CONV1_FILTERS; i++)
			{
				temp += src[i].at<float>(row, col) * kernel[i];
			}
			temp += bias;

			/* Threshold */
			temp = (temp >= 0) ? temp : 0;

			dst.at<float>(row, col) = temp;
		}
	}

	return;
}

/***
 * FuncName	: Convolution55
 * Function	: Complete the cell in the third Convolutional Layer
 * Parameter	: src - the second layer data 
 *		  dst - the output image
 *		  kernel - the convolutional kernel
 *		  bias - the cell bias
 * Output	: <void>
***/
void Convolution55(vector<Mat>& src, Mat& dst, float kernel[32][5][5], float bias)
{
	/* Expand the src image */
	vector<Mat> src2(CONV2_FILTERS);
	for (int i = 0; i < CONV2_FILTERS; i++)
	{
		src2[i].create(Size(src[i].cols + 4, src[i].rows + 4), CV_32F);
		for (int row = 0; row < src2[i].rows; row++)
		{
			for (int col = 0; col < src2[i].cols; col++)
			{
				int tmpRow = row - 2;
				int tmpCol = col - 2;

				if (tmpRow < 0)
					tmpRow = 0;
				else if (tmpRow >= src[i].rows)
					tmpRow = src[i].rows - 1;

				if (tmpCol < 0)
					tmpCol = 0;
				else if (tmpCol >= src[i].cols)
					tmpCol = src[i].cols - 1;

				src2[i].at<float>(row, col) = src[i].at<float>(tmpRow, tmpCol);
			}
		}
	}

	/* Complete the Convolution Step */
	for (int row = 0; row < dst.rows; row++)
	{
		for (int col = 0; col < dst.cols; col++)
		{
			float temp = 0;

			for (int i = 0; i < CONV2_FILTERS; i++)
			{
				double temppixel = 0;
				for (int m = 0; m < 5; m++)
				{
					for (int n = 0; n < 5; n++)
					{
						temppixel += kernel[i][m][n] * src2[i].at<float>(row + m, col + n);
					}
				}

				temp += temppixel;
			}

			temp += bias;

			/* Threshold */
			temp = (temp >= 0) ? temp : 0;
			temp = (temp <= 255) ? temp : 255;

			dst.at<unsigned char>(row, col) = (unsigned char)temp;
		}
#ifdef DEBUG
		cout << "Convolutional Layer III : " << setw(4) << row + 1 << '/' << dst.rows << " Complete ..." << endl;
#endif
	}

	return;
}

#endif // SRCNN_HPP