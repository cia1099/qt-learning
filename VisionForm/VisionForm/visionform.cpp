#pragma execution_character_set("utf-8")
//#pragma comment(lib, "../x64/Debug/Canvas.lib") //used CMake building need command it
//#define BUILD_STATIC
//#pragma comment(lib, "../x64/Debug/Shape.lib") //used CMake building need command it
//#pragma comment(lib, "../x64/Debug/MySource.lib") //used CMake building need command it
#include "visionform.h"
#include <QListView>
#include <QTreeView>
#include <QListWidget>
#include <QTreeWidget>
#include <QLayout>
#include <QObject>
#include <QString>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QScrollBar>
#include <QCursor>
#include <QPoint>
#include <QtGlobal>
#include <QWidgetAction>
#include <functional>
#include "../Shape/shape.h"
#include "../PopWidgets/popwidgets.h"
#include <QTextBrowser>
#include <QFileDialog>

using namespace std::placeholders;
using namespace shape;
using namespace popwin;
//static initialization
QSize ToolButton::minSize(80, 64);

QSize ToolButton::minimumSizeHint() {
	QSize ms = QToolButton::minimumSizeHint();
	int w1(ms.width()), h1(ms.height());
	int w2(minSize.width()), h2(minSize.height());
	minSize = QSize(qMax(w1, w2), qMax(h1, h2));
	return ToolButton::minSize;
}

ToolBar::ToolBar(QString title) :QToolBar(title) {
	QLayout *layout = this->layout();
	QMargins m(0, 0, 0, 0);
	layout->setSpacing(0);
	layout->setContentsMargins(m);
	this->setContentsMargins(m);
	this->setWindowFlags(this->windowFlags() | Qt::FramelessWindowHint);
}

void ToolBar::addAction(QAction* action) {
	if (std::string(typeid(*action).name()) == "class QWidgetAction")
		return QToolBar::addAction(action);//QToolBar::this->addAction(action);

	ToolButton *btn = new ToolButton();
	btn->setDefaultAction(action);
	btn->setToolButtonStyle(this->toolButtonStyle());
	btn->setFixedSize(btn->minimumSizeHint());
	if (action->menu() != nullptr)
		btn->setPopupMode(QToolButton::ToolButtonPopupMode::InstantPopup);
	this->addWidget(btn);
}

ZoomWidget::ZoomWidget(int value) :QSpinBox() {
	setButtonSymbols(QAbstractSpinBox::NoButtons);
	setRange(1, 500);
	setSuffix(" %");
	setValue(value);
	setToolTip("Zoom Level");
	setStatusTip(this->toolTip());
	setAlignment(Qt::AlignCenter);
}
QSize ZoomWidget::minimumSizeHint() {
	int height = this->QSpinBox::minimumSizeHint().height();
	auto fm = QFontMetrics(this->font());
	int width = fm.width(QString(this->maximum()));
	return QSize(width, height);
}


QAction* newAction(QObject* parent, QString text, QString shortcut = "",
	QString icon = "", QString tip = "", bool checkable = false, bool enabled = true) {
	QAction* a = new QAction(text, parent);
	if (icon != "") {
		auto newIcon = [](QString ic) { return QIcon(":/" + ic); };
		a->setIcon(newIcon(icon));
	}
	if (shortcut != "") {
		a->setShortcut(QKeySequence(shortcut));
	}
	if (tip != "") {
		a->setToolTip(tip);
		a->setStatusTip(tip);
	}
	if (checkable)
		a->setCheckable(true);
	a->setEnabled(enabled);
	return a;
}

void myAddActions(ToolBar* wid, QList<QAction*> actList) {
	for (auto it = actList.begin(); it != actList.end(); ++it) {
		wid->ToolBar::addAction(*it);
	}
	if (actList.size() == 0)
		wid->addSeparator();
}

ToolBar* createToolbar(QMainWindow *wid, QString title, QList<QAction*> acts) {
	ToolBar* tb = new ToolBar(title);
	tb->setObjectName(QString("%1ToolBar").arg(title));
	tb->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	if (acts.size() != 0)
		myAddActions(tb, acts);
	wid->addToolBar(Qt::TopToolBarArea, tb);
	return tb;
}

