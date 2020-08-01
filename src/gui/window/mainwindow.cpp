#include <ui_mainwindow.h>
#include <src/utils.h>
#include <src/serializer.h>
#include <src/gui/window/mainwindow.h>
#include <src/download/downloader.h>
#include <src/danbooru/danbooruurl.h>
#include <src/ksz/kszreader.h>
#include <src/gui/widget/notelabel.h>
#include <src/gui/window/collectionwindow.h>
#include <src/gui/window/downloadwindow.h>
#include <src/gui/window/aboutwindow.h>
#include <src/settings.h>
#include <QMessageBox>
#include <QCoreApplication>
#include <QDir>
#include <QProgressBar>
#include <QKeyEvent>
#include <QDesktopServices>
#include <QKeyEvent>
#include <QToolTip>
#include <QScrollBar>
#include <QtMath>
#include <QInputDialog>
#include <QMenu>
#include <QFileDialog>
#include <QMimeData>
#include <QFontDatabase>
#include <QToolBar>
#include <QWidgetAction>
#include <QSlider>

///
/// Constant
///

// Used to counter some margins imposed by the scrollarea
const int MARGIN = 8;

///
/// Constructor / Destructor
///

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	this->setFocusPolicy(Qt::FocusPolicy::NoFocus);
	this->setWindowIcon(QIcon(APP_DIR + "/assets/suzu.png"));

	// Restore settings
	Settings settings;
	settings.read();

	m_fontSlider = new QSlider(this);
	m_fontSlider->setOrientation(Qt::Horizontal);
	m_fontSlider->setMinimum(6);
	m_fontSlider->setMaximum(22);
	m_fontSlider->setStyleSheet("background-color: #505050;");
	m_fontSlider->setValue((settings.fontSize() >= m_fontSlider->minimum() && settings.fontSize() <= m_fontSlider->maximum()) ? settings.fontSize() : FONT_SIZE);
	m_fontSlider->setMaximumWidth(100);

	ui->scrollArea->installEventFilter(this);
	ui->scrollArea->setBackgroundRole(QPalette::Dark);
	ui->scrollArea->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->menuBar->setStyleSheet("border: none;");
	ui->buttonNotes->setChecked(settings.notes());
	ui->buttonFit->setChecked(settings.fit());
	ui->buttonRightToLeft->setChecked(settings.rightToLeft());
	ui->statusBar->addPermanentWidget(m_fontSlider);

	this->setIcon(ui->buttonPage, "page");
	this->setIcon(ui->buttonNotes, "note");
	this->setIcon(ui->buttonStyle, "switch");
	this->setIcon(ui->buttonFit, "fit");
	this->setIcon(ui->buttonRightToLeft, "rtl");
	this->setIcon(ui->buttonFullscreen, "expand");
	this->setIcon(ui->buttonCollection, "bookmark");
	this->setIcon(ui->buttonDownloader, "download");
	this->setIcon(ui->buttonFirst, "first");
	this->setIcon(ui->buttonPrev, "previous");
	this->setIcon(ui->buttonNext, "next");
	this->setIcon(ui->buttonLast, "last");

	this->toggleButtonStyle(ui->buttonNotes);
	this->toggleButtonStyle(ui->buttonFit);
	this->toggleButtonStyle(ui->buttonRightToLeft);
	this->toggleButtonStyle(ui->buttonFullscreen);
	this->toggleButtonStyle(ui->buttonCollection);
	this->toggleButtonStyle(ui->buttonDownloader);

	connect(ui->actionOpenKszFile, SIGNAL(triggered()), this, SLOT(onOpenKsz()));
	connect(ui->actionOpenProgramFolder, SIGNAL(triggered()), this, SLOT(onOpenProgramFolder()));
	connect(ui->actionOpenDownloadsFolder, SIGNAL(triggered()), this, SLOT(onOpenDownloadsFolder()));
	connect(ui->actionGitHubRepository, SIGNAL(triggered()), this, SLOT(onGitHubRepository()));
	connect(ui->actionAboutQt, SIGNAL(triggered()), this, SLOT(onAboutQt()));
	connect(ui->actionAboutKosuzu, SIGNAL(triggered()), this, SLOT(onAboutKosuzu()));
	connect(ui->buttonNotes, SIGNAL(clicked()), this, SLOT(onToggleNotes()));
	connect(ui->buttonStyle, SIGNAL(clicked()), this, SLOT(onToggleNoteStyle()));
	connect(ui->buttonFit, SIGNAL(clicked()), this, SLOT(onFitButton()));
	connect(ui->buttonFirst, SIGNAL(clicked()), this, SLOT(onFirstPage()));
	connect(ui->buttonPrev, SIGNAL(clicked()), this, SLOT(onPreviousPage()));
	connect(ui->buttonPage, SIGNAL(clicked()), this, SLOT(onJumpToPage()));
	connect(ui->buttonNext, SIGNAL(clicked()), this, SLOT(onNextPage()));
	connect(ui->buttonLast, SIGNAL(clicked()), this, SLOT(onLastPage()));
	connect(ui->buttonRightToLeft, SIGNAL(clicked()), this, SLOT(onRightToLeftButton()));
	connect(ui->buttonFullscreen, SIGNAL(clicked()), this, SLOT(onToggleFullscreen()));
	connect(ui->buttonCollection, SIGNAL(clicked()), this, SLOT(onToggleCollection()));
	connect(ui->buttonDownloader, SIGNAL(clicked()), this, SLOT(onToggleDownloader()));
	connect(ui->scrollArea, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onScrollAreaContextMenu(QPoint)));
	connect(m_fontSlider, SIGNAL(valueChanged(int)), this, SLOT(onFontSliderValueChanged(int)));

	// Load font
	int fontId = QFontDatabase::addApplicationFont(APP_DIR + "/assets/CC Wild Words.ttf");

	if (fontId != -1) {
		m_fontFamily = QFontDatabase::applicationFontFamilies(fontId).at(0);
	}

	// Restore last KSZ
	bool restored = false;

	if (!settings.lastKsz().isNull()) {
		restored = this->openKsz(settings.lastKsz(), settings.lastPage());
	}

	// Default image
	if (!restored) {
		QImage appIcon(APP_DIR + "/assets/suzu.png");
		QPixmap pixmap = QPixmap::fromImage(appIcon);
		ui->imageLabel->setPixmap(pixmap);
	}
}

