#include <src/gui/widget/tooltiplabel.h>

TooltipLabel::TooltipLabel()
{
	QString fontFamily = this->font().defaultFamily();
	QFont font(fontFamily, 10, QFont::Normal);
	font.setStyleStrategy(QFont::PreferAntialias);

	this->setFont(font);
	this->setWordWrap(true);
	this->setTextFormat(Qt::TextFormat::RichText);
	this->setContentsMargins(0, 0, 0, 0);
	this->setStyleSheet("QLabel {color: black; background-color: rgb(255,255,238); border: 1px solid #000; margin: 0px; padding: 0px;}");
}

///
/// Public method
///

void TooltipLabel::setAppearance()
{
	if (this->text().contains("background:") || this->text().contains("background-color:")) {
		this->setMargin(0);
		this->setAlignment(Qt::AlignCenter);
	} else {
		this->setMargin(5);
		this->setAlignment(Qt::AlignLeft);
	}
}