VisionForm::VisionForm(QWidget *parent)
	: QMainWindow(parent), scrollArea(new QScrollArea), zoomWidget(new ZoomWidget), labelCoordinates(new QLabel("")),
	myResource(new vf::ResourceManager)
{
	ui.setupUi(this);
	this->img.load("../../VisionForm/nonWeld.jpg"); // Assume exe file be at .../VisionForm/build/Debug/
	//this->img.load("D:/Project/Hough/line.bmp");//debug genLine
	canvas = new Canvas(this);
	canvas->loadPixmap(QPixmap::fromImage(this->img));
	this->adjustScale(true);
	this->paintCanvas();

	scrollArea->setWidget(canvas);
	scrollArea->setWidgetResizable(true);
	scrollBars[Qt::Vertical] = scrollArea->verticalScrollBar();
	scrollBars[Qt::Horizontal] = scrollArea->horizontalScrollBar();
	//---need every widget
	for (int i = 0; i != NUM_DOCK; ++i) {
		if (i == FlowTree) {
			QTreeWidget *treeWidget = new QTreeWidget;
			treeWidget->setColumnCount(2);
			QStringList headers;
			headers << "¥\¯à¨ç¼Æ" << "¿é¥Xµ²ªG";
			treeWidget->setHeaderLabels(headers);
			QTreeWidgetItem *grade1 = new QTreeWidgetItem(treeWidget);
			grade1->setText(0, "¶ê´£¨ú");
			QTreeWidgetItem *student = new QTreeWidgetItem(grade1);
			//student->setText(0, "");
			student->setText(1, "455.361, 1224.783 121.432");
			/*student = new QTreeWidgetItem(grade1);
			student->setText(0, "Ä£°åÆ¥Åä");
			student->setText(1, "…¢¿¼ˆA");
			student = new QTreeWidgetItem(grade1);
			student->setText(0, "Éú³Éüc");
			student->setText(1, "");*/
			QTreeWidgetItem *grade2 = new QTreeWidgetItem(treeWidget, grade1);
			grade2->setText(0, "¦ì¸m«×");
			student = new QTreeWidgetItem(grade2);
			student->setText(1, "455.361, 1224.783");
			this->widgets[i] = treeWidget; //in future replace by treeView
		}
		else if (i == DOCK::FlowWidget) {
			this->widgets[i] = new popwin::FlowWidget(this);
		}
		else {
			QWidget *logwidget = new QWidget(this);
			QTextBrowser *logBrowser = new QTextBrowser(logwidget);
			QStringList& mylog = logBrowser->searchPaths();
			mylog << "Welcome General Vision Form." << "Camera initial successfuuly."
				<< "Enjoy it :)";
			logBrowser->setSearchPaths(mylog);
			//----edit log contents
			logBrowser->clear();
			QTextCursor cursor(logBrowser->textCursor());
			cursor.movePosition(QTextCursor::Start);
			QTextCharFormat normalFormat;
			normalFormat.setFontPointSize(12);
			normalFormat.setForeground(Qt::darkGreen);
			foreach(QString log_line, mylog) {
				QString timing = '[' + QTime::currentTime().toString("H:m:s a") + ']';
				normalFormat.setFontWeight(QFont::Bold);
				cursor.insertText(timing, normalFormat);
				normalFormat.setFontWeight(QFont::Thin);
				cursor.insertText(' ' + log_line, normalFormat);
				cursor.insertBlock();
			}

			logwidget->setLayout(new QVBoxLayout);
			logwidget->layout()->addWidget(logBrowser);
			this->widgets[i] = logwidget;
		}
		this->docks[i] = new QDockWidget(this->DockName[i], this);
		this->docks[i]->setWidget(this->widgets[i]);
	}

	this->setCentralWidget(this->scrollArea);
	/*this->addDockWidget(Qt::RightDockWidgetArea, this->docks[DOCK::DrawPoints]);
	this->addDockWidget(Qt::RightDockWidgetArea, this->docks[GeneratePoints]);
	this->addDockWidget(Qt::RightDockWidgetArea, this->docks[ReferencePoints]);*/
	this->addDockWidget(Qt::RightDockWidgetArea, this->docks[DOCK::LogWidget]);
	this->addDockWidget(Qt::LeftDockWidgetArea, this->docks[FlowWidget]);
	this->addDockWidget(Qt::RightDockWidgetArea, this->docks[FlowTree]);
	this->docks[FlowTree]->setFeatures(QDockWidget::DockWidgetFloatable);
	QFlag dockFeature = QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetFloatable;
	for (int i = 0; i != NUM_DOCK; ++i) {
		if (i == DOCK::FlowTree) continue;
		this->docks[i]->setFeatures(this->docks[i]->features() ^ dockFeature);
	}

	//============== Actions ======================
	//all action whose trigger still need to connect slot function
	auto action = std::bind(newAction, this, _1, _2, _3, _4, false, true);
	auto a_open = action(tr("¸ü¤J°t¸m"), "Ctrl+O", "open", tr("openFileDetail"));
	auto a_save = action(tr("«O¦s°t¸m"), "Ctrl+s", "save", tr("save file"));
	auto a_saveAs = action(tr("¥t¦s°t¸m"), "Ctrl+r", "paste", tr("save file as"));
	auto a_calib = action(tr("¬Û¾÷¼Ð©w"), "", "checkerboard", tr("calibration"));
	auto a_capture = action(tr("¹Ï¹³±Ä¶°"), "Ctrl+C", "camera", tr("take picture"));
	auto a_calculate = action(tr("¤Ø¤oÀË´ú"), "Ctrl+t", "cut", tr("measurement tools"));
	auto a_generatePoint = action(tr("¿é¥X³]¸m"), "Ctrl+G", "verify", tr("generate points"));
	auto a_imgProcess = action(tr("¥~Æ[ÀË´ú"), "", "lana", tr("image process"));
	auto a_newFile = action(tr("·s«Ø°t¸m"), "", "new", tr("create new file"));
	auto a_drawobject = action(tr("Ã¸»sROI"), "", "objects", tr("draw a template to correlation"));
	auto a_genCircle = action(tr("Circle"), "C", "gen_circle", tr("draw a circle template"));
	auto a_genLine = action(tr("Line"), "L", "gen_line", tr("draw a line template"));
	auto a_findRef = action(tr("¼ÒªO¤Ç°t"), "", "app", tr("Calculate reference position of selected shape"));
	auto a_weldCircle = action(tr("Arc"), "", "gen_circle", tr("generate a shape of arc to weld"));
	auto a_weldLine = action(tr("Line"), "", "gen_line", tr("generate a shape of line to weld"));
	auto a_resultPreview = action(tr("µ²ªG¹wÄý"), "", "view_tree", tr("preview the results for every templated module"));
	QMenu *draw_menu = new QMenu(this);
	draw_menu->addAction(a_genCircle);
	draw_menu->addAction(a_genLine);
	a_drawobject->setMenu(draw_menu);
	QMenu *weld_menu = new QMenu(this);
	weld_menu->addAction(a_weldCircle);
	weld_menu->addAction(a_weldLine);
	a_generatePoint->setMenu(weld_menu);

	auto a_delete = action(tr("§R°£"), "Delete", "cancel", tr("delShapeDetail"));

	QList<QAction*> toolBarAction{ a_open, a_save,a_saveAs,a_newFile,a_drawobject,a_calib,a_capture,a_generatePoint,a_calculate,a_imgProcess,a_findRef,a_resultPreview,a_delete };
	//======= ui toolbar actions
	auto ui_file = action(tr("ÀÉ®×(&F)"), "", "", "");
	QMenu* file_menu = new QMenu(this);
	file_menu->addActions(QList<QAction*>{a_newFile, a_open, a_save, a_saveAs});
	ui_file->setMenu(file_menu);
	auto ui_camera = action(tr("CCD³]¸m(&C)"), "", "", "");
	auto ui_analysis = action(tr("¤ÀªR(&A)"), "", "", "");
	auto ui_analysis1 = action(tr("Blob"), "", "", "");
	auto ui_analysis2 = action(tr("ª½¤è¹Ï"), "", "", "");
	auto ui_analysis3 = action(tr("¤G­È¤Æ"), "", "", "");
	auto ui_analysis4 = action(tr("ÀW°ì"), "", "", "");
	auto ui_analysis5 = action(tr("¯¾²z"), "", "", "");
	QMenu *analysis_menu = new QMenu(this);
	analysis_menu->addAction(ui_analysis1);
	analysis_menu->addAction(ui_analysis2);
	analysis_menu->addAction(ui_analysis3);
	analysis_menu->addAction(ui_analysis4);
	analysis_menu->addAction(ui_analysis5);
	ui_analysis->setMenu(analysis_menu);
	auto ui_explain = action(tr("»¡©ú(&H)"), "", "", "");
	auto ui_register = action(tr("µn¤J(&I)"), "", "", "");
	auto ui_args = action(tr("¥~±µ°Ñ¼Æ(&T)"), "", "", "");
	QList<QAction*> uiToolActs{ ui_file, ui_camera,ui_analysis, ui_register, ui_args,ui_explain};
	ui.menuBar->addActions(uiToolActs);

	auto zoom = new QWidgetAction(this);
	zoom->setDefaultWidget(this->zoomWidget);
	zoomWidget->setWhatsThis(tr("¿s·ÅˆDÏñ£¬Ö§Ô® %1 ºÍ %2 ×÷„Ó×÷").arg("Ctrl+L?").arg("Ctrl+[+-]"));
	//zoomWidget->setEnabled(false);
	auto a_zoomIn = action(tr("ZoomIn"), "Ctrl++", "zoom-in", tr("zoominDetail"));
	auto a_zoomOut = action(tr("ZoomOut"), "Ctrl+-", "zoom-out", tr("zoomoutDetail"));
	auto a_fitWindow = action(tr("³Ì¾A¤j¤p"), "Ctrl+=", "fit_window", tr("fitWinDetail"));
	toolBarAction.append(a_fitWindow);
	QList<QAction*> zoomActions{ a_zoomIn, a_zoomOut, a_fitWindow };
	actionMap["zoomIn"] = a_zoomIn;
	actionMap["zoomOut"] = a_zoomOut;
	actionMap["fitWindow"] = a_fitWindow;
	this->zoomMode = ZOOM_MODE::MANUAL_ZOOM;


	ui.mainToolBar->addActions(toolBarAction);
	ui.mainToolBar->hide();
	ui.statusBar->addPermanentWidget(this->labelCoordinates);//Display cursor coordinates at the right of status bar
	this->tools = createToolbar(this, "Tools", toolBarAction);

	connect(canvas, &Canvas::zoomRequest, this, &VisionForm::zoomRequest);
	connect(canvas, &Canvas::scrollRequest, this, &VisionForm::scrollRequest);
	connect(canvas, &Canvas::selectionChanged, this, &VisionForm::shapeSelectionChanged);
	connect(canvas, &Canvas::showCoordinate, this, &VisionForm::showCoordinate);
	connect(zoomWidget, static_cast<void(ZoomWidget::*)(int)>(&ZoomWidget::valueChanged), this, &VisionForm::paintCanvas);
	connect(a_fitWindow, &QAction::triggered, [&]() {this->setFitWindow(); });
	connect(a_delete, &QAction::triggered, this, &VisionForm::deleteSelectedShape);
	connect(a_genCircle, &QAction::triggered, [&]() {this->toggleDrawMode(false, SHAPE_TYPE::CIRCLE); });
	connect(a_genLine, &QAction::triggered, [&]() {this->toggleDrawMode(false, SHAPE_TYPE::LINE); });
	connect(a_findRef, &QAction::triggered, this, &VisionForm::calculateRefPoint);
	connect(a_weldCircle, &QAction::triggered, [&]() {this->generateWelds(SHAPE_TYPE::GEN_ARC_WELD); });
	connect(a_weldLine, &QAction::triggered, [&]() {this->generateWelds(SHAPE_TYPE::GEN_LINE_WELD); });
	connect(a_open, &QAction::triggered, [&]() {this->loadFlowConfig(""); });
	connect(a_saveAs, &QAction::triggered, [&]() {this->onSaveAsClicked(""); });
	connect(a_calib, &QAction::triggered, [&]() {this->clicked_toolBarEvent(TB_FORM::CALIBRATION); });
}

