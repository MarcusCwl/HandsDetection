#include <jni.h>
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>





int main(int argc,unsigned char* argv[])
{
	IplImage* frame1=NULL;
	IplImage* frame2=NULL;
	


	while(1)
	{

		frame1 = cvQueryFrame(capture);
		if(!frame1)
		{
			break;
		}
		IplImage* gray1 = cvCreateImage(cvGetSize(frame1), IPL_DEPTH_8U, 1);
		

		cvCvtColor(frame1,gray1,CV_BGR2GRAY);


		frame2 = cvQueryFrame(capture);		
		if(!frame2)
		{
			break;
		}
		IplImage* gray2 = cvCreateImage(cvGetSize(frame2), IPL_DEPTH_8U, 1);
		cvCvtColor(frame2,gray2,CV_BGR2GRAY);
		IplImage* gray_diff = cvCreateImage(cvGetSize(gray2), IPL_DEPTH_8U, 1);
		cvAbsDiff(gray1,gray2,gray_diff);


		//CvScalar t;
		//for(int i=0;i<gray_diff->height;i++)
		//{
		//	for(int j=0;j<gray_diff->width;j++)
		//	{
		//		t=cvGet2D(gray_diff,i,j);
		//		if(t.val[0]>30)
		//			t.val[0]=255;
		//		else
		//			t.val[0]=0;
		//		cvSet2D(gray_diff,i,j,t);
		//	}
		//}
		for ( int i = 0; i < gray_diff->height; ++i )
		{
			uchar * pucPixel = (uchar*)gray_diff->imageData + i*gray_diff->widthStep;
            for ( int j = 0; j < gray_diff->width; ++j )
			{       
			if(pucPixel[j]>35)
				pucPixel[j]=255;
			else
				pucPixel[j]=0;

            }

		}

		IplImage *gray_diff_erode = cvCreateImage(cvGetSize(gray_diff), 8, 1); 
		IplImage *gray_diff_dilate = cvCreateImage(cvGetSize(gray_diff), 8, 1); 

		cvDilate(gray_diff,gray_diff_dilate, NULL,1); //���� 
		cvErode(gray_diff_dilate,gray_diff_erode, NULL,1); //��ʴ  
		

		cvNamedWindow("avi");
//		cvNamedWindow("act");
		
		cvNamedWindow("222");
		/*��ʾ��Ƶ֡*/
		cvShowImage("avi",frame1);
	//	cvShowImage("222",gray_diff);
		cvShowImage("222",gray_diff_erode);
		
		
		char c = cvWaitKey(60);

		/*�˳�ѭ��*/
		if(c == 27)
		{
			break;
		}

		
		cvReleaseImage(&gray1);
		cvReleaseImage(&gray2);
		cvReleaseImage(&gray_diff);
		cvReleaseImage(&gray_diff_erode);
		cvReleaseImage(&gray_diff_dilate);
	}
		
	cvReleaseCapture(&capture);

	/*ע�����ڣ��ͷ�ͼ�񸱱�*/
	cvDestroyWindow("avi");
	cvDestroyWindow("act");
   
    return 0;
}