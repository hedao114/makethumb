#include "makethumb.h"
#include <QtWidgets/QApplication>
#include <QTextCodec>

int main(int argc, char *argv[])
{

	QTextCodec *codec = QTextCodec::codecForName("utf-8");
	QTextCodec::setCodecForLocale(codec);

	QApplication a(argc, argv);
	makethumb w;
	w.show();
	return a.exec();
}