MainWindow::~MainWindow()
{
	this->unloadKsz();

	delete ui;
}

///
/// Public method
///

bool MainWindow::openKsz(QString filePath, unsigned int page)
{
	Ksz* ksz = KszReader::read(filePath);

	if (ksz == nullptr) {
		return false;
	}

	ksz->setPage(page > 1 ? page : 1);

	return this->openKsz(ksz);
}

bool MainWindow::openKsz(Ksz* ksz)
{
	if (ksz == nullptr) {
		return false;
	}

	this->unloadKsz();

	m_ksz = ksz;
	m_kszReader = new KszReader(ksz->path());

	Collection collection;
	collection.save(ksz, true);

	QString windowTitle = "Kosuzu";
	QString kszTitle = ksz->title().trimmed();

	if (!kszTitle.isEmpty()) {
		windowTitle += " | " + kszTitle;
	}

	this->setWindowTitle(windowTitle);
	this->jump(ksz->page()); // Load the first page

	ui->buttonNotes->setEnabled(true);
	ui->buttonStyle->setEnabled(true);
	ui->buttonFit->setEnabled(true);
	ui->buttonFullscreen->setEnabled(true);
	ui->buttonFirst->setEnabled(true);
	ui->buttonPrev->setEnabled(true);
	ui->buttonPage->setEnabled(true);
	ui->buttonNext->setEnabled(true);
	ui->buttonLast->setEnabled(true);

	return true;
}

void MainWindow::collectionClosed()
{
	// Window will be automatically deleted with the DeleteOnClose attribute
	m_collectionWindow = nullptr;

	ui->buttonCollection->setChecked(false);
	this->toggleButtonStyle(ui->buttonCollection);
}

void MainWindow::downloaderClosed()
{
	// Window will be automatically deleted with the DeleteOnClose attribute
	m_downloadWindow = nullptr;

	ui->buttonDownloader->setChecked(false);
	this->toggleButtonStyle(ui->buttonDownloader);
}

///
/// Private method
///

void MainWindow::addNote(Note* note)
{
	// Be sure to store unscaled offset values in the label
	NoteLabel *label = new NoteLabel(note, m_fontFamily);

	unsigned int x = note->x();
	unsigned int y = note->y();
	unsigned int width = note->width();
	unsigned int height = note->height();

	// Scale values
	if (m_scaleRatio != 1) {
		x = qRound((float)x * m_scaleRatio);
		y = qRound((float)y * m_scaleRatio);
		width = qRound((float)width * m_scaleRatio);
		height = qRound((float)height * m_scaleRatio);
	}

	// Get label position relative to the image
	x += this->calcTopLeftX();
	y += this->calcTopLeftY();

	label->setGeometry(x, y, width, height);
	label->setMinimumWidth(0);
	label->setMinimumHeight(0);
	label->setParent(ui->scrollAreaWidgetContents);
	label->setBackgroundStyle(m_currentNoteStyle, m_fontSlider->value(), m_scaleRatio);
	label->setVisible(ui->buttonNotes->isChecked());

	connect(label, SIGNAL(showTooltip(NoteLabel*)), this, SLOT(showTooltip(NoteLabel*)));
	connect(label, SIGNAL(hideTooltip()), this, SLOT(hideTooltip()));
}

