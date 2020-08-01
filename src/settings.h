#ifndef SETTINGS_H
#define SETTINGS_H

#include <src/main.h>
#include <QString>

class Settings
{
	public:
		Settings();
		bool notes() const;
		bool fit() const;
		bool rightToLeft() const;
		QString lastKsz() const;
		unsigned int lastPage() const;
		char fontSize() const;
		void setNotes(bool);
		void setFit(bool);
		void setRightToLeft(bool);
		void setLastKsz(QString);
		void setLastPage(unsigned int);
		void setFontSize(char);
		void read();
		void write();

	private:
		bool m_notes = true;
		bool m_fit = true;
		bool m_rightToLeft = false;
		QString m_lastKsz;
		unsigned int m_lastPage;
		char m_fontSize = FONT_SIZE;
};

#endif // SETTINGS_H
