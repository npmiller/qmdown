#include "qmdown.h"

#include "ui_qmdown.h"

#include <QFileDialog>
#include <QUrl>
#include <QFile>
#include <QProcess>
#include <QDesktopServices>
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

	QString render = QString::fromUtf8(pandoc.readAllStandardOutput());
	ui->webEngineView->page()->setHtml(templ.arg(stylesheet, render),
	                                   QUrl("file://" + file));
	ui->plainTextEdit->setPlainText(render);
}

void QMdown::fileChanged(const QString& path) {
	if (fsw->files().size() == 0) {
		QFile f(path);

		// terrible workaround for some editors that delete files to replace
		// them with a copy when saving, this will hang the application
		// if the file was actually deleted
		while (!f.exists());

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
	if (url.scheme() == QString("qrc"))
		return true;
	QDesktopServices::openUrl(url);
	return false;
}