void MainWindow::clearNotes()
{
	if (ui->scrollAreaWidgetContents->children().size() < 1) {
		return;
	}

	foreach (QObject *o, ui->scrollAreaWidgetContents->children()) {
		NoteLabel *noteLabel = qobject_cast<NoteLabel*>(o);

		// Could not be casted
		if (!noteLabel) {
			continue;
		}

		delete o;
	}
}

/**
 * Called when the window is resized.
 */
void MainWindow::resizeEvent(QResizeEvent*)
{
	// We need to wait for the first resize to display a ksz in order for size values be correct
	if (!m_initialized) {
		// Skip first resize with unitialized widgets
		if (ui->scrollArea->width() == 100 && ui->scrollArea->height() == 30) {
			return;
		}

		this->sizeButton(ui->buttonPage);
		this->sizeButton(ui->buttonNotes);
		this->sizeButton(ui->buttonStyle);
		this->sizeButton(ui->buttonFit);
		this->sizeButton(ui->buttonRightToLeft);
		this->sizeButton(ui->buttonFullscreen);
		this->sizeButton(ui->buttonFirst);
		this->sizeButton(ui->buttonPrev);
		this->sizeButton(ui->buttonNext);
		this->sizeButton(ui->buttonLast);

		m_initialized = true;
	}

	// Ksz not loaded
	if (m_ksz == nullptr) {
		return;
	}

	// Properly size widgets
	this->fixWidgetSizes();

	// Let Qt scale the image to fit the now-correclty sized widgets
	if (ui->buttonFit->isChecked()) {
		this->scaleImage();
	}

	// // Now we can update the scale ratio using the widget sizes
	this->calcScaleRatio();

	// Update note positions from the updated scale ratio
	this->refreshNotes();
}

/**
 * Update added note positions.
 */
void MainWindow::refreshNotes()
{
	int topLeftCornerX = this->calcTopLeftX();
	int topLeftCornerY = this->calcTopLeftY();

	// Iterate over added labels
	for (int i = 0; i < ui->scrollAreaWidgetContents->children().size(); i++) {
		NoteLabel *noteLabel = qobject_cast<NoteLabel*>(ui->scrollAreaWidgetContents->children().at(i));

		// Could not be casted
		if (!noteLabel) {
			continue;
		}

		int x = topLeftCornerX + qRound((float)noteLabel->offsetX() * m_scaleRatio);
		int y = topLeftCornerY + qRound((float)noteLabel->offsetY() * m_scaleRatio);
		int w = qRound((float)noteLabel->note()->width() * m_scaleRatio);
		int h = qRound((float)noteLabel->note()->height() * m_scaleRatio);

		noteLabel->setGeometry(x, y, w, h);
		noteLabel->setBackgroundStyle(m_currentNoteStyle, m_fontSlider->value(), m_scaleRatio);
	}
}

void MainWindow::refreshImage()
{
	if (!ui->buttonFit->isChecked()) {
		ui->imageLabel->setPixmap(m_post->pixmap());
	} else {
		this->scaleImage();
	}
}

/**
 * Jump to next page.
 */
void MainWindow::next()
{
	if (m_ksz->page() < m_ksz->pages()) {
		this->jump(m_ksz->page() + 1);
	}
}

/**
 * Jump to previous page.
 */
void MainWindow::previous()
{
	if (m_ksz->page() > 1) {
		this->jump(m_ksz->page() - 1);
	}
}

/**
 * Jump to a specific page.
 *
 * @param page: Starts at 1
 */
