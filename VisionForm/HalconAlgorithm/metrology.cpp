#include "metrology.h"
#include <exception>
using namespace mvTec;
using namespace HalconCpp;

AbstractProcess::AbstractProcess(int c, QString n):functionCatagory(c),functionName(n){}
AbstractProcess& AbstractProcess::operator=(const AbstractProcess& rhs) {
	functionName = rhs.functionName;
	functionDescribe = rhs.functionDescribe;
	functionCatagory = rhs.functionCatagory;
	return *this;
}
MetrologyParam::MetrologyParam() :my_shape(0),
measure_lenght1(20), measure_length2(5), measure_sigma(1.0), measure_threshold(30.0) {}

char const* Metrology::ShapeList[] = { "circle\0", "ellipse\0", "line\0", "rectangle2\0" };
Metrology::Metrology(int c, QString n):AbstractProcess(c,n),my_param(new MetrologyParam){
	functionDescribe = "Metrology";
}
Metrology::Metrology(const Metrology& rhs) : AbstractProcess(rhs.functionCatagory,rhs.functionName), 
my_param(rhs.my_param), metrologyHandle(rhs.metrologyHandle) {
	functionDescribe = rhs.functionDescribe;
}
Metrology& Metrology::operator=(const Metrology& rhs){
	if (this != &rhs) {
		AbstractProcess::operator=(rhs);
		my_param = rhs.my_param;
		metrologyHandle = rhs.metrologyHandle;
	}
	return *this;
}
bool Metrology::createMetrologyHandle(HObject image, MetrologyParam* param) {
	if (!image.IsInitialized())
		return false;
	HTuple img_row, img_col;
	GetImageSize(image, &img_row, &img_col);
	try {
		if (metrologyHandle.GetMetrologyObjectIndices().Length() && metrologyHandle.IsInitialized())
			metrologyHandle.Clear();
		metrologyHandle.CreateMetrologyModel();
		metrologyHandle.SetMetrologyModelImageSize(img_row, img_col);
		if (param != nullptr)
			my_param.reset(param);
		HTuple shapeParam;
		foreach(QString pos, my_param->draw_path.split(","))
			shapeParam.Append(pos.toDouble());

		HTuple GenParamName, GenParamValue;
		QJsonObject GenParam = my_param->GenParam;
		foreach(QString key, GenParam.keys()) {
			//ref. set_metrology_object_param in Halcon reference
			GenParamName.Append(key.toStdString().c_str());
			if (key == "measure_interpolation" || key == "measure_transition" ||
				key == "instances_outside_measure_regions")
				GenParamValue.Append(GenParam[key].toString().toStdString().c_str());
			else
				GenParamValue.Append(GenParam[key].toDouble());
		}

		metrologyHandle.AddMetrologyObjectGeneric(ShapeList[my_param->my_shape], shapeParam,
			my_param->measure_lenght1, my_param->measure_length2, my_param->measure_sigma,
			my_param->measure_threshold, GenParamName, GenParamValue);

	}
	catch (std::exception e) {
		//log error here
		return false;
	}
	return true;
}

QString Metrology::run(HObject img) { 
	QString resultObject;
	if (createMetrologyHandle(img)) {
		metrologyHandle.ApplyMetrologyModel(img);
		resultObject = this->getObjectResult();
	}
	else
		resultObject = "error";
	return resultObject;
}

QString Metrology::getObjectResult(bool isEdge) {
	QString resultObject;
	if (!isEdge) {
		HTuple h_result = metrologyHandle.GetMetrologyObjectResult("all", "all", "result_type", "all_param");
		resultObject = h_result.ToString();
	}
	else {
		HTuple used_rows, used_cols;
		used_rows = metrologyHandle.GetMetrologyObjectResult("all", "all", "used_edges", "row");
		used_cols = metrologyHandle.GetMetrologyObjectResult("all", "all", "used_edges", "column");
		HTuple used_edges;
		for (Hlong i = 0; i != used_rows.Length(); ++i) {
			used_edges.Append(HTuple(used_rows[i].D(), used_cols[i].D()));
		}
		resultObject = used_edges.ToString();
	}

	return resultObject;
}


