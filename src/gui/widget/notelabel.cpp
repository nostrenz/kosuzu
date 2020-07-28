#include <src/gui/widget/notelabel.h>
#include <QPainter>
#include <QToolTip>

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
	this->parseBody();
}

NoteLabel::~NoteLabel()
{
	m_note = NULL;
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
QString NoteLabel::body(float scaleRatio, bool isEmbeded) const
{
	int fontSize = qRound(14.0f * m_fontScale);

	if (scaleRatio != 1) {
		fontSize = qRound((float)fontSize * scaleRatio);
	}

	QString style = "font-size:" + QString::number(fontSize) + "px;";

	// No pre-set style for this note body
	if (!m_note->body().startsWith('<')) {
		style += " padding:5px;";

		if (isEmbeded) {
			style += " color:black; background-color:rgb(255,255,255);";
		}
	}

	return "<div style=\"" + style + "\">" + m_note->body() + "</div>";
}

float NoteLabel::fontScale() const
{
	return m_fontScale;
}

void NoteLabel::renderVertically()
{
	// Only use this if words are too big to fit vertically in the bubble
	if (this->width() > 50) {
		return;
	}

	QString text = this->text();
	QString temp = "";

	for (int i = 0; i < text.size(); i++) {
		temp += text[i] + "\n";
	}

	this->setText(temp);
}

void NoteLabel::setBackgroundStyle(BackgroundStyle bgStyle, float scaleRatio)
{
	m_backgroundStyle = bgStyle;

	if (bgStyle == BackgroundStyle::Danbooru) {
		this->setStyleSheet("QLabel {color: black; background-color: rgba(255,255,238,50%); border: 1px solid #000; padding: 5px;}");
		this->clear(); // Remove text
	} else if (bgStyle == BackgroundStyle::Embeded) {
		this->setStyleSheet("QLabel {color: black; background-color: " + (m_backgroundColor.isNull() ? "rgb(255,255,255)" : m_backgroundColor) + "; border: none; padding: 0px; margin: 0px; border-radius: 10px;}");
		this->setText(this->body(scaleRatio, true));
	}
}

///
/// Private method
///

void NoteLabel::parseBody()
{
	QString text = m_note->body();

	text.replace("\n", "<br>");
	text.replace("<tn>", "<p style=\"color: #8C8C8C; font-size: 12px;\">");
	text.replace("</tn>", "</p>");

	m_note->setBody(text);

	if (!text.startsWith('<')) {
		return;
	}

	// Get stylsheet
	int startIndex = text.indexOf("style=\"");

	if (startIndex == -1) {
		return;
	}

	startIndex += 7;
	int endIndex = text.indexOf('"', startIndex);

	if (endIndex == -1) {
		return;
	}

	QString style = text.mid(startIndex, endIndex-startIndex);
	QString editedStyle = style;

	if (!editedStyle.endsWith(';')) {
		editedStyle += ';';
	}

	int fontSizeIndex = editedStyle.indexOf("font-size:");
	int backgroundColorIndex = editedStyle.indexOf("background-color:");
	int backgroundIndex = editedStyle.indexOf("background:");

	// Qt doesn't understand font-sizes using the '%' unit so we need to convert then to use 'px'
	if (fontSizeIndex != -1) {
		fontSizeIndex += 10;
		QString fontSize = editedStyle.mid(fontSizeIndex, editedStyle.indexOf(';', fontSizeIndex)-fontSizeIndex);
		QString newFontSize = fontSize;

		if (newFontSize.endsWith('%')) {
			newFontSize.remove('%');
		}

		int value = newFontSize.toInt();

		if (value > 0) {
			m_fontScale = (float)value / 100;
		}

		// Remove font-size rule from style
		editedStyle.replace("font-size:" + fontSize + ";", "");

		text.replace(style, editedStyle.trimmed());
		text.replace(" style=\"\"", "");

		m_note->setBody(text);
	}

	if (backgroundColorIndex != -1) {
		backgroundColorIndex += 17;
		m_backgroundColor = style.mid(backgroundColorIndex, style.indexOf(';', backgroundColorIndex)-backgroundColorIndex);
	} else if (backgroundIndex != -1) {
		backgroundIndex += 11;
		m_backgroundColor = style.mid(backgroundIndex, style.indexOf(';', backgroundIndex)-backgroundIndex);
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
