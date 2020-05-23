#pragma once
#include "halconalgorithm_global.h"
#ifndef __APPLE__
#  include "HalconCpp.h"
#  include "HDevThread.h"
#  if defined(__linux__) && !defined(__arm__) && !defined(NO_EXPORT_APP_MAIN)
#    include <X11/Xlib.h>
#  endif
#else
#  ifndef HC_LARGE_IMAGES
#    include <HALCONCpp/HalconCpp.h>
#    include <HALCONCpp/HDevThread.h>
#  else
#    include <HALCONCppxl/HalconCpp.h>
#    include <HALCONCppxl/HDevThread.h>
#  endif
#  include <stdio.h>
#  include <HALCON/HpThread.h>
#  include <CoreFoundation/CFRunLoop.h>
#endif
#include <HXLDCont.h>
#include <HMetrologyModel.h>
#include <QtCore\QObject>
#include <QString>
#include <QJsonObject>
#include <memory>

using HalconCpp::HXLDCont;
using HalconCpp::HMetrologyModel;
using HalconCpp::HObject;

namespace mvTec {
	class HALCONALGORITHM_EXPORT AbstractProcess : public QObject {
		Q_OBJECT
			/*Q_PROPERTY(QString functionName)
			Q_PROPERTY(int functionCatagory)
			Q_PROPERTY(QString functionDescribe)*/
	public:
		AbstractProcess(int, QString);
		AbstractProcess(const AbstractProcess&) = delete;
		AbstractProcess& operator=(const AbstractProcess&);
		QString functionName;
		QString functionDescribe;
		int functionCatagory;
		virtual QString run(HObject) = 0;
	};

	class HALCONALGORITHM_EXPORT MetrologyParam : public QObject {
	public:
		MetrologyParam();
		int my_shape;
		int handle_idx;
		double measure_lenght1;
		double measure_length2;
		double measure_sigma;
		double measure_threshold;
		QString draw_path;
		QJsonObject GenParam;
	};
	class HALCONALGORITHM_EXPORT Metrology: public AbstractProcess {
	public:
		Metrology(int,QString);
		Metrology(const Metrology&);
		Metrology& operator=(const Metrology&);
		//Metrology(const AbstractProcess&);
		enum MerologyShape { Circle, Ellipse, Line, Rectangle2 };
		QString run(HObject) override;
	protected:
		bool createMetrologyHandle(HObject, MetrologyParam* = nullptr);
		QString getObjectResult(bool =false);
		QString getObjectMeasures();
	private:
		/*HXLDCont xld;
		HXLDCont cross;
		HXLDCont xld1;*/
		HMetrologyModel metrologyHandle;
		/*int my_shape;
		ushort measure_lenght1;
		ushort measure_length2;
		float measure_sigma;
		float measure_threshold;
		QJsonObject GenParam;*/
		std::shared_ptr<MetrologyParam> my_param;
		static char const* ShapeList[];
	};
}//namespace mvTec