void VisionForm::zoomRequest(int delta) {
	auto h_bar = scrollBars[Qt::Horizontal];
	auto v_bar = scrollBars[Qt::Vertical];
	//# get the current maximum, to know the difference after zooming
	int h_bar_max = h_bar->maximum();
	int v_bar_max = v_bar->maximum();
	/*get the cursor position and canvas size
	calculate the desired movement from 0 to 1
	where 0 = move left
	1 = move right
	up and down analogous*/
	QCursor cursor = QCursor();
	QPoint pos = cursor.pos();
	QPoint relative_pos = this->QWidget::mapFromGlobal(pos);
	int cursor_x(relative_pos.x()), cursor_y(relative_pos.y());
	int w(scrollArea->width()), h(scrollArea->height());
	/*the scaling from 0 to 1 has some padding
	you don't have to hit the very leftmost pixel for a maximum-left movement*/
	static const qreal margin(0.1);
	int move_x = qRound((cursor_x - margin*w) / (w - 2 * margin*w));
	int move_y = qRound((cursor_y - margin*h) / (h - 2 * margin*h));
	move_x = qMin(qMax(move_x, 0), 1);
	move_y = qMin(qMax(move_y, 0), 1);
	// zoom in
	int units = qRound((float)delta / (8 * 15));
	static const int scale(10);
	addZoom(scale*units);
	/*# get the difference in scrollbar values
	# this is how far we can move*/
	int d_h_bar_max = h_bar->maximum() - h_bar_max;
	int d_v_bar_max = v_bar->maximum() - v_bar_max;
	//# get the new scrollbar values
	int new_h_bar_value = h_bar->value() + move_x*d_h_bar_max;
	int new_v_bar_value = v_bar->value() + move_y*d_v_bar_max;
	h_bar->setValue(new_h_bar_value);
	v_bar->setValue(new_v_bar_value);
}