void MainWindow::jump(unsigned int page)
{
	// Delete previous post
	if (m_post && m_post != nullptr && m_post != NULL) {
		delete m_post;
		m_post = NULL;
	}

	m_ksz->setPage(page);
	m_post = m_kszReader->postAtPage(page);

	// Set page status
	ui->statusBar->showMessage("Page " + QString::number(page) + " / " + QString::number(m_ksz->pages()));

	// Clear previous notes
	this->clearNotes();
	this->hideTooltip();

	this->loadImage(); // Load image
	this->fixWidgetSizes(); // Now we can fix size of some widgets using the image size
	this->calcScaleRatio(); // Now we have reliable widget sizes to calculate a scale ratio

	// Load notes
	for (int i = 0; i < m_post->notes().size(); i++) {
		this->addNote(m_post->notes().at(i));
	}

	// Add tooltip label
	if (m_tooltipLabel == nullptr) {
		m_tooltipLabel = new TooltipLabel();
		m_tooltipLabel->setVisible(false);
		m_tooltipLabel->setParent(ui->scrollAreaWidgetContents);
	}
}

/**
 * Fix widget sizes causing notes to be misplaced and wrong scale ratio.
 */
void MainWindow::fixWidgetSizes()
{
	int height = this->height();

	if (ui->statusBar->isVisible()) {
		height -= ui->statusBar->height();
	}

	if (!this->isFullScreen()) {
		height -= (ui->widgetButtons->height() + ui->menuBar->height());
	}

	ui->imageLabel->adjustSize();
	ui->scrollAreaWidgetContents->adjustSize();
	ui->scrollArea->adjustSize();
	ui->scrollArea->resize(this->width(), height);
}

void MainWindow::loadImage()
{
	m_imageWidth = m_post->pixmap().width();
	m_imageHeight = m_post->pixmap().height();

	this->refreshImage();

	ui->imageLabel->setBackgroundRole(QPalette::Base);
	ui->imageLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	ui->imageLabel->setScaledContents(false);
}

void MainWindow::unloadKsz()
{
	if (m_ksz) {
		// Save in collection
		Collection collection;
		collection.save(m_ksz);

		delete m_ksz;
		m_ksz = nullptr;
	}

	if (m_post) {
		delete m_post;
		m_post = nullptr;
	}

	if (m_kszReader) {
		m_kszReader->close();

		delete m_kszReader;
		m_kszReader = nullptr;
	}

	if (m_tooltipLabel) {
		delete m_tooltipLabel;
		m_tooltipLabel = nullptr;
	}
}

/**
 * Return true to prevent an event to be forwarded or false to let it be forwarded.
 *
 * @param obj
 * @param event
 * @return
 */
bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
	// Pool not loaded
	if (m_ksz == nullptr) {
		return false;
	}

	// We only want to filter events to the scroll area
	if (obj != ui->scrollArea) {
		return  false;
	}

	if (event->type() == QEvent::MouseButtonRelease) {
		QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);

		if (mouseEvent->button() != Qt::LeftButton) {
			return false;
		}

		int posX = mouseEvent->x();
		int topLeftX = this->calcTopLeftX();
		int topRightX = topLeftX + (m_imageWidth*m_scaleRatio);

		if (posX < topLeftX) {
			ui->buttonRightToLeft->isChecked() ? this->next() : this->previous();
		} else if (posX > topRightX) {
			ui->buttonRightToLeft->isChecked() ? this->previous() : this->next();
		} else {
			ui->buttonNotes->setChecked(!ui->buttonNotes->isChecked());
			this->onToggleNotes();
		}

		return true;
	}

	// Don't block non-key press event (like the window resize event)
	if (event->type() != QEvent::KeyPress) {
		return false;
	}

	QKeyEvent* key = static_cast<QKeyEvent*>(event);

	if (key->key() == /*Qt::LeftArrow*/16777234) {
		ui->buttonRightToLeft->isChecked() ? this->next() : this->previous();

		return true;
	} else if (key->key() == /*Qt::RightArrow*/16777236) {
		ui->buttonRightToLeft->isChecked() ? this->previous() : this->next();

		return true;
	} else if (key->key() == /*Qt::Key_P*/80) {
		this->onJumpToPage();

		return true;
	} else if (key->key() == /*Qt::Key_N*/78) {
		ui->buttonNotes->setChecked(!ui->buttonNotes->isChecked());
		this->onToggleNotes();

		return true;
	} else if (key->key() == /*Qt::Key_S*/83 || key->key() == /*Qt::Key_F*/70) {
		ui->buttonFit->setChecked(!ui->buttonFit->isChecked());
		this->onFitAction();

		return true;
	} else if (key->key() == /*Qt::Key_Tab*/16777217) {
		this->onToggleNoteStyle();

		return true;
	} else if (key->key() == /*Qt::Key_F11*/16777274) {
		ui->buttonFullscreen->setChecked(!ui->buttonFullscreen->isChecked());
		this->onToggleFullscreen();

		return true;
	} else if (key->key() == /*Qt::Key_Escape*/16777216) {
		if (isFullScreen()) {
			showMaximized();
			ui->menuBar->setVisible(true);
			ui->widgetButtons->setVisible(true);
			ui->buttonFullscreen->setChecked(false);
			this->toggleButtonStyle(ui->buttonFullscreen);

			return true;
		}
	}

	// Let the event be forwarded
	return false;
}

