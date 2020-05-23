#pragma execution_character_set("utf-8")
#include "ResourceManager.h"
#include <iostream>
#include <random>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <QTextStream>
#include <QString>

using namespace vf;
using namespace std;
const QString ResourceManager::str_catagory[] = { tr("圖像採集"), tr("尺寸檢測"), tr("外觀檢測"), tr("輸出設置"), tr("模板匹配") };
const QString ResourceManager::icon_idx[] = { "camera","cut","lana","verify","app" };

FunctionData::FunctionData(int c, QString n) :functionCatagory(c), functionName(n) {}
SerializeData::SerializeData():runStatus(0),runResult("NaN"){}

FunctionSerialize::FunctionSerialize(const FunctionSerialize& other):fd(other.fd),
sd(other.sd), used(other.used) {
	++*used;
}
FunctionSerialize::FunctionSerialize(int c, QString n):fd(new FunctionData(c,n)),sd(new SerializeData), 
used(new size_t(1)){}
FunctionSerialize& FunctionSerialize::operator=(const FunctionSerialize& rhs) { 
	++*rhs.used;
	this->~FunctionSerialize();
	fd = rhs.fd; 
	sd = rhs.sd;
	used = rhs.used;
	return *this; 
}
FunctionSerialize::~FunctionSerialize() {
	if (!--*used) {
		delete sd;
		delete used;
	}
}
int FunctionSerialize::getCatagory() const { return fd->functionCatagory; }
QString FunctionSerialize::name() const { return fd->functionName; }
//int FunctionSerialize::runStatus()const { return sd->runStatus; }
//QString FunctionSerialize::runResult()const { return sd->runResult; }
//FunctionSerialize FunctionSerialize::clone(){  }
//FunctionSerialize::FunctionSerialize(const FunctionSerialize& other):d(other.d), runStatus(other.runStatus), runResult(other.runResult) {}
//FunctionSerialize::FunctionSerialize(int c, QString n) : runStatus(0), runResult("NaN") { d = new FunctionData(c, n); }

QDataStream& vf::operator << (QDataStream& out, const FunctionSerialize& fs) {
	return out << fs.getCatagory() << fs.name();
}
QDataStream& vf::operator >> (QDataStream& in, FunctionSerialize& fs) {
	int catagory;
	QString name;
	in >> catagory >> name;
	fs = FunctionSerialize(catagory, name);
	return in;
}

void ResourceManager::writeSeries(QString filename) {
	QFile file(filename);
	if (!file.open(QIODevice::WriteOnly)) {
		cerr << "Cannot open file for writing: "
			<< qPrintable(file.errorString()) << endl;
		return;
	}
	QDataStream outStream(&file);
	outStream << this->FunctionList;
	file.close();
}

void ResourceManager::loadSeries(QString filename) {
	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly)) {
		cerr << "Cannot open file for reading: "
			<< qPrintable(file.errorString()) << endl;
		return;
	}
	QDataStream inStream(&file);
	this->FunctionList.clear();
	inStream >> this->FunctionList;
	file.close();
}

QString ResourceManager::getResult() {
	QString returnStr;// (tr("%1,%2,%3,%4").arg());
	foreach(FunctionSerialize serialize, this->FunctionList) {
		returnStr += str_catagory[serialize.getCatagory()];
		returnStr += "," + serialize.name();
		returnStr += tr(",%1,%2\n").arg(to_string(serialize.runStatus()).c_str(), serialize.runResult());
		//QString status(serialize.runStatus);
		//returnStr += "," + QString( to_string(serialize.runStatus).c_str() );
		//returnStr += "," + serialize.runResult + "\n";
	}
	return returnStr;
}

void ResourceManager::logResult(QString filename) {
	QFile file(filename);
	if (!file.open(QIODevice::WriteOnly | QFile::Truncate)) {
		cerr << "Cannot open file for writing: "
			<< qPrintable(file.errorString()) << endl;
		return;
	}
	QTextStream ts(&file);
	ts.setCodec("unicode");
	ts << this->getResult() << endl;
	ts.flush();
	file.close();
}


void ResourceManager::runProcess() {
	std::default_random_engine genrnd(time(nullptr));
	uniform_int_distribution<int> status_rnd(0, 1);
	uniform_real_distribution<double> pos_rnd(0.0, 1000.0);
	uniform_int_distribution<int> pos_length_rnd(1, 3);
	foreach(FunctionSerialize serialize, this->FunctionList) {
		serialize.runStatus() = status_rnd(genrnd);
		if (serialize.runStatus()) {
			double pos(0.0);
			int pos_length(0);
			switch (serialize.getCatagory()) {
			case CAPTURE:
			case OUTPUT:
			default:
				break;
			case SIZE_MEASURE:
			case EXTRIOR_MEASURE:
				pos_length = 0x01 << pos_length_rnd(genrnd);
				serialize.runResult().clear();
				for (int j = 0; j != pos_length; ++j) {
					stringstream ss;
					ss << setprecision(3) << fixed << pos_rnd(genrnd);
					serialize.runResult() += !j ? "" : ",";
					serialize.runResult() += ss.str().c_str();
				}
				break;
			}
		}
	}

	//for (int i = 0; i != FunctionList.count(); ++i) {
	//	FunctionSerialize serialize = FunctionList.at(i);
	//	//serialize.runStatus() = 1;//status_rnd(genrnd);
	//	serialize.setRunStatus(1);
	//	if (serialize.runStatus()) {
	//		double pos(0.0);
	//		int pos_length(0);
	//		switch (serialize.getCatagory()) {
	//		case CAPTURE:
	//		case OUTPUT:
	//		default:
	//			break;
	//		case SIZE_MEASURE:
	//		case EXTRIOR_MEASURE:
	//			pos_length = 0x01 << pos_length_rnd(genrnd);
	//			//serialize.runResult().clear();
	//			QString createStr;
	//			for (int j = 0; j != pos_length; ++j) {
	//				stringstream ss;
	//				ss << setprecision(3) << fixed << pos_rnd(genrnd);
	//				createStr += !j ? "" : ",";
	//				createStr += ss.str().c_str();
	//				//serialize.runResult() += !j ? "" : ",";
	//				//serialize.runResult() += ss.str().c_str();
	//			}
	//			serialize.setRunResult(createStr);
	//			break;
	//		}
	//	}
	//}
}