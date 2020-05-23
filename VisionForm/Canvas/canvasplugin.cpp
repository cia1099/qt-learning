#include "canvas.h"
#include "canvasplugin.h"

#include <QtCore/QtPlugin>

CanvasPlugin::CanvasPlugin(QObject *parent)
	: QObject(parent)
{
	initialized = false;
}

void CanvasPlugin::initialize(QDesignerFormEditorInterface * /*core*/)
{
	if (initialized)
		return;

	initialized = true;
}

bool CanvasPlugin::isInitialized() const
{
	return initialized;
}

QWidget *CanvasPlugin::createWidget(QWidget *parent)
{
	return new Canvas(parent);
}

QString CanvasPlugin::name() const
{
	return "Canvas";
}

QString CanvasPlugin::group() const
{
	return "My Plugins";
}

QIcon CanvasPlugin::icon() const
{
	return QIcon();
}

QString CanvasPlugin::toolTip() const
{
	return QString();
}

QString CanvasPlugin::whatsThis() const
{
	return QString();
}

bool CanvasPlugin::isContainer() const
{
	return false;
}

QString CanvasPlugin::domXml() const
{
	return "<widget class=\"Canvas\" name=\"canvas\">\n"
		" <property name=\"geometry\">\n"
		"  <rect>\n"
		"   <x>0</x>\n"
		"   <y>0</y>\n"
		"   <width>100</width>\n"
		"   <height>100</height>\n"
		"  </rect>\n"
		" </property>\n"
		"</widget>\n";
}

QString CanvasPlugin::includeFile() const
{
	return "canvas.h";
}