void MainWindow::wheelEvent(QWheelEvent *event)
{
	// Pool not loaded
	if (m_ksz == nullptr) {
		return;
	}

	// Don't prevent normal scrolling
	if (ui->imageLabel->pixmap()->height() > ui->scrollArea->height()) {
		return;
	}

	if (event->angleDelta().y() > 0) {
		this->next();
	} else if (event->angleDelta().y() < 0) {
		this->previous();
	}

	event->accept();
}

int MainWindow::calcTopLeftX()
{
	return qRound(((float)ui->scrollAreaWidgetContents->width() / 2.0f) - (((float)ui->imageLabel->pixmap()->width()) / 2.0f));
}

int MainWindow::calcTopLeftY()
{
	return qRound(((float)ui->scrollAreaWidgetContents->height() / 2.0f) - (((float)ui->imageLabel->pixmap()->height()) / 2.0f));
}

void MainWindow::calcScaleRatio()
{
	this->fixWidgetSizes();

	if (!ui->buttonFit->isChecked()) {
		m_scaleRatio = 1;

		return;
	}

	// Image is smaller than the scroll area, we won't scale
	if (m_post->pixmap().height() <= ui->scrollArea->height() - MARGIN) {
		m_scaleRatio = 1;

		return;
	}

	float xRatio = (float)ui->imageLabel->width() / (float)m_imageWidth;
	float yRatio = (float)ui->imageLabel->height() / ((float)m_imageHeight + MARGIN);

	if (xRatio > yRatio) {
		m_scaleRatio = yRatio;

		return;
	} else if (yRatio > xRatio) {
		m_scaleRatio = xRatio;

		return;
	}

	m_scaleRatio = xRatio;
}

void MainWindow::scaleImage()
{
	if (m_post->pixmap().height() > ui->scrollArea->height() - MARGIN) {
		ui->imageLabel->setPixmap(m_post->pixmap().scaled(ui->scrollArea->width(), ui->scrollArea->height() - MARGIN, Qt::KeepAspectRatio, Qt::SmoothTransformation));
	} else {
		ui->imageLabel->setPixmap(m_post->pixmap());
	}
}

void MainWindow::switchNotesStyle()
{
	for (int i = 0; i < ui->scrollAreaWidgetContents->children().size(); i++) {
		NoteLabel *q = qobject_cast<NoteLabel*>(ui->scrollAreaWidgetContents->children().at(i));

		// Could not be casted
		if (!q) {
			continue;
		}

		q->setBackgroundStyle(m_currentNoteStyle, m_fontSlider->value(), m_scaleRatio);
	}
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	if (m_downloadWindow != nullptr) {
		if (m_downloadWindow->isDownloading()) {
			QMessageBox msgBox;
			msgBox.setText("Downloads in progress");
			msgBox.setInformativeText("You has some downloads in progress, do you really want to quit?");
			msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
			msgBox.setDefaultButton(QMessageBox::No);
			msgBox.setIcon(QMessageBox::Warning);

			auto buttonYes = msgBox.button(QMessageBox::Yes);
			buttonYes->setText("Yes, cancel dowloads");

			auto buttonNo = msgBox.button(QMessageBox::No);
			buttonNo->setText("No, keep downloading");

			msgBox.exec();

			if (msgBox.clickedButton() == buttonNo) {
				event->ignore();

				return;
			}
		}

		delete m_downloadWindow;
		m_downloadWindow = nullptr;
	}

	if (m_collectionWindow != nullptr) {
		delete m_collectionWindow;
		m_collectionWindow = nullptr;
	}

	if (m_ksz != nullptr) {
		Settings settings;

		settings.setNotes(ui->buttonNotes->isChecked());
		settings.setFit(ui->buttonFit->isChecked());
		settings.setRightToLeft(ui->buttonRightToLeft->isChecked());
		settings.setLastKsz(m_ksz->path());
		settings.setLastPage(m_ksz->page());
		settings.setFontSize(m_fontSlider->value());

		settings.write();
	}
}

void MainWindow::dragEnterEvent(QDragEnterEvent *e)
{
	if (e->mimeData()->hasUrls()) {
		e->acceptProposedAction();
	}
}

