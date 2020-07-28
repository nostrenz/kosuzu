#include <ui_downloadwindow.h>
#include <src/main.h>
#include <src/utils.h>
#include <src/gui/window/downloadwindow.h>
#include <src/serializer.h>
#include <src/collection.h>
#include <QMessageBox>
#include <QProgressBar>
#include <QLabel>
#include <QDesktopServices>
#include <QClipboard>
#include <QCloseEvent>
#include <QMenu>

///
/// Constant
///

const QString DOWNLOADS_JSON = "downloads.json";

DownloadWindow::DownloadWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::DownloadWindow)
{
	setAttribute(Qt::WA_DeleteOnClose);

	ui->setupUi(this);

	this->setWindowFlags(Qt::Dialog);
	this->installEventFilter(this);

	ui->tableDownload->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
	ui->tableDownload->setContextMenuPolicy(Qt::CustomContextMenu);

	this->setIcon(ui->buttonAdd, "add");
	this->setIcon(ui->buttonStart, "start");
	this->setIcon(ui->buttonPause, "pause");
	this->setIcon(ui->buttonStop, "stop");
	this->setIcon(ui->buttonClear, "clear");

	m_downloader = new Downloader();

	connect(ui->lineUrl, SIGNAL(returnPressed()), this, SLOT(onAddDownload()));
	connect(ui->buttonAdd, SIGNAL(clicked()), this, SLOT(onAddDownload()));
	connect(ui->buttonStart, SIGNAL(clicked()), this, SLOT(onStartDownloads()));
	connect(ui->buttonPause, SIGNAL(clicked()), this, SLOT(onPauseDownloads()));
	connect(ui->buttonStop, SIGNAL(clicked()), this, SLOT(onStopDownloads()));
	connect(ui->buttonClear, SIGNAL(clicked()), this, SLOT(onClearDownloads()));
	connect(ui->tableDownload, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(onDownloadTableItemCellDoubleClicked(int,int)));
	connect(ui->tableDownload, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onDownloadTableContextMenuRequested(QPoint)));
	connect(m_downloader, SIGNAL(downloadAdded(DownloadQuery*)), this, SLOT(onDownloadAdded(DownloadQuery*)));
	connect(m_downloader, SIGNAL(postDownloaded(DownloadQuery*)), this, SLOT(onPostDownloaded(DownloadQuery*)));
	connect(m_downloader, SIGNAL(postDownloadProgress(unsigned int, qint64, qint64)), this, SLOT(onPostDownloadProgress(unsigned int, qint64, qint64)));
	connect(m_downloader, SIGNAL(statusMessage(QString)), this, SLOT(onStatusMessage(QString)));

	this->restoreDownloads();
	this->resizeSections();
}

DownloadWindow::~DownloadWindow()
{
	delete m_downloader;
	delete ui;
}

///
/// Public method
///

void DownloadWindow::setMainWindow(MainWindow* mainWindow)
{
	m_mainWindow = mainWindow;
}

void DownloadWindow::centerParent()
{
	this->move(parentWidget()->window()->frameGeometry().topLeft() + parentWidget()->window()->rect().center() - rect().center());
}

bool DownloadWindow::addQuery(DownloadQuery* query)
{
	if (m_downloader->hasQuery(query)) {
		return false;
	}

	return m_downloader->add(query);
}

bool DownloadWindow::isDownloading() const
{
	return m_downloader->isRunning();
}

///
/// Private method
///

void DownloadWindow::restoreDownloads()
{
	QFile file(APP_DIR + '/' + DOWNLOADS_JSON);

	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return;
	}

	QByteArray data = file.readAll();
	file.close();

	Serializer serializer;
	QVector<DownloadQuery*> queries = serializer.downloadQueries(data);

	foreach (DownloadQuery* query, queries) {
		// Temporary file not found, reset query
		if (query->downloadedPosts() > 0) {
			if (query->kszPath().isNull() || !QFile::exists(query->kszPath())) {
				query->reset();
			}
		} else if (!query->kszPath().isNull() && !QFile::exists(query->kszPath())) {
			query->reset();
		}

		m_downloader->addReady(query);
	}
}

