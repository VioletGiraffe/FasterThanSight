#include "ccolorsdialog.h"
#include "ui_ccolorsdialog.h"

DISABLE_COMPILER_WARNINGS
#include <QApplication>
#include <QColorDialog>
RESTORE_COMPILER_WARNINGS



CColorsDialog::CColorsDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::CColorsDialog)
{
	ui->setupUi(this);

	connect(ui->btnWindowBackgroundColor, &QToolButton::clicked, [this](){
		QColor c = QColorDialog::getColor(Qt::white, this, "Pick a color");
		if (!c.isValid())
			return;

		ui->btnWindowBackgroundColor->setStyleSheet(QString("background-color: %1").arg(c.name()));
	});
}

CColorsDialog::~CColorsDialog()
{
	delete ui;
}

void CColorsDialog::reject()
{
	QDialog::reject();
}

QString CColorsDialog::generateStyleSheet()
{
	return QString();
}

void CColorsDialog::applyStyleSheet()
{
	qApp->setStyleSheet(generateStyleSheet());
}
