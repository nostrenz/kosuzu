#include "aboutwindow.h"
#include "ui_aboutwindow.h"
#include <src/main.h>

AboutWindow::AboutWindow(QWidget *parent) : QDialog(parent), ui(new Ui::AboutWindow)
{
	setAttribute(Qt::WA_DeleteOnClose);

	ui->setupUi(this);

	QImage appIcon(APP_DIR + "/assets/suzu.png");
	QPixmap pixmap = QPixmap::fromImage(appIcon);
	ui->labelIcon->setPixmap(pixmap.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation));

	QFont font("Arial", 14, QFont::Bold);
	ui->labelName->setFont(font);

	this->setModal(true);
	this->setWindowFlags(Qt::Tool);
}

AboutWindow::~AboutWindow()
{
	delete ui;
}
