#include <QtCore/QCoreApplication>
#pragma execution_character_set("utf-8")
#pragma comment(lib, "../x64/Debug/MySource.lib")
#include "../MySource/ResourceManager.h"
#include <QtDebug>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonArray>
#include <json\json.h>

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	////====== check series
	//using namespace vf;
	//QString filepath = "D:/Project/海邊/VisionForm/flowProcess.dat";
	//ResourceManager RM;
	//RM.FunctionList.append(FunctionSerialize(0, "fuck"));
	//RM.FunctionList.append(FunctionSerialize(2, "shit"));
	////RM.writeSeries(filepath);

	//RM.FunctionList.append(FunctionSerialize(5, "holy"));
	//RM.loadSeries(filepath);
	//for (int i = 0; i != RM.FunctionList.count(); ++i) {
	//	qDebug() << "Catagory = " << RM.FunctionList.at(i).getCatagory()
	//		<< "\nFunctionName = " << RM.FunctionList.at(i).name() << "\n";
	//	int lookint = RM.FunctionList.at(i).getCatagory();
	//	QString lookName = RM.FunctionList.at(i).name();
	//}
	//RM.runProcess();
	////wchar_t *wc  = static_cast<wchar_t*>("幹你老師");
	//qDebug() << endl << "===============================\n" << QString(RM.getResult());
	//RM.logResult("D:/Project/海邊/VisionForm/runResult.csv");

	//=====check json with unicode
	QJsonObject root;
	root.insert("encoding", "unicode");
	QJsonArray metrologyArray;
	QJsonObject metrologyObj;
	metrologyObj.insert("roi_x", "55,66,77,88,99");
	metrologyObj.insert("name", "去你媽");
	metrologyObj.insert("isUsedEdge", false);
	metrologyArray.append(metrologyObj);
	root["metrology"] = metrologyArray;

	QJsonDocument jdoc(root);

	QFile ofile("D:/Project/海邊/VisionForm/params.json");
	/*if (!ofile.open(QIODevice::WriteOnly))
		qDebug() << "File open error";
	ofile.write(jdoc.toJson());
	ofile.close();
	qDebug() << "write successful";*/

	//----read json from disk
	if(!ofile.open(QIODevice::ReadOnly))
		qDebug() << "File open error";
	QString contJson = ofile.readAll();
	ofile.close();
	qDebug() << "Read from string: " << contJson << "\n\n";

	jdoc = QJsonDocument::fromJson(contJson.toUtf8());
	QJsonObject readRoot = jdoc.object();
	qDebug() << readRoot;
	QJsonArray readMetrology = readRoot["metrology"].toArray();
	QString funcName = readMetrology[0].toObject()["name"].toString();
	if (funcName == "去你媽")
		funcName += "good\n";
	qDebug() << "my name is " << funcName;

	return a.exec();
}
