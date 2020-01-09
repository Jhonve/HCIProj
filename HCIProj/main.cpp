#include "HCIProj.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	HCIProj w;
	w.show();
	return a.exec();
}
