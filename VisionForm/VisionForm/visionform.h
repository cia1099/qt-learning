#pragma once
#include <QtWidgets/QMainWindow>
#include <QScrollArea>
#include <QDockWidget>
#include "ui_visionform.h" //Visual Studio Qt Tools Support which has GeneratedFiles folder
#include <QToolButton>
#include <QSpinBox>
#include <QLabel>
#include "../Canvas/canvas.h"
#include "../PopWidgets/flowwidget.h"
#include "../MySource/ResourceManager.h"


class ToolBar;
class ZoomWidget;
class VisionForm : public QMainWindow
{
	Q_OBJECT
#define NUM_DOCK 3
public:
	VisionForm(QWidget *parent = Q_NULLPTR);
	void setZoom(int);
	void setFitWindow(bool = true);
protected:
	enum DOCK
	{
		LogWidget = 0, FlowTree, FlowWidget
	};
	QString DockName[NUM_DOCK] = { "運行日誌", "輸出日誌", "流程配置" };//{ "LogWidget", "FlowTree", "FlowWidget" };
	enum ZOOM_MODE {
		FIT_WINDOW = 0, MANUAL_ZOOM
	};
	enum TB_FORM {
		CALIBRATION, CAPTURE, COMPUTE, RESULT_PREVIEW, TOOL
	};
	inline void addZoom(int = 10);
	void adjustScale(bool = false);
	qreal scaleFitWindow() const;
	void paintCanvas();
	void zoomRequest(int);
	void scrollRequest(int, int);
	void toggleDrawMode(bool, int=0);
	void shapeSelectionChanged(QVector<shape::Shape>);
	//void newShape();
	void setDirty();
	void deleteSelectedShape();
	inline void setEditMode() { this->toggleDrawMode(true); }
	inline void setCreateMode(int v) { this->toggleDrawMode(false, v); }
	void showCoordinate(int, int);
	void calculateRefPoint();
	void generateWelds(int);
	void openToolsWindow(QString);
	void onSaveAsClicked(QString);
	void loadFlowConfig(QString);
	void clicked_toolBarEvent(int);
private:
	Ui::VisionFormClass ui;
	QScrollArea *scrollArea;
	QMap<uint, QScrollBar*> scrollBars;
	QDockWidget* docks[NUM_DOCK];
	QWidget* widgets[NUM_DOCK];
	//QDockWidget *drawPosDock, *genPosDock, *flowDock, *refPosDock;
	//QWidget *drawPosWid, *genPosWid, *flowWid, *refPosWid;
	QSharedPointer<vf::ResourceManager> myResource;
	ToolBar *tools;
	Canvas* canvas;
	QImage img;
	QLabel *labelCoordinates;
	ZoomWidget* zoomWidget;
	int zoomMode;
	QMap<QString, QAction*> actionMap;
	bool _noSelectionSlot;
	bool dirty;
};

class ToolBar :public QToolBar {
public:
	ToolBar(QString);
	void addAction(QAction*);
};

class ToolButton :public QToolButton {
public:
	explicit ToolButton() {}
	static QSize minSize;
	QSize minimumSizeHint();
};

class ZoomWidget :public QSpinBox {
	Q_OBJECT
public:
	ZoomWidget(int = 100);
	QSize minimumSizeHint();
};