#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_makethumb.h"

class makethumb : public QMainWindow
{
	Q_OBJECT

public:
	makethumb(QWidget *parent = Q_NULLPTR);

private:
	Ui::makethumbClass ui;
};
