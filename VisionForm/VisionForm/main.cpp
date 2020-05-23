//#pragma comment(lib, "../x64/Debug/Shape.lib") ////used CMake building need command it
#include "visionform.h"
#include <QtWidgets/QApplication>

//#pragma  comment(lib, "../x64/Debug/PopWidgets.lib") ////used CMake building need command it
//#include "../PopWidgets/popwidgets.h"
//#include "../PopWidgets/flowwidget.h"
#include <QTableView>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	using namespace popwin;
	/*SampleTable model;
	TableViewer tableView;
	tableView.setModel(&model);
	tableView.resizeColumnsToContents();
	SpinBoxDelegate deleget(&model);
	tableView.setItemDelegate(&deleget);
	tableView.show();*/
	//CalibrationForm cf;
	//cf.show();
	//ResultPreviewForm rf;
	//rf.show();
	//FlowWidget fw;
	//fw.show();
	//ToolForm cf;
	//cf.show();
	

	VisionForm w;
	w.show();
	return a.exec();
}
