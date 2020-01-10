#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_HCIProj.h"

class HCIProj : public QMainWindow
{
	Q_OBJECT

public:
	HCIProj(QWidget *parent = Q_NULLPTR);

private:
	Ui::HCIProjClass ui;

private:
	void bindSlotsAndSignals();
};