void VisionForm::showCoordinate(int x, int y) {
	this->labelCoordinates->setText(QString("X: %1; Y: %2").arg(x).arg(y));
}

inline void VisionForm::addZoom(int increment) { setZoom(this->zoomWidget->value() + increment); }
void VisionForm::setZoom(int value) {
	this->actionMap["fitWindow"]->setCheckable(false);
	this->zoomMode = ZOOM_MODE::MANUAL_ZOOM;
	this->zoomWidget->setValue(value);
}
void VisionForm::setFitWindow(bool value) {
	if (value)
		this->actionMap["fitWindow"]->setCheckable(false);
	this->zoomMode = value ? ZOOM_MODE::FIT_WINDOW : ZOOM_MODE::MANUAL_ZOOM;
	this->adjustScale();
}
void VisionForm::adjustScale(bool initial) {
	qreal value = initial ? scaleFitWindow() : zoomMode ? 1.0 : scaleFitWindow(); //ref line#1070 labelImg.py
	this->zoomWidget->setValue(qRound(100 * value));
}

void VisionForm::scrollRequest(int delta, int orientation) {
	int units = -qRound((float)delta / 8 / 15);
	auto bar = this->scrollBars[orientation];
	bar->setValue(bar->value() + bar->singleStep()*units);
}

