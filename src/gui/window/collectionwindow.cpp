#include <ui_collectionwindow.h>
#include <src/gui/window/collectionwindow.h>
#include <src/gui/widget/collectionitem.h>
#include <src/utils.h>
#include <src/serializer.h>
#include <QKeyEvent>
#include <QMimeData>
#include <QDesktopServices>
#include <QMenu>
#include <QMessageBox>

///
/// Constant
///

const QString COLLECTION_JSON = "collection.json";

CollectionWindow::CollectionWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::CollectionWindow)
{
	setAttribute(Qt::WA_DeleteOnClose);

	ui->setupUi(this);

	this->setWindowFlags(Qt::Tool);
	this->installEventFilter(this);

	ui->listKsz->setContextMenuPolicy(Qt::CustomContextMenu);

	connect(ui->listKsz, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(onKszListItemClicked(QListWidgetItem*)));
	connect(ui->listKsz, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(onKszListItemDoubleClicked(QListWidgetItem*)));
	connect(ui->listKsz, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onKszListShowContextMenu(QPoint)));
	connect(ui->lineFilter, SIGNAL(textChanged(QString)), this, SLOT(onFilterTextChanged(QString)));

	this->loadKszs();
}

CollectionWindow::~CollectionWindow()
{
	qDeleteAll(m_kszs);
	m_kszs.clear();

	delete ui;
}

///
/// Public method
///

void CollectionWindow::setMainWindow(MainWindow* mainWindow)
{
	m_mainWindow = mainWindow;
}

void CollectionWindow::centerParent()
{
	this->move(parentWidget()->window()->frameGeometry().topLeft() + parentWidget()->window()->rect().center() - rect().center());
}

///
/// Private method
///

void CollectionWindow::loadKszs()
{
	Collection collection;
	m_kszs = collection.load();

	foreach (Ksz* ksz, m_kszs) {
		this->addKsz(ksz);
	}

	this->updateItemsCounter();
}

void CollectionWindow::dragEnterEvent(QDragEnterEvent *e)
{
	if (e->mimeData()->hasUrls()) {
		e->acceptProposedAction();
	}
}

void CollectionWindow::dropEvent(QDropEvent* event)
{
	const QMimeData* mimeData = event->mimeData();

	if (!mimeData->hasUrls()) {
		return;
	}

	ui->statusbar->showMessage("Adding files...");

	Collection collection;
	QList<QUrl> urlList = mimeData->urls();

	for (int i = 0; i < urlList.size(); i++) {
		bool isNew;
		Ksz* ksz = collection.save(urlList.at(i).toLocalFile(), isNew);

		if (isNew && ksz != nullptr) {
			this->addKsz(ksz);
		}
	}

	this->updateItemsCounter();
}

void CollectionWindow::closeEvent (QCloseEvent*)
{
	m_mainWindow->collectionClosed();
}

///
/// Private slot
///

void CollectionWindow::onKszListItemClicked(QListWidgetItem *item)
{
	CollectionItem* collectionItem = static_cast<CollectionItem*>(item);

	this->openKsz(collectionItem->ksz());
}

void CollectionWindow::onKszListItemDoubleClicked(QListWidgetItem *item)
{
	this->close();
}

void CollectionWindow::openKsz(Ksz* ksz)
{
	if (ksz == nullptr) {
		return;
	}

	/// File doesn't exists anymore, remove it
	if (!QFile::exists(ksz->path())) {
		QMessageBox::information(this, "Kosuzu", "The selected file doesn't exists anymore and was removed from the collection.", QMessageBox::Ok);

		this->removeKsz(ksz);

		return;
	}

	m_mainWindow->openKsz(ksz->path(), ksz->page());
}

void CollectionWindow::updateItemsCounter()
{
	ui->statusbar->showMessage(QString::number(ui->listKsz->count()) + " files");
}

///
/// Private slot
///

void CollectionWindow::onKszListShowContextMenu(const QPoint &pos)
{
	if (ui->listKsz->count() < 1) {
		return;
	}

	Ksz* pool = this->selectedKsz();

	if (pool == nullptr) {
		return;
	}

	QPoint globalPos = ui->listKsz->mapToGlobal(pos);
	QMenu menu;

	menu.addAction(utils::icon("folder"), "Open folder", this, SLOT(onOpenContainingFolder()));
	menu.addAction(utils::icon("url"), "Open URL", this, SLOT(onOpenPoolUrl()));
	menu.addSeparator();
	menu.addAction(utils::icon("remove"), "Remove", this, SLOT(onRemoveItem()));

	menu.exec(globalPos);
}

void CollectionWindow::onOpenContainingFolder()
{
	Ksz* ksz = this->selectedKsz();

	if (ksz != nullptr) {
		utils::openContainingFolder(ksz->path());
	}
}

void CollectionWindow::onOpenPoolUrl()
{
	Ksz* ksz = this->selectedKsz();

	if (ksz != nullptr) {
		utils::openUrl(ksz);
	}
}

Ksz* CollectionWindow::selectedKsz() const
{
	QListWidgetItem* item = ui->listKsz->currentItem();

	if (!item) {
		return nullptr;
	}

	CollectionItem* collectionItem = static_cast<CollectionItem*>(item);

	return collectionItem->ksz();
}

void CollectionWindow::addKsz(Ksz* ksz)
{
	CollectionItem* item = new CollectionItem;
	item->setText(ksz->title() + " (" + QString::number(ksz->page()) + "/" + QString::number(ksz->pages()) + ")");
	item->setKsz(ksz);

	ui->listKsz->addItem(item);
}

void CollectionWindow::removeKsz(Ksz* ksz)
{
	Collection collection;
	collection.remove(ksz);

	m_kszs.removeAll(ksz);

	ui->listKsz->takeItem(ui->listKsz->currentRow());

	delete ksz;
}

/**
 * Return true to prevent an event to be forwarded or false to let it be forwarded.
 *
 * @param obj
 * @param event
 * @return
 */
bool CollectionWindow::eventFilter(QObject *obj, QEvent *event)
{
	// Don't block non-key press event (like the window resize event)
	if (event->type() != QEvent::KeyPress) {
		return false;
	}

	QKeyEvent* key = static_cast<QKeyEvent*>(event);

	if (key->key() == /*Qt::Key_Escape*/16777216) {
		m_mainWindow->collectionClosed();

		return true;
	}

	if (key->key() == /*Qt::Key_Return*/16777220 && obj == ui->listKsz) {
		Ksz* ksz = this->selectedKsz();

		if (ksz != nullptr) {
			this->openKsz(ksz);
		}

		return true;
	}

	// Let the event be forwarded
	return false;
}

void CollectionWindow::onRemoveItem()
{
	this->removeKsz(this->selectedKsz());
}

void CollectionWindow::onFilterTextChanged(QString text)
{
	text = text.toLower();

	ui->listKsz->clear();

	foreach (Ksz* ksz, m_kszs) {
		if (ksz->title().toLower().contains(text)) {
			this->addKsz(ksz);
		}
	}

	this->updateItemsCounter();
}
