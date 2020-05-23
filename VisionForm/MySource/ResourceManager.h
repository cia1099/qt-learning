#pragma once
#include "mysource_global.h"
#include <QJsonValue>
#include <QJsonObject>
#include <QObject>
#include <QList>
#include <QMap>
#include <QString>
#include <QFile>
#include <QSharedData>
#include <QSharedDataPointer>

namespace vf {
	enum FUNC_CATAGORY{CAPTURE=0, SIZE_MEASURE, EXTRIOR_MEASURE, OUTPUT, TEMPLATE_MATCH, NULL_CATAGORY};

	class MYSOURCE_EXPORT FunctionData : public QSharedData {
	public:
		FunctionData(int = -1, QString = "");
		FunctionData(const FunctionData& o) :functionCatagory(o.functionCatagory),
			functionName(o.functionName) {}
		int functionCatagory;
		QString functionName;
	};
	class MYSOURCE_EXPORT SerializeData{
	public:
		SerializeData();
		int runStatus;
		QString runResult;
	};
	class MYSOURCE_EXPORT FunctionSerialize {
	public:
		FunctionSerialize(int = -1, QString = "");// : runStatus(0), runResult(""){ fd = new FunctionData(c, n); }
		FunctionSerialize(const FunctionSerialize& other); //:fd(other.fd) {}//, runStatus(other.runStatus), runResult(other.runResult) {}
		//FunctionSerialize& operator=(const FunctionSerialize& rhs);// { fd = rhs.fd; sd = rhs.sd; return *this; }
		FunctionSerialize& FunctionSerialize::operator=(const FunctionSerialize& rhs);// { fd = rhs.fd; ; return *this; }
		int getCatagory() const;// { return fd->functionCatagory; }
		QString name() const;// { return fd->functionName; }
		~FunctionSerialize();
		int& runStatus() { return sd->runStatus; }
		QString& runResult() { return sd->runResult; }

	private:
		size_t *used;
		QSharedDataPointer<FunctionData> fd;
		SerializeData *sd;
		//QSharedDataPointer<SerializeData> sd;
	};
	QDataStream& operator<<(QDataStream&, const FunctionSerialize&);
	QDataStream& operator >> (QDataStream&, FunctionSerialize&);

	class MYSOURCE_EXPORT ResourceManager : public QObject {
		/*Q_OBJECT*/
	public:
		static const QString str_catagory[];
		static const QString icon_idx[];
		ResourceManager(){}
		void writeSeries(QString);
		void logResult(QString);
		QString getResult();
		void loadSeries(QString);
		void runProcess();
		
	//private:
		QList<FunctionSerialize> FunctionList;
	};

}//namespace vf
