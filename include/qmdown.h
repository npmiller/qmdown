#ifndef QMDOWN_H
#define QMDOWN_H

#include <QDebug>
#include <QDialog>
#include <QFileSystemWatcher>
#include <QMainWindow>
#include <QProcess>
#include <QString>
#include <QWebEnginePage>

namespace Ui {
class qmdown;
}

class QMdown : public QMainWindow {
	Q_OBJECT

      public:
	explicit QMdown(QString file, QWidget *parent = 0);
	~QMdown();

	void render(const QString &file);

      public slots:
	void actionOpen();
	/* void actionRefresh(); */
	void fileChanged(const QString &path);

      private:
	Ui::qmdown *ui;
	QFileSystemWatcher *fsw;
	QString templ;
	QString stylesheet;
};

class PreviewPage : public QWebEnginePage {
	Q_OBJECT
      public:
	explicit PreviewPage(QObject *parent = nullptr)
	    : QWebEnginePage(parent) {}

      protected:
	bool acceptNavigationRequest(const QUrl &url, NavigationType type,
	                             bool isMainFrame);
};

#endif // QMDOWN_H