void DownloadWindow::showMesage(QString message)
{
	QMessageBox::information(this, "Kosuzu", message, QMessageBox::Ok);
}

void DownloadWindow::setIcon(QPushButton* button, QString icon)
{
	button->setIcon(utils::icon(icon));
	button->setIconSize(QSize(13, 13));
}

bool DownloadWindow::addDownload(QString text, bool inBatch)
{
	DanbooruUrl* url = new DanbooruUrl(text);

	if (!url->isValid()) {
		if (!inBatch) {
			QMessageBox messageBox;
			messageBox.setText(text);
			messageBox.setInformativeText("Unable to obtain a valid pool or post ID.");
			messageBox.exec();
		}

		return false;
	}

	if (url->isAmbiguous()) {
		QMessageBox msgBox;
		msgBox.setText(text);
		msgBox.setInformativeText("Do you want to download a single post or an entire pool?");
		msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Yes);
		msgBox.setIcon(QMessageBox::Question);

		auto buttonPool = msgBox.button(QMessageBox::Yes);
		buttonPool->setText("Pool");

		auto buttonPost = msgBox.button(QMessageBox::No);
		buttonPost->setText("Post");

		msgBox.exec();

		if (msgBox.clickedButton() == buttonPool) {
			url->keepPoolId();
		} else if (msgBox.clickedButton() == buttonPost) {
			url->keepPostId();
		} else {
			return false;
		}
	}

	// Check if added pool ID is already in the collection or in the download queue
	if (m_downloader->hasQueryWithUrl(url)) {
		QMessageBox msgBox;
		msgBox.setText(text);
		msgBox.setInformativeText("This pool ID or post ID seems to already be in the download list.");
		msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		msgBox.setDefaultButton(QMessageBox::Yes);
		msgBox.setIcon(QMessageBox::Question);

		auto buttonAdd = msgBox.button(QMessageBox::Yes);
		buttonAdd->setText("Add it anyway");

		auto buttonForget = msgBox.button(QMessageBox::No);
		buttonForget->setText("Forget it");

		msgBox.exec();

		if (msgBox.clickedButton() == buttonForget) {
			return false;
		}
	}

	ui->statusbar->showMessage("Adding " + url->url() + "...");

	bool added = m_downloader->add(url);

	if (!added) {
		ui->statusbar->showMessage("Failed to call Danbooru API");
	}

	return added;
}

///
/// Slot
///

void DownloadWindow::onAddDownload()
{
	if (ui->lineUrl->text().isEmpty()) {
		return;
	}

	bool added = this->addDownload(ui->lineUrl->text());

	if (added) {
		ui->lineUrl->clear();
	}
}

void DownloadWindow::onDownloadAdded(DownloadQuery* query)
{
	QLabel* titleLabel = new QLabel();
	QLabel* pagesLabel = new QLabel();
	QLabel* statusLabel = new QLabel();
	QProgressBar* progressBar = new QProgressBar();
	int rowCount = ui->tableDownload->rowCount();

	titleLabel->setText(query->title());
	titleLabel->setAlignment(Qt::AlignCenter);
	titleLabel->setStyleSheet("background-color: transparent;");

	pagesLabel->setText(QString::number(query->downloadedPosts()) + "/" + QString::number(query->postCount()));
	pagesLabel->setAlignment(Qt::AlignCenter);
	titleLabel->setStyleSheet("background-color: transparent;");

	statusLabel->setText("Not started");
	statusLabel->setAlignment(Qt::AlignCenter);
	titleLabel->setStyleSheet("background-color: transparent;");

	progressBar->setRange(0, query->postCount());
	progressBar->setValue(query->downloadedPosts());
	progressBar->setTextVisible(false);

	ui->tableDownload->insertRow(rowCount);
	ui->tableDownload->setCellWidget(rowCount, 0, titleLabel);
	ui->tableDownload->setCellWidget(rowCount, 1, pagesLabel);
	ui->tableDownload->setCellWidget(rowCount, 2, statusLabel);
	ui->tableDownload->setCellWidget(rowCount, 3, progressBar);

	ui->statusbar->showMessage("Download added: '" + query->title() + "'");
}

