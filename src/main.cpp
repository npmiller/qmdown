#include <qmdown.h>
#include <QApplication>
#include <QCommandLineParser>
#include <QString>

int main(int argc, char *argv[]) {
	QCoreApplication::setApplicationName("QMdown");
	QCoreApplication::setApplicationVersion("0.1");
	QApplication app(argc, argv);

	QCommandLineParser parser;
	parser.setApplicationDescription("Simple web display");
	parser.addHelpOption();
	parser.addVersionOption();
	parser.addPositionalArgument("file", "Markdown file to preview");
	parser.process(app);

	QStringList args(parser.positionalArguments());
	QString file;
	if (args.size() > 0) {
		file = args.at(0);
	}

	QMdown w(file);
	w.show();

	return app.exec();
}