void MainWindow::dropEvent(QDropEvent* event)
{
	const QMimeData* mimeData = event->mimeData();

	if (!mimeData->hasUrls()) {
		return;
	}

	QList<QUrl> urlList = mimeData->urls();

	if (urlList.size() == 1) {
		this->openKsz(urlList.at(0).toLocalFile());
	}
}

void MainWindow::showDownloader()
{
	if (m_downloadWindow != nullptr) {
		return;
	}

	m_downloadWindow = new DownloadWindow(this);
	m_downloadWindow->setMainWindow(this);
	m_downloadWindow->centerParent();
	m_downloadWindow->show();
}

void MainWindow::setIcon(QPushButton* button, QString icon)
{
	button->setIcon(utils::icon(icon));
	button->setIconSize(QSize(19, 19));

	this->sizeButton(button);
}

void MainWindow::toggleButtonStyle(QPushButton* button)
{
	if (button->isChecked()) {
		button->setStyleSheet("background-color: #148CD2;");
	} else {
		button->setStyleSheet("background-color: #333333;");
	}

	button->adjustSize();
	button->setFixedSize(button->maximumWidth(), button->maximumHeight());
}

void MainWindow::sizeButton(QPushButton* button)
{
	button->adjustSize();
	button->setFixedSize(button->maximumWidth(), button->maximumHeight());
}

///
/// Public slot
///

void MainWindow::receivedMessage(int instanceId, QByteArray message)
{
	QString filePath(message);

	if (!filePath.isNull() && !filePath.isEmpty()) {
		this->raise();
		this->activateWindow();
		this->openKsz(filePath);
	}
}

///
/// Private slot
///

void MainWindow::onOpenKsz()
{
	QString fileName = QFileDialog::getOpenFileName(this, "Open KSZ", "./", "Book (*.ksz | *.cbz)");

	if (fileName.isNull()) {
		return;
	}

	this->openKsz(fileName);
}

void MainWindow::onToggleCollection()
{
	this->toggleButtonStyle(ui->buttonCollection);

	if (m_collectionWindow != nullptr) {
		m_collectionWindow->close();
		delete m_collectionWindow;
		m_collectionWindow = nullptr;

		return;
	}

	m_collectionWindow = new CollectionWindow(this);
	m_collectionWindow->setMainWindow(this);
	m_collectionWindow->centerParent();
	m_collectionWindow->show();
	m_collectionWindow->activateWindow();
}

void MainWindow::onToggleDownloader()
{
	if (m_downloadWindow == nullptr) {
		m_downloadWindow = new DownloadWindow(this);
		m_downloadWindow->setMainWindow(this);
		m_downloadWindow->centerParent();
		m_downloadWindow->show();
	} else {
		if (m_downloadWindow->isDownloading()) {
			QMessageBox msgBox;
			msgBox.setText("Downloads in progress");
			msgBox.setInformativeText("Are you sure to close the downloader? Running downloads will be canceled.");
			msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
			msgBox.setDefaultButton(QMessageBox::No);
			msgBox.setIcon(QMessageBox::Warning);

			auto buttonYes = msgBox.button(QMessageBox::Yes);
			buttonYes->setText("Yes, close it");

			auto buttonNo = msgBox.button(QMessageBox::No);
			buttonNo->setText("No, don't close it");

			msgBox.exec();

			if (msgBox.clickedButton() != buttonYes) {
				return;
			}
		}

		m_downloadWindow->close();
		delete m_downloadWindow;
		m_downloadWindow = nullptr;
	}

	this->toggleButtonStyle(ui->buttonDownloader);
}

void MainWindow::showTooltip(NoteLabel* noteLabel)
{
	int x = noteLabel->x();
	int y = noteLabel->y() + noteLabel->height() + 5;

	m_tooltipLabel->setText(noteLabel->body(m_fontSlider->value()));
	m_tooltipLabel->setAppearance();
	m_tooltipLabel->setGeometry(x, y, 100, 50);
	m_tooltipLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	m_tooltipLabel->adjustSize();
	m_tooltipLabel->show();
	m_tooltipLabel->raise();

	int x2 = x;
	int y2 = y;

	if (x + m_tooltipLabel->width() > ui->scrollArea->width()) {
		x2 = noteLabel->x() - m_tooltipLabel->width() - 5;
	}

	if (y + m_tooltipLabel->height() > ui->scrollArea->height()) {
		y2 = noteLabel->y() - m_tooltipLabel->height() - 5;
	}

	if (x != x2 || y != y2) {
		if (x2 < 0) {
			x2 = 0;
		}

		if (y2 < 0) {
			y2 = 0;
		}

		m_tooltipLabel->move(x2, y2);
	}
}