void DownloadWindow::onStartDownloads()
{
	bool started = m_downloader->start();

	if (started) {
		this->updateDownloadRows();
	}
}

void DownloadWindow::onPauseDownloads()
{
	m_downloader->pause();
	this->updateDownloadRows();
}

void DownloadWindow::onStopDownloads()
{
	m_downloader->stop();
	this->updateDownloadRows();

	ui->buttonStart->setText("Start");
}

void DownloadWindow::updateDownloadRow(DownloadQuery* query)
{
	int row = m_downloader->indexOfQuery(query);

	QWidget* pagesWidget = ui->tableDownload->cellWidget(row, 1);
	QLabel* pagesLabel = qobject_cast<QLabel*>(pagesWidget);

	QWidget* statusWidget = ui->tableDownload->cellWidget(row, 2);
	QLabel* statusLabel = qobject_cast<QLabel*>(statusWidget);

	QWidget* progressWidget = ui->tableDownload->cellWidget(row, 3);
	QProgressBar* progressBar = qobject_cast<QProgressBar*>(progressWidget);

	pagesLabel->setText(QString::number(query->downloadedPosts()) + "/" + (query->postCount() > 0 ? QString::number(query->postCount()) : "?"));

	progressBar->setRange(0, query->postCount());
	progressBar->setValue(query->downloadedPosts());

	if (query->isCompleted()) {
		statusLabel->setText("Completed");
	} else if (query->isRunning()) {
		statusLabel->setText("Downloading");
	} else {
		int placeInQueue = m_downloader->placeInQueue(query);

		if (placeInQueue > 0) {
			int queueLength = m_downloader->queueLength();

			statusLabel->setText("Queued " + QString::number(placeInQueue+1) + "/" + QString::number(queueLength));
		} else if (query->isPaused()) {
			statusLabel->setText("Paused");
		} else {
			statusLabel->setText("Stopped");
		}
	}
}

/**
 * Update all downloads rows.
 */
void DownloadWindow::updateDownloadRows()
{
	int queryCount = m_downloader->queryCount();

	for (int i = 0; i < queryCount; i++) {
		DownloadQuery* query = m_downloader->queryAt(i);

		this->updateDownloadRow(query);
	}
}

void DownloadWindow::closeEvent(QCloseEvent *event)
{
	if (!m_downloader) {
		m_mainWindow->downloaderClosed();

		return;
	}

	if (m_downloader->isRunning()) {
		// Ask confirmation if there's running downloads
		QMessageBox::StandardButton button = QMessageBox::question(
			this,
			"Kosuzu",
			"You have downloads in progress, do you really whant to quit?",
			QMessageBox::No | QMessageBox::Yes,
			QMessageBox::Yes
		);

		if (button != QMessageBox::Yes) {
			event->ignore();
		} else {
			event->accept();
		}
	}

	QFile file(APP_DIR + '/' + DOWNLOADS_JSON);

	// Nothing to be saved
	if (!m_downloader->hasIncompleteDownloads(false)) {
		if (file.exists()) {
			file.remove();
		}

		m_mainWindow->downloaderClosed();

		return;
	}

	// Save download list content to disk
	Serializer serializer;
	QVector<DownloadQuery*> queries;
	int queryCount = m_downloader->queryCount();

	for (int i = 0; i < queryCount; i++) {
		DownloadQuery* query = m_downloader->queryAt(i);

		if (!query->isCompleted()) {
			queries.append(query);
		}
	}

	if (file.open(QIODevice::WriteOnly)) {
		QTextStream stream(&file);
		stream << serializer.serialize(queries);
		file.close();
	}

	m_mainWindow->downloaderClosed();

	return;
}

