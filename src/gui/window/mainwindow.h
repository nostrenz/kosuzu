#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#pragma once

#include <src/data/post.h>
#include <src/gui/widget/notelabel.h>
#include <src/gui/widget/tooltiplabel.h>
#include <src/ksz/kszreader.h>
#include <src/collection.h>
#include <src/gui/window/collectionwindow.h>
#include <src/gui/window/downloadwindow.h>
#include <QMainWindow>
#include <QListWidgetItem>
#include <QVector>
#include <QCloseEvent>
#include <QPushButton>

class CollectionWindow;
class DownloadWindow;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
	Q_OBJECT

	public:
		MainWindow(QWidget *parent = nullptr);
		~MainWindow();
		bool openKsz(QString filePath, unsigned int page=1);
		bool openKsz(Ksz* ksz);
		void collectionClosed();
		void downloaderClosed();

	private:
		Ui::MainWindow *ui;
		CollectionWindow* m_collectionWindow = nullptr;
		DownloadWindow* m_downloadWindow = nullptr;
		int m_imageWidth;
		int m_imageHeight;
		NoteLabel::BackgroundStyle m_currentNoteStyle = NoteLabel::BackgroundStyle::Danbooru;
		TooltipLabel* m_tooltipLabel = nullptr;
		KszReader* m_kszReader = nullptr;
		bool m_initialized = false;
		Ksz* m_ksz = nullptr;
		Post* m_post = nullptr;
		float m_scaleRatio = 1;
		QString m_fontFamily = "CC Wild Words";
		void addNote(Note* note);
		void clearNotes();
		void next();
		void previous();
		void jump(unsigned int page);
		void loadImage();
		int calcTopLeftX();
		int calcTopLeftY();
		void calcScaleRatio();
		void switchNotesStyle();
		void scaleImage();
		void refreshNotes();
		void refreshImage();
		void fixWidgetSizes();
		void showCollection();
		void showDownloader();
		void setIcon(QPushButton* button, QString icon);
		void toggleButtonStyle(QPushButton* button);
		void sizeButton(QPushButton* button);
		void unloadKsz();
		bool eventFilter(QObject *obj, QEvent *event);
		void showMesage(QString message);
		void resizeEvent(QResizeEvent*);
		void wheelEvent(QWheelEvent *event);
		void closeEvent (QCloseEvent*);
		void dragEnterEvent(QDragEnterEvent *e);
		void dropEvent(QDropEvent* event);

	public slots:
		void receivedMessage(int instanceId, QByteArray message);

	private slots:
		void onOpenKsz();
		void onToggleCollection();
		void onToggleDownloader();
		void showTooltip(NoteLabel*);
		void hideTooltip();
		void switchNotesStyleToDanbooru();
		void switchNotesStyleToEmbeded();
		void onScrollAreaContextMenu(const QPoint &pos);
		void onToggleNoteStyle();
		void onFitButton();
		void onFitAction();
		void onJumpToPage();
		void onToggleNotes();
		void onToggleFullscreen();
		void onRightToLeftButton();
		void onRightToLeftAction();
		void onOpenContainingFolder();
		void onOpenPoolUrl();
		void onOpenPostUrl();
		void onCheckForNewPosts();
		void onAboutKosuzu();
		void onFirstPage();
		void onPreviousPage();
		void onNextPage();
		void onLastPage();
		void onAboutQt();
		void onOpenProgramFolder();
		void onOpenDownloadsFolder();
		void onGitHubRepository();

	private slots:

};
#endif // MAINWINDOW_H