void MainWindow::hideTooltip()
{
	if (m_tooltipLabel != nullptr) {
		m_tooltipLabel->hide();
	}
}

void MainWindow::switchNotesStyleToDanbooru()
{
	m_currentNoteStyle = NoteLabel::BackgroundStyle::Danbooru;
	this->switchNotesStyle();
}

void MainWindow::switchNotesStyleToEmbeded()
{
	m_currentNoteStyle = NoteLabel::BackgroundStyle::Embeded;
	this->switchNotesStyle();
}

void MainWindow::onScrollAreaContextMenu(const QPoint &pos)
{
	// Pool not loaded
	if (m_ksz == nullptr) {
		return;
	}

	QMenu menu;
	QWidgetAction widgetAction(0);
	QToolBar toolbar;

	widgetAction.setDefaultWidget(&toolbar);

	toolbar.addAction(utils::icon("first"), (ui->buttonRightToLeft->isChecked() ? "Last" : "First"), this, SLOT(onFirstPage()));
	toolbar.addAction(utils::icon("previous"), (ui->buttonRightToLeft->isChecked() ? "Next" : "Previous"), this, SLOT(onPreviousPage()));
	toolbar.addAction(utils::icon("page"), "Jump", this, SLOT(onJumpToPage()));
	toolbar.addAction(utils::icon("next"), (ui->buttonRightToLeft->isChecked() ? "Previous" : "Next"), this, SLOT(onNextPage()));
	toolbar.addAction(utils::icon("last"), (ui->buttonRightToLeft->isChecked() ? "First" : "Last"), this, SLOT(onLastPage()));

	menu.addAction(&widgetAction);
	menu.addSeparator();
	menu.addAction(utils::icon("note"), "Toggle notes", this, SLOT(onToggleNotes()));
	menu.addAction(utils::icon("switch"), "Toggle note style", this, SLOT(onToggleNoteStyle()));
	menu.addAction(utils::icon("fit"), "Toggle fit", this, SLOT(onFitAction()));
	menu.addAction(utils::icon("rtl"), "Toggle right to left", this, SLOT(onRightToLeftAction()));
	menu.addAction(utils::icon("expand"), "Toggle fullscreen", this, SLOT(onToggleFullscreen()));
	menu.addAction(utils::icon("status"), "Toggle status bar", this, SLOT(onToggleStatusBar()));
	menu.addSeparator();
	menu.addAction(utils::icon("folder"), "Open folder", this, SLOT(onOpenContainingFolder()));

	if (m_ksz->poolId() > 0) {
		menu.addAction(utils::icon("url"), "Open pool URL", this, SLOT(onOpenPoolUrl()));
	}

	if (m_post->id() > 0) {
		menu.addAction(utils::icon("url"), "Open post URL", this, SLOT(onOpenPostUrl()));
	}

	if (m_post->id() > 0 || m_post->id() > 0) {
		menu.addSeparator();
		menu.addAction(utils::icon("new"), "Check for new posts", this, SLOT(onCheckForNewPosts()));
	}

	menu.exec(ui->scrollArea->mapToGlobal(pos));
}

void MainWindow::onToggleNoteStyle()
{
	if (m_currentNoteStyle == NoteLabel::BackgroundStyle::Danbooru) {
		m_currentNoteStyle = NoteLabel::BackgroundStyle::Embeded;
	} else {
		m_currentNoteStyle = NoteLabel::BackgroundStyle::Danbooru;
	}

	this->switchNotesStyle();

	if (!ui->buttonNotes->isChecked()) {
		ui->buttonNotes->setChecked(true);
		this->onToggleNotes();
	}
}

void MainWindow::onFitButton()
{
	if (m_ksz == nullptr) {
		return;
	}

	this->refreshImage(); // Let Qt scale the image to fit the scroll viewer
	this->fixWidgetSizes(); // Fix size of some widgets
	this->calcScaleRatio(); // Now we have reliable widget sizes to calculate a scale ratio
	this->refreshNotes(); // Move and resize notes based on the scale ratio

	this->toggleButtonStyle(ui->buttonFit);
}

void MainWindow::onFitAction()
{
	ui->buttonFit->setChecked(!ui->buttonFit->isChecked());

	this->onFitButton();
}

void MainWindow::onJumpToPage()
{
	if (m_ksz == nullptr) {
		return;
	}

	bool ok;
	unsigned int page = QInputDialog::getInt(this, "Page jump", "Page number:", m_ksz->page(), 1, m_ksz->pages(), 1, &ok);

	if (ok && page != m_ksz->page() && page > 0 && m_ksz->page() <= m_ksz->pages()) {
		this->jump(page);
	}
}