qreal VisionForm::scaleFitWindow()const {
	/*Figure out the size of the pixmap in order to fit the main widget.*/
	static const qreal e(2.0); //In order that no scrollbars are generated.
	qreal w1 = this->centralWidget()->width() - e;
	qreal h1 = this->centralWidget()->height() - e;
	qreal a1(w1 / h1);
	//# Calculate a new scale value based on the pixmap's aspect ratio.
	qreal w2 = this->canvas->getPixmap().width() - 0.0;
	qreal h2 = this->canvas->getPixmap().height() - 0.0;
	qreal a2(w2 / h2);
	return a2 >= a1 ? w1 / w2 : h1 / h2;
}

void VisionForm::paintCanvas() {
	assert(!this->img.isNull());
	int zoomWidgetValue = this->zoomWidget->value(); //temporary variable (1, 100)
	this->canvas->setScale(0.01 * zoomWidgetValue);
	this->canvas->adjustSize();
	this->canvas->update();
}

void VisionForm::toggleDrawMode(bool edit, int crtMode) {
	this->canvas->setEditing(edit);
	this->canvas->createMode = crtMode;
}

void VisionForm::shapeSelectionChanged(QVector<Shape> selected_shapes) {
	this->_noSelectionSlot = true;
	foreach(Shape a_shape, this->canvas->selectedShapes)
		a_shape.seletcted() = false;
	this->canvas->selectedShapes = selected_shapes;
	foreach(Shape a_shape, this->canvas->selectedShapes)
		a_shape.seletcted() = true;
	this->_noSelectionSlot = false;
	int n_selected(selected_shapes.size());
}

