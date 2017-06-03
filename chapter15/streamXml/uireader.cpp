#include <QtGui>
#include <QDebug>

#include "uireader.h"

UiReader::UiReader(QTextEdit *textEdit, QString fileName)
{
	txtEdit = textEdit;
	macroName = fileName.toUpper() + "_H";
	level = 0;
}

bool UiReader::read(QIODevice *device)
{
	setDevice(device);
	while(!atEnd()) {
		readNext();
		
		if(isStartElement()) {
			if(name() == "ui" && attributes().value("version") == "4.0")
				readUi();
			else
				raiseError(QObject::tr("The file is not an Qt 4.x UI file."));
		}
	}		
	return !error();
}

void UiReader::readUi()
{
	txtEdit->clear();
	cursor = txtEdit->textCursor();
	cursor.insertText("// Generated by stream xml parser test.\n\n");
	cursor.insertText("#ifndef " + macroName + "\n");
	cursor.insertText("#define " + macroName + "\n\n");
	
	while(!atEnd()) {
		readNext();
				
		if(isStartElement()) {
			if(name() == "widget")
				readWidget();
			
		}
	}
	cursor.insertText("#endif\n");	
}

/**
 * 读取Widget标签.
 */
void UiReader::readWidget()
{
	classAttr = attributes().value("class").toString();
	nameAttr = attributes().value(QString("name")).toString();
	if(level == 0) {
		cursor.insertText("class " + nameAttr + " : public " + classAttr + "\n");
		cursor.insertText("{\n");
		cursor.insertText("public:\n");
		constructorText = "\n\t" + nameAttr + "() {\n";
		constructorText = "\n\t" + nameAttr + "() {\n";	// 构造函数
	}
	else {
		cursor.insertText("\t" + classAttr + " *" + nameAttr + ";\n");
		constructorText += "\t\t" + nameAttr + " = new " + classAttr + ";\n";		
	}

	++level;
	while(!atEnd()) {
		readNext();

		if(isEndElement() && name() == "widget")
			break;
				
		if(isStartElement()) {
			if(name() == "property") {
		        readProperty();
			}
			else if(name() == "widget") {
				readWidget();
				--level;
			}
		}	
	}
	
	if(level == 1) {
		constructorText += "\t}\n";
		cursor.insertText(constructorText);
		cursor.insertText("};\n");
	}
}

void UiReader::readProperty()
{
	propName = attributes().value(QString("name")).toString();
	while(!atEnd()) {
		readNext();
		
		if(isEndElement() && name() == "property")
			break;
					
		if(isStartElement()) {
			if(name() == "string") {
				readNext();
				if(propName == "windowTitle") {
		 			constructorText += "\t\tsetWindowTitle(tr(\"" + text().toString() + "\"));\n";
				}
				else if(propName == "text") { // QComboBox is incorrect.
		 			constructorText += "\t\t" + nameAttr + "->setText(tr(\"" + text().toString() + "\"));\n";
				}
			}
			else if(name() == "rect") {
				readRect();
			}
		}
	}
}

void UiReader::readRect()
{
	while(!atEnd()) {
		readNext();
		
		if(isEndElement() && name() == "rect")
			break;
			
		if(isStartElement()) {
			if(name() == "x") {
				xTag = readElementText();
			}
			else if(name() == "y") {
				yTag = readElementText();
			}
			else if(name() == "width") {
				wTag = readElementText();
			}
			else if(name() == "height") {
				hTag = readElementText();
			}
		}
	}
	if(level == 1) {
		constructorText += "\t\tresize(" + wTag + ", " + hTag + ");\n";
	}
	else {
		constructorText += "\t\t" + nameAttr + "->setGeometry(QRect(" + xTag + ", " + yTag + ", " + wTag + ", " + hTag + "));\n";
	}		
}
