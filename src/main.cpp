#include <src/main.h>
#include <src/gui/window/mainwindow.h>
#include <lib/SingleApplication/SingleApplication>
#include <QTextStream>
#include <QTextCodec>

QString APP_DIR;

int main(int argc, char* argv[])
{
	// Use UTF-8
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

	SingleApplication a(argc, argv, true);
	QString kszPath;

	// Get KSZ path from command line arguments
	if (QCoreApplication::arguments().size() >= 2) {
		kszPath = QCoreApplication::arguments().at(1);
	}

	// Secondary instance, send the KSZ path to the primary one
	if (a.isSecondary()) {
		a.sendMessage(kszPath.toUtf8());
		a.exit(0);

		return 0;
	}

	// Keep path to the app directory
	APP_DIR = a.applicationDirPath();

	MainWindow w;

	// Listen for message sent by the secondary instance
	QObject::connect(
		&a,
		&SingleApplication::receivedMessage,
		&w,
		&MainWindow::receivedMessage
	);

	// Load style
	QFile f(APP_DIR + "/assets/qdarkstyle/style.qss");

	if (f.exists() && f.open(QFile::ReadOnly | QFile::Text)) {
		QTextStream ts(&f);
		a.setStyleSheet(ts.readAll());
	}

	// Open KSZ from command line argument
	if (!kszPath.isNull() & !kszPath.isEmpty()) {
		w.openKsz(kszPath);
	}

	w.showMaximized();

	return a.exec();
}
