#include "qmdown.h"

#include "ui_qmdown.h"

#include <QDesktopServices>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>
#include <QThread>
#include <QUrl>
#include <QWebEngineSettings>

QMdown::QMdown(QString file, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::qmdown),
      fsw(new QFileSystemWatcher(parent)), stylesheet("qrc:/markdown.css") {
	ui->setupUi(this);

	ui->webEngineView->setPage(new PreviewPage(this));
	/* ui->webEngineView->setContextMenuPolicy(Qt::NoContextMenu); */

	connect(ui->actionOpen, &QAction::triggered, this, &QMdown::actionOpen);
	/* connect(ui->actionRefresh, &QAction::triggered, this, &QMDown::actionRefresh); */
	connect(fsw, &QFileSystemWatcher::fileChanged, this, &QMdown::fileChanged);

	QFile t(":/template.html");
	t.open(QFile::ReadOnly);
	templ = t.readAll();
	t.close();

	if (!file.isEmpty()) {
		fsw->addPath(file);
		ui->statusBar->showMessage(file);
		render(file);
	}
}

void QMdown::render(const QString &file) {
	QProcess pandoc(this);

	QStringList args;
	args << file;
	pandoc.start("pandoc", args);
	pandoc.waitForFinished();

	if (QProcess::FailedToStart == pandoc.error()) {
		QMessageBox::warning(this, tr("QMdown - Error"),
		                     tr("Couldn't start rendering, please make "
		                        "sure pandoc is installed correctly"));
		return;
	}

	QString render = QString::fromUtf8(pandoc.readAllStandardOutput());
	ui->webEngineView->page()->setHtml(templ.arg(stylesheet, render),
	                                   QUrl("file://" + file));
	ui->plainTextEdit->setPlainText(render);
}

void QMdown::fileChanged(const QString& path) {
	// In some cases editors delete the file and recreate it so we need to
	// wait a little
	if (fsw->files().size() == 0) {
		QFile f(path);

		bool recreated = false;
		for (int i = 0; i < 3; ++i) {
			if (f.exists()) {
				recreated = true;
				break;
			}

			// wait for 200 ms
			QThread::msleep(200);
		}

		if (!recreated) {
			QString error(tr("The file %1 appears to have been "
			                 "deleted, rendering is stopped."));
			QMessageBox::warning(
			    this, tr("QMdown - Error"),
			    error.arg(path));
			return;
		}

		// re-add the file to the watcher
		fsw->addPath(path);
	}

	render(path);
}

void QMdown::actionOpen() {
	QString file = QFileDialog::getOpenFileName(
	    this, "Open File",
	    "Markdown files (*.md);;HTML files (*.html *.htm)");

	QFile f(file);
	if (!f.exists())
		return;

	// clear any watched paths
	fsw->removePaths(fsw->files());

	// watch the new file
	fsw->addPath(file);

	render(file);

	ui->statusBar->showMessage(file);
}

QMdown::~QMdown() {
	delete ui;
	delete fsw;
}

bool PreviewPage::acceptNavigationRequest(
    const QUrl &url, QWebEnginePage::NavigationType /*type*/,
    bool /*isMainFrame*/) {
	qDebug() << url.scheme();
	// Only allow qrc:/index.html.
	if (url.scheme() == QStringLiteral("qrc") ||
	    url.scheme() == QStringLiteral("data"))
		return true;
	QDesktopServices::openUrl(url);
	return false;
}
