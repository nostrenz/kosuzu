#ifndef COLLECTIONWINDOW_H
#define COLLECTIONWINDOW_H

#pragma once

#include <QMainWindow>
#include <QString>
#include <QListWidgetItem>
#include <src/gui/window/mainwindow.h>
#include <src/ksz/ksz.h>

class MainWindow;

namespace Ui {
	class CollectionWindow;
}

class CollectionWindow : public QMainWindow
{
	Q_OBJECT

	public:
		explicit CollectionWindow(QWidget *parent = nullptr);
		~CollectionWindow();
		void setMainWindow(MainWindow* mainWindow);
		void centerParent();

	private:
		Ui::CollectionWindow *ui;
		MainWindow* m_mainWindow;
		QVector<Ksz*> m_kszs;
		void loadKszs();
		void openKsz(Ksz*);
		Ksz* selectedKsz() const;
		void addKsz(Ksz*);
		void removeKsz(Ksz*);
		void updateItemsCounter();
		bool eventFilter(QObject *obj, QEvent *event);
		void dragEnterEvent(QDragEnterEvent *e);
		void dropEvent(QDropEvent* event);
		void closeEvent (QCloseEvent*);

	private slots:
		void onKszListItemClicked(QListWidgetItem *item);
		void onKszListItemDoubleClicked(QListWidgetItem *item);
		void onKszListShowContextMenu(const QPoint&);
		void onOpenContainingFolder();
		void onOpenPoolUrl();
		void onRemoveItem();
		void onFilterTextChanged(QString);
};

#endif // COLLECTIONWINDOW_H
