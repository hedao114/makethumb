#include "makethumb.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	makethumb w;
	w.show();
	return a.exec();
}
