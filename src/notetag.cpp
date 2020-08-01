#include <src/notetag.h>
#include <src/utils.h>

/// This is used to parse an HTML tag containing a style attribute and modify this style to be understandable by Qt.

NoteTag::NoteTag(QString html, int startIndex, int endIndex, NoteTag* parent)
{
	m_parent = parent;

	// Extract style attribute
	m_styleStart = html.indexOf("style=\"");

	if (m_styleStart == -1) {
		return;
	}

	m_styleStart += 7;
	m_styleEnd = html.indexOf('"', m_styleStart);

	if (m_styleEnd == -1) {
		return;
	}

	m_style = html.mid(m_styleStart, m_styleEnd-m_styleStart);
	m_styleStart += startIndex;
	m_styleEnd += startIndex;

	// Extract font-size and background-color attributes
	if (!m_style.endsWith(';')) {
		m_style += ';';
	}

	int fontSizeIndex = m_style.indexOf("font-size:");
	int backgroundColorIndex = m_style.indexOf("background-color:");
	int backgroundIndex = m_style.indexOf("background:");

	// Qt doesn't understand font-sizes using the '%' unit so we need to convert then to use 'px'
	if (fontSizeIndex != -1) {
		fontSizeIndex += 10;
		m_fontSize = m_style.mid(fontSizeIndex, m_style.indexOf(';', fontSizeIndex)-fontSizeIndex);
		QString newFontSize = m_fontSize;

		if (newFontSize.endsWith('%')) {
			newFontSize.remove('%');
		}

		int value = newFontSize.toInt();

		if (value > 0) {
			m_fontScale = (float)value / 100;
		}
	}

	if (backgroundColorIndex != -1) {
		backgroundColorIndex += 17;
		m_backgroundColor = m_style.mid(backgroundColorIndex, m_style.indexOf(';', backgroundColorIndex)-backgroundColorIndex);
	} else if (backgroundIndex != -1) {
		backgroundIndex += 11;
		m_backgroundColor = m_style.mid(backgroundIndex, m_style.indexOf(';', backgroundIndex)-backgroundIndex);
	}

	int colorIndex = m_style.indexOf("color:");
	int textShadowIndex = m_style.indexOf("text-shadow:");

	// Qt's stylesheet don't support text-shadow so we'll use its color as the text color instead
	if (textShadowIndex != -1 && colorIndex != -1) {
		textShadowIndex += 12;
		colorIndex += 6;

		QString textShadow = m_style.mid(textShadowIndex, m_style.indexOf(';', textShadowIndex)-textShadowIndex);
		QString color = m_style.mid(colorIndex, m_style.indexOf(';', colorIndex)-colorIndex);
		QString textShadowColor = utils::regex(textShadow, "[a-z]{3,}", 0);

		if (!textShadowColor.isNull()) {
			m_style.replace("color:" + color + ";", "color:" + textShadowColor + ";");
			m_style.replace("text-shadow:" + textShadow + ";" , "");
		}
	}

	m_style = m_style.trimmed();
}

///
/// Public method
///

NoteTag* NoteTag::parent()
{
	return m_parent;
}

QString NoteTag::style()
{
	return m_style;
}

QString NoteTag::backgroundColor() const
{
	return m_backgroundColor;
}

float NoteTag::fontScale() const
{
	return m_fontScale;
}

int NoteTag::styleStart() const
{
	return m_styleStart;
}

int NoteTag::styleEnd() const
{
	return m_styleEnd;
}

QString NoteTag::modifyStyle(int fontSize, float scaleRatio)
{
	m_fontScale = this->fontScaleFromParent(this);

	if (scaleRatio != 1) {
		fontSize *= scaleRatio;
	}

	fontSize = qRound((float)fontSize * m_fontScale);

	if (m_fontScale == 1) {
		return m_style;
	}

	QString style = m_style;
	style.replace("font-size:" + m_fontSize + ";", "font-size:" + QString::number(fontSize) + "px;");

	return style;
}

///
/// Private method
///

/**
 * Get the font scale relative to parents tags.
 */
float NoteTag::fontScaleFromParent(NoteTag* parent, float fontScale)
{
	fontScale *= parent->fontScale();

	if (parent->parent() == nullptr) {
		return fontScale;
	}

	return this->fontScaleFromParent(parent->parent(), fontScale);
}