void VisionForm::deleteSelectedShape() {
	this->canvas->deleteSelected();//return the indices to update model viewer
	this->setDirty();
}
void VisionForm::setDirty() {
	this->dirty = true;
}

void VisionForm::calculateRefPoint() {
	QVector<QPointF> refPoints;//prepare for the future
	foreach(Shape a_shape, this->canvas->selectedShapes)
	/*foreach(QList<Shape>::iterator a_shape, this->canvas->selectedShapes)*/ {
		refPoints.push_back(a_shape.determineRefPos(canvas->getPixmap()));
	}
}
//void VisionForm::setDirty() {
//
//}
//
//void VisionForm::newShape() {
//	this->setDirty();
//}
void VisionForm::generateWelds(int whatsGen) {
	QVector<Shape>::iterator a_shape = canvas->selectedShapes.begin();
	for(;a_shape!= canvas->selectedShapes.end();++a_shape)
	/*foreach(QList<Shape>::iterator a_shape, this->canvas->selectedShapes)*/ {
		if (*a_shape && a_shape->refPoint().isNull() /*|| a_shape->points().empty()*/)
			continue;
		Shape tmp_shape, debug_shape(*a_shape);
		switch (whatsGen) {
		case SHAPE_TYPE::GEN_ARC_WELD:
			tmp_shape = GenArcWelds(*a_shape);
			this->canvas->appendShape(tmp_shape);
			break;
		case SHAPE_TYPE::GEN_LINE_WELD:
			tmp_shape = GenLineWelds(*a_shape);
			this->canvas->appendShape(tmp_shape);
			break;
		default:
			break;
		}
	}//foreach
	this->canvas->deSelectShape();
}

//void VisionForm::openToolsWindow(QString toolName) {
//	const char* sw = toolName.toStdString().c_str();
//	switch(sw)
//	{
//	case "Calibration":
//		popwin::CalibrationWidget* cw = new popwin::CalibrationWidget(this);
//		cw->show();
//		break;
//	default:
//		break;
//	}
//}

void VisionForm::onSaveAsClicked(QString filename) {
	if (filename.isEmpty()) {
		filename = QFileDialog::getSaveFileName(this, tr("Save Configuration as"), "d:", tr("°t¸m¤å¥ó(*.dat)"));
	}

	popwin::FlowWidget* flow_widget = qobject_cast<popwin::FlowWidget*>(this->widgets[DOCK::FlowWidget]);
	QListWidget* flowList = flow_widget->flowList;

	myResource->FunctionList.clear();
	for (int i = 0; i != flow_widget->flowList->count(); ++i) {
		QString userName = flowList->item(i)->text();
		//size_t N = sizeof(vf::ResourceManager::str_catagory)/sizeof(*vf::ResourceManager::str_catagory);
		int j = 0; //find index of FunctionCatagory
		for (; j != vf::NULL_CATAGORY; ++j)
			if (vf::ResourceManager::str_catagory[j] == flowList->item(i)->toolTip())
				break;
		
		myResource->FunctionList.append( vf::FunctionSerialize(static_cast<int>(j), userName) );
	}

	myResource->writeSeries(filename); //write flow configuration
}

void VisionForm::loadFlowConfig(QString filename) {
	if (filename.isEmpty())
		filename = QFileDialog::getOpenFileName(this, tr("Load Configuration"), "d:", tr("°t¸m¤å¥ó(*.dat)"));

	myResource->loadSeries(filename);
	popwin::FlowWidget* flow_widget = qobject_cast<popwin::FlowWidget*>(this->widgets[DOCK::FlowWidget]);
	flow_widget->flowList->clear();
	//QList<vf::FunctionSerialize> funcList = myResource->FunctionList;
	for (int i = 0; i != myResource->FunctionList.count(); ++i) {
		vf::FunctionSerialize serial = myResource->FunctionList.at(i);
		QListWidgetItem* item = new QListWidgetItem(QIcon(":/"+vf::ResourceManager::icon_idx[ serial.getCatagory() ]),
			serial.name(), flow_widget->flowList);
	}
}

void VisionForm::clicked_toolBarEvent(int whichClicked) {
	QDialog *popWidget;
	switch (whichClicked)
	{
	case TB_FORM::CALIBRATION:
		popWidget = new CalibrationForm;		
		break;
	default:
		break;
	}
	popWidget->setModal(true);
	popWidget->exec();
}