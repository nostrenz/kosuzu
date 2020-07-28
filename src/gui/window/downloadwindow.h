#ifndef DOWNLOADWINDOW_H
#define DOWNLOADWINDOW_H

#pragma once

#include <src/download/downloader.h>
#include <src/gui/window/mainwindow.h>
#include <QMainWindow>
#include <QPushButton>

class MainWindow;

namespace Ui {
	class DownloadWindow;
}

class DownloadWindow : public QMainWindow
{
	Q_OBJECT

	public:
		explicit DownloadWindow(QWidget *parent = nullptr);
		~DownloadWindow();
		void setMainWindow(MainWindow* mainWindow);
		void centerParent();
		bool addQuery(DownloadQuery* query);
		bool isDownloading() const;

	private:
		Ui::DownloadWindow *ui;
		MainWindow* m_mainWindow;
		Downloader* m_downloader;
		int m_rightClickedDownloadRow = -1;
		bool m_initialized = false;
		void restoreDownloads();
		void updateDownloadRow(DownloadQuery*);
		void updateDownloadRows();
		void showMesage(QString message);
		void resizeSections();
		bool addDownload(QString text, bool inBatch=false);
		void setIcon(QPushButton* button, QString icon);
		void resizeEvent(QResizeEvent*);
		bool eventFilter(QObject *obj, QEvent *event);
		void closeEvent (QCloseEvent*);

	private slots:
		void onAddDownload();
		void onDownloadAdded(DownloadQuery*);
		void onStartDownloads();
		void onPauseDownloads();
		void onStopDownloads();
		void onPostDownloadProgress(unsigned int page, qint64 recieved, qint64 total);
		void onPostDownloaded(DownloadQuery*);
		void onClearDownloads();
		void onDownloadTableItemCellDoubleClicked(int, int);
		void onDownloadTableContextMenuRequested(const QPoint&);
		void onCopyDownloadTitle();
		void onOpenDownloadUrl();
		void onOpenDownloadFolder();
		void onStartDownload();
		void onPauseDownload();
		void onStopDownload();
		void onRemoveDownload();
		void onPasteUrl();
		void onStatusMessage(QString);
};

#endif // DOWNLOADWINDOW_H