void MainWindow::onToggleNotes()
{
	// Iterate over added labels
	for (int i = 0; i < ui->scrollAreaWidgetContents->children().size(); i++) {
		NoteLabel *noteLabel = qobject_cast<NoteLabel*>(ui->scrollAreaWidgetContents->children().at(i));

		// Could not be casted
		if (!noteLabel) {
			continue;
		}

		noteLabel->setVisible(!noteLabel->isVisible());
	}

	this->toggleButtonStyle(ui->buttonNotes);
}

void MainWindow::onToggleFullscreen()
{
	if (m_ksz == nullptr) {
		return;
	}

	bool fullscreen = isFullScreen();

	fullscreen ? showMaximized() : showFullScreen();
	ui->menuBar->setVisible(fullscreen);
	ui->widgetButtons->setVisible(fullscreen);

	this->toggleButtonStyle(ui->buttonFullscreen);
}

void MainWindow::onRightToLeftButton()
{
	if (ui->buttonRightToLeft->isChecked()) {
		ui->buttonFirst->setToolTip("Last page");
		ui->buttonPrev->setToolTip("Next page");
		ui->buttonNext->setToolTip("Previous page");
		ui->buttonLast->setToolTip("First page");
	} else {
		ui->buttonFirst->setToolTip("First page");
		ui->buttonPrev->setToolTip("Previous page");
		ui->buttonNext->setToolTip("Next page");
		ui->buttonLast->setToolTip("Last page");
	}

	this->toggleButtonStyle(ui->buttonRightToLeft);
}

void MainWindow::onRightToLeftAction()
{
	ui->buttonRightToLeft->setChecked(!ui->buttonRightToLeft->isChecked());

	this->onRightToLeftButton();
}

void MainWindow::onOpenContainingFolder()
{
	utils::openContainingFolder(m_ksz->path());
}

void MainWindow::onOpenPoolUrl()
{
	utils::openUrl(m_ksz);
}

void MainWindow::onOpenPostUrl()
{
	utils::openUrl(m_ksz->poolId(), m_post->id());
}

void MainWindow::onCheckForNewPosts()
{
	if (m_ksz->poolId() == 0) {
		QMessageBox::warning(this, "Kosuzu", "Cannot check for new posts: missing pool ID.", QMessageBox::Ok);

		return;
	}

	DownloadQuery* query = new DownloadQuery(m_ksz, m_post->id());

	// Open downloader window
	this->showDownloader();
	bool added = m_downloadWindow->addQuery(query);

	if (!added) {
		QMessageBox::critical(this, "Kosuzu", "Error while contacting the Danbooru API.", QMessageBox::Ok);
	}
}

void MainWindow::onAboutKosuzu()
{
	AboutWindow* window = new AboutWindow();
	window->show();
}

void MainWindow::onAboutQt()
{
	QMessageBox::aboutQt(this, "About Qt");
}

void MainWindow::onOpenProgramFolder()
{
	QDesktopServices::openUrl(APP_DIR);
}

void MainWindow::onOpenDownloadsFolder()
{
	utils::openDownloadDir();
}

void MainWindow::onGitHubRepository()
{
	QDesktopServices::openUrl(QUrl("https://github.com/nostrenz/kosuzu"));
}

void MainWindow::onFirstPage()
{
	if (m_ksz != nullptr) {
		ui->buttonRightToLeft->isChecked() ? this->jump(m_ksz->pages()) : this->jump(1);
	}
}

void MainWindow::onPreviousPage()
{
	if (m_ksz != nullptr) {
		ui->buttonRightToLeft->isChecked() ? this->next() : this->previous();
	}
}

void MainWindow::onNextPage()
{
	if (m_ksz != nullptr) {
		ui->buttonRightToLeft->isChecked() ? this->previous() : this->next();
	}
}

void MainWindow::onLastPage()
{
	if (m_ksz != nullptr) {
		ui->buttonRightToLeft->isChecked() ? this->jump(1) : this->jump(m_ksz->pages());
	}
}

void MainWindow::onToggleStatusBar()
{
	ui->statusBar->setVisible(!ui->statusBar->isVisible());
}

void MainWindow::onFontSliderValueChanged(int value)
{
	if (m_ksz == nullptr) {
		return;
	}

	QToolTip::showText(m_fontSlider->mapToGlobal(QPoint(0, -50)), "Font size: " + QString::number(m_fontSlider->value()));

	this->refreshNotes();
}
