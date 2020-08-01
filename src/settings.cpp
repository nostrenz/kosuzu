#include <src/settings.h>
#include <QFile>
#include <QTextStream>

const QString SETTINGS_INI = "settings.ini";

Settings::Settings()
{
}

///
/// Public method
///

bool Settings::notes() const
{
	return m_notes;
}

bool Settings::fit() const
{
	return m_fit;
}

bool Settings::rightToLeft() const
{
	return m_rightToLeft;
}

QString Settings::lastKsz() const
{
 return m_lastKsz;
}

unsigned int Settings::lastPage() const
{
	return m_lastPage;
}

char Settings::fontSize() const
{
	return m_fontSize;
}

void Settings::setNotes(bool value)
{
	m_notes = value;
}

void Settings::setFit(bool value)
{
	m_fit = value;
}

void Settings::setRightToLeft(bool value)
{
	m_rightToLeft = value;
}

void Settings::setLastKsz(QString value)
{
	m_lastKsz = value;
}

void Settings::setLastPage(unsigned int value)
{
	m_lastPage = value;
}

void Settings::setFontSize(char value)
{
	m_fontSize = value;
}

void Settings::read()
{
	QFile file(APP_DIR + '/' + SETTINGS_INI);

	if (!file.open(QIODevice::ReadOnly)) {
		return;
	}

	QStringList lines;
	QTextStream stream(&file);
	QString line;

	do {
		line = stream.readLine();
		int separatorIndex = line.indexOf('=');

		if (separatorIndex < 1) {
			continue;
		}

		QString key = line.mid(0, separatorIndex);
		QString value = line.mid(separatorIndex + 1).trimmed();

		if (key == "notes") {
			m_notes = (value == "true");
		} else if (key == "fit") {
			m_fit = (value == "true");
		} else if (key == "rightToLeft") {
			m_rightToLeft = (value == "true");
		} else if (key == "lastKsz") {
			m_lastKsz = value;
		} else if (key == "lastPage") {
			m_lastPage = value.toUInt();
		} else if (key == "fontSize") {
			m_fontSize = value.toInt();
		}
	} while (!line.isNull());

	file.close();
}

void Settings::write()
{
	QFile file(APP_DIR + '/' + SETTINGS_INI);

	if (!file.open(QIODevice::WriteOnly)) {
		return;
	}

	QTextStream stream(&file);
	stream.seek(0);

	stream << "notes=" << (m_notes ? "true" : "false") << "\n";
	stream << "fit=" << (m_fit ? "true" : "false") << "\n";
	stream << "rightToLeft=" << (m_rightToLeft ? "true" : "false") << "\n";
	stream << "lastKsz=" << m_lastKsz << "\n";
	stream << "lastPage=" << m_lastPage << "\n";
	stream << "fontSize=" << (int)m_fontSize << "\n";

	file.close();
}
