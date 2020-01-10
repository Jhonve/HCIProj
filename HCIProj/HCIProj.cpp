#include "HCIProj.h"

HCIProj::HCIProj(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	bindSlotsAndSignals();
}

void HCIProj::bindSlotsAndSignals()
{
	connect(ui.button_model_load, SIGNAL(clicked()), ui.main_opengl_widget, SLOT(slotLoadModel()));
}