void DownloadWindow::resizeSections()
{
	QHeaderView* headerView = ui->tableDownload->horizontalHeader();

	int width = ui->tableDownload->width();
	width = (width - 50 - 150 - 200) - 20;

	headerView->resizeSection(0, width);
	headerView->resizeSection(1, 50);
	headerView->resizeSection(2, 150);
	headerView->resizeSection(3, 200);
}

///
/// Private slot
///

void DownloadWindow::onDownloadTableItemCellDoubleClicked(int row, int column)
{
	DownloadQuery* query = m_downloader->queryAt(row);

	if (query != nullptr) {
		m_mainWindow->openKsz(query->kszPath());
	}
}

void DownloadWindow::onDownloadTableContextMenuRequested(const QPoint& pos)
{
	QPoint globalPos = ui->tableDownload->mapToGlobal(pos);
	QMenu menu;

	menu.addAction(utils::icon("clipboard"), "Paste from clipboard", this, SLOT(onPasteUrl()));

	if (ui->tableDownload->rowCount() < 1) {
		menu.exec(globalPos);

		return;
	}

	m_rightClickedDownloadRow = ui->tableDownload->rowAt(pos.y());

	if (m_rightClickedDownloadRow < 0) {
		menu.exec(globalPos);

		return;
	}

	DownloadQuery* query = m_downloader->queryAt(m_rightClickedDownloadRow);

	menu.addSeparator();
	menu.addAction(utils::icon("text"), "Copy title", this, SLOT(onCopyDownloadTitle()));
	menu.addAction(utils::icon("url"), "Open URL", this, SLOT(onOpenDownloadUrl()));
	menu.addAction(utils::icon("folder"), "Open folder", this, SLOT(onOpenDownloadFolder()));
	menu.addSeparator();

	bool queryIsQueued = m_downloader->isQueued(query);

	if (!query->isRunning() && !query->isCompleted()) {
		menu.addAction(utils::icon("start"), (query->isPaused() ? "Resume" : "Start"), this, SLOT(onStartDownload()));
	}

	if (!query->isPaused() && query->isRunning() && !query->isCompleted() && queryIsQueued) {
		menu.addAction(utils::icon("pause"), "Pause", this, SLOT(onPauseDownload()));
	}

	if (!query->isCompleted() && queryIsQueued) {
		menu.addAction(utils::icon("stop"), "Stop", this, SLOT(onStopDownload()));
	}

	menu.addAction(utils::icon("remove"), "Remove", this, SLOT(onRemoveDownload()));

	menu.exec(globalPos);
}

void DownloadWindow::onPostDownloaded(DownloadQuery* query)
{
	this->updateDownloadRow(query);
}

void DownloadWindow::onPostDownloadProgress(unsigned int page, qint64 recieved, qint64 total)
{
	float fRecieved = (float)recieved;
	float fTotal = (float)total;

	float percent = (fRecieved / fTotal) * 100.0f;

	fRecieved /= 1000;
	fTotal /= 1000;

	QString message = "Downloading page "
	+ QString::number(page) + ": "
	+ QString::number(fRecieved, 'f', 0) + " KB /"
	+ QString::number(fTotal, 'f', 0) + " KB ("
	+ QString::number(percent, 'f', 0) + "%)";

	if (m_downloader->currentQuery()->isPaused()) {
		message += " - will pause after this download";
	}

	ui->statusbar->showMessage(message);
}

/**
 * Clear the download list.
 * Called by clicking on the "Clear" button.
 */
void DownloadWindow::onClearDownloads()
{
	if (m_downloader->hasIncompleteDownloads(true)) {
		QMessageBox::warning(this, "Kosuzu", "Cannot clear the list while there's incomplete downloads.", QMessageBox::Ok);

		return;
	}

	m_downloader->clear();

	ui->tableDownload->setRowCount(0);
	ui->statusbar->clearMessage();
}

