#ifndef SETTINGS_H
#define SETTINGS_H

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
		void setNotes(bool);
		void setFit(bool);
		void setRightToLeft(bool);
		void setLastKsz(QString);
		void setLastPage(unsigned int);
		void read();
		void write();

	private:
		bool m_notes = true;
		bool m_fit = true;
		bool m_rightToLeft = false;
		QString m_lastKsz;
		unsigned int m_lastPage;
};

#endif // SETTINGS_H
