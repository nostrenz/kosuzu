#include <src/gui/widget/notelabel.h>
#include <src/utils.h>

///
/// Constructor
///

NoteLabel::NoteLabel(Note* note, QString fontFamily)
{
	m_note = note;

	QFont font(fontFamily, 10, QFont::Normal);
	font.setStyleStrategy(QFont::PreferAntialias);

	this->setWordWrap(true);
	this->setAlignment(Qt::AlignCenter);
	this->setTextFormat(Qt::TextFormat::RichText);
	this->setFont(font);

	if (!m_note->body().startsWith('<')) {
		return;
	}

	// Parse HTML structure
	int startIndex = m_note->body().indexOf('<');
	NoteTag* parentTag = nullptr;

	while (startIndex != -1) {
		int endIndex = m_note->body().indexOf(">", startIndex);

		if (endIndex < 1) {
			break;
		}

		endIndex += 1;
		QString tagText = m_note->body().mid(startIndex, endIndex-startIndex);

		if (tagText.contains("style=\"")) {
			NoteTag* noteTag = new NoteTag(tagText, startIndex, endIndex, parentTag);

			if (noteTag->styleStart() > 0 && noteTag->styleEnd() > 0) {
				m_tags.append(noteTag);
				parentTag = noteTag;
				m_backgroundColor = noteTag->backgroundColor();
			} else {
				delete noteTag;
			}
		}

		startIndex = m_note->body().indexOf('<', endIndex);

		if (m_note->body().at(startIndex + 1) == '/') {
			parentTag = nullptr;
		}
	}
}

NoteLabel::~NoteLabel()
{
	m_note = NULL;
	qDeleteAll(m_tags);
}

///
/// Public method
///

Note* NoteLabel::note()
{
	return m_note;
}

unsigned int NoteLabel::offsetX() const
{
	return m_note->x();
}

unsigned int NoteLabel::offsetY() const
{
	return m_note->y();
}

/**
 * For some reason using a QStyleSheet to set the text color only works if text is in a HTML tag.
 */
QString NoteLabel::body(int fontSize, float scaleRatio) const
{
	QString body = m_note->body();

	if (!m_tags.isEmpty()) {
		int lengthDiff = 0;

		foreach (NoteTag* tag, m_tags) {
			int styleStart = tag->styleStart() - lengthDiff;
			int styleEnd = (tag->styleEnd()-tag->styleStart()) - (lengthDiff > 0 ? lengthDiff-1 : 0);

			QString originalText = body;
			QString originalStyle = body.mid(styleStart, styleEnd);

			body.replace(styleStart, styleEnd, tag->modifyStyle(fontSize, scaleRatio));

			lengthDiff += (originalText.length() - body.length());
		}
	} else {
		fontSize = qRound((float)fontSize * scaleRatio);

		body = "<div style=\"font-size: " + QString::number(fontSize) + "px;\">" + body + "</div>";
	}

	body.replace("\n", "<br>");
	body.replace("<tn>", "<p style=\"color: #8C8C8C; font-size: 12px;\">");
	body.replace("</tn>", "</p>");

	return body;
}

void NoteLabel::setBackgroundStyle(BackgroundStyle bgStyle, int fontSize, float scaleRatio)
{
	m_backgroundStyle = bgStyle;

	if (bgStyle == BackgroundStyle::Danbooru) {
		this->setStyleSheet("QLabel {color: black; background-color: rgba(255,255,238,50%); border: 1px solid #000; padding: 5px;}");
		this->clear(); // Remove text
	} else if (bgStyle == BackgroundStyle::Embeded) {
		this->setStyleSheet("QLabel {color: black; background-color: " + (m_backgroundColor.isNull() ? "rgb(255,255,255)" : m_backgroundColor) + "; border: none; padding: 0px; margin: 0px; border-radius: 10px;}");
		this->setText(this->body(fontSize, scaleRatio));
	}
}

///
/// Private slot
///

void NoteLabel::enterEvent(QEvent* event)
{
	emit showTooltip(this);
}

void NoteLabel::leaveEvent(QEvent*)
{
	emit hideTooltip();
}
