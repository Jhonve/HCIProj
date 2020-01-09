#include "HCIProj.h"
#include <QtWidgets/QApplication>

//#include <opencv2/opencv.hpp>
//
//#include <dlib/image_processing/frontal_face_detector.h>
//#include <dlib/gui_widgets.h>
//#include <dlib/image_io.h>
//#include <dlib/image_processing.h>
//#include <dlib/opencv/cv_image.h>
//#include <iostream>

int main(int argc, char *argv[])
{
	//dlib::frontal_face_detector detector = dlib::get_frontal_face_detector();
	//dlib::image_window win;

	//std::cout << "processing image " << "./test.jpg" << std::endl;

	//cv::Mat cv_img = cv::imread("./test.jpg");
	//cv::imshow("test", cv_img);
	//cv::waitKey(0);

	//dlib::array2d<dlib::bgr_pixel> img;
	//// dlib::load_image(img, "./test.jpg");
	//dlib::assign_image(img, dlib::cv_image<dlib::bgr_pixel>(cv_img));
	//// Make the image bigger by a factor of two.  This is useful since
	//// the face detector looks for faces that are about 80 by 80 pixels
	//// or larger.  Therefore, if you want to find faces that are smaller
	//// than that then you need to upsample the image as we do here by
	//// calling pyramid_up().  So this will allow it to detect faces that
	//// are at least 40 by 40 pixels in size.  We could call pyramid_up()
	//// again to find even smaller faces, but note that every time we
	//// upsample the image we make the detector run slower since it must
	//// process a larger image.
	//dlib::pyramid_up(img);

	//// Now tell the face detector to give us a list of bounding boxes
	//// around all the faces it can find in the image.
	//std::vector<dlib::rectangle> dets = detector(img);

	//std::cout << "Number of faces detected: " << dets.size() << std::endl;
	//// Now we show the image on the screen and the face detections as
	//// red overlay boxes.
	//win.clear_overlay();
	//win.set_image(img);
	//win.add_overlay(dets, dlib::rgb_pixel(255, 0, 0));

	/*cv::VideoCapture v_c(0);

	while (true)
	{
		cv::Mat frame;
		v_c >> frame;
		cv::imshow("Read video", frame);
		cv::waitKey(30);
	}*/

	QApplication a(argc, argv);
	HCIProj w;
	w.show();
	return a.exec();
}
