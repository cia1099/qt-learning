#pragma once
//#include "popwidgets_global.h"
#include <QPointF>
#include <QVector>
#include <QImage>
#include <vector>

namespace popwin {
	template<typename T> class /*POPWIDGETS_EXPORT*/ Matqt_ {
		T* _data;
		std::size_t* use;
		int _rows, _cols;
	public:
		Matqt_<T>(int = 1, int = 1);
		Matqt_<T>(const Matqt_&);
		Matqt_<T>& operator=(const Matqt_&);
		T& operator[](int);
		~Matqt_<T>();
		T* const& data;
		void writeTo(T*);
		void writeFrom(T const*);
		const int &rows, &cols;
	};
	///*POPWIDGETS_EXPORT*/ Matqt_<float> toHomogeneous2D(const QVector<QPointF>&);
	//POPWIDGETS_EXPORT QPointF metrologyCircle(const QImage&, const std::vector<QPointF>&, )
}//popwin
Q_DECLARE_METATYPE(popwin::Matqt_<float>)

template<typename T> popwin::Matqt_<T>::Matqt_(int row, int col) :
	rows(_rows), cols(_cols), data(_data), use(new std::size_t(1)) {
	_rows = row;
	_cols = col;
	_data = new T[row*col];
	memset(_data, 0, sizeof(T)*rows*cols);
}
template<typename T> popwin::Matqt_<T>::Matqt_(const Matqt_<T>& rhs) :
	data(_data), use(rhs.use), rows(_rows), cols(_cols) {
	_rows = rhs._rows;
	_cols = rhs._cols;
	_data = rhs._data;
	++*this->use;
}
template<typename T>
popwin::Matqt_<T>& popwin::Matqt_<T>::operator=(const Matqt_<T>& rhs) {
	++*rhs.use;
	this->~Matqt_<T>();
	_data = rhs._data;
	use = rhs.use;
	_rows = rhs._rows;
	_cols = rhs._cols;
	return *this;
}
template<typename T>
T& popwin::Matqt_<T>::operator[](int i) {
	assert(0 <= i && i < rows*cols);
	return _data[i];
}
template<typename T>
popwin::Matqt_<T>::~Matqt_() { if (--*use == 0) { delete[] _data; delete use; } }
template<typename T>
void popwin::Matqt_<T>::writeTo(T* dst) { memcpy(dst, _data, sizeof(T)*rows*cols); }
template<typename T>
void popwin::Matqt_<T>::writeFrom(T const* src) { memcpy(_data, src, sizeof(T)*rows*cols); }