void DownloadWindow::resizeEvent(QResizeEvent*)
{
	this->resizeSections();
}

/**
 * Return true to prevent an event to be forwarded or false to let it be forwarded.
 *
 * @param obj
 * @param event
 * @return
 */
bool DownloadWindow::eventFilter(QObject *obj, QEvent *event)
{
	if (!m_initialized) {
		if (ui->tableDownload->width() == 100 && ui->tableDownload->height() == 30) {
			return false;
		}

		this->resizeSections();

		m_initialized = true;
	}

	// Don't block non-key press event (like the window resize event)
	if (event->type() != QEvent::KeyPress) {
		return false;
	}

	QKeyEvent* key = static_cast<QKeyEvent*>(event);

	if (key->key() == /*Qt::Key_Escape*/16777216) {
		this->close();

		return true;
	}

	// Let the event be forwarded
	return false;
}

void DownloadWindow::onCopyDownloadTitle()
{
	DownloadQuery* query = m_downloader->queryAt(m_rightClickedDownloadRow);

	QClipboard* clipboard = QApplication::clipboard();
	clipboard->setText(query->title());

	m_rightClickedDownloadRow = -1;
}

void DownloadWindow::onOpenDownloadUrl()
{
	DownloadQuery* query = m_downloader->queryAt(m_rightClickedDownloadRow);

	QDesktopServices::openUrl(query->url()->url());

	m_rightClickedDownloadRow = -1;
}

void DownloadWindow::onOpenDownloadFolder()
{
	DownloadQuery* query = m_downloader->queryAt(m_rightClickedDownloadRow);
	QString dirPath = utils::dirname(query->kszPath());

	QDesktopServices::openUrl(dirPath);

	m_rightClickedDownloadRow = -1;
}

void DownloadWindow::onStartDownload()
{
	DownloadQuery* query = m_downloader->queryAt(m_rightClickedDownloadRow);

	m_downloader->start(query);
	this->updateDownloadRow(query);

	m_rightClickedDownloadRow = -1;
}

void DownloadWindow::onPauseDownload()
{
	// Cannot pause if nothing is downloading
	if (!m_downloader->isRunning()) {
		this->showMesage("Cannot pause as nothing is being downloaded.");

		m_rightClickedDownloadRow = -1;

		return;
	}

	DownloadQuery* query = m_downloader->queryAt(m_rightClickedDownloadRow);

	m_downloader->pause(query);
	this->updateDownloadRow(query);

	m_rightClickedDownloadRow = -1;
}

void DownloadWindow::onStopDownload()
{
	DownloadQuery* query = m_downloader->queryAt(m_rightClickedDownloadRow);

	m_downloader->stop(query);
	this->updateDownloadRow(query);

	m_rightClickedDownloadRow = -1;

	ui->buttonStart->setText("Start");
}

void DownloadWindow::onRemoveDownload()
{
	DownloadQuery* query = m_downloader->queryAt(m_rightClickedDownloadRow);

	m_downloader->remove(query);
	ui->tableDownload->removeRow(m_rightClickedDownloadRow);

	m_rightClickedDownloadRow = -1;
}

void DownloadWindow::onPasteUrl()
{
	QClipboard* clipboard = QApplication::clipboard();
	QString text = clipboard->text();

	// Add single download
	if (!text.contains("\n")) {
		this->addDownload(text);

		return;
	}

	QString line;
	int index;

	// Add multiple downloads
	do {
		index = text.indexOf("\n");
		line = text.mid(0, index);
		text = text.remove(0, index+1);

		if (!line.isEmpty()) {
			this->addDownload(line, true);
		}
	} while (index > -1);
}

void DownloadWindow::onStatusMessage(QString message)
{
	this->statusBar()->showMessage(message);
}
