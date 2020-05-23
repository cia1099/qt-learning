#pragma once

#ifndef SHAPE_H_
#define SHAPE_H_

#include "shape_global.h" //Qt create libaray
#include "../PopWidgets/generictools.h"
#include <cmath>
#include <vector>
#include <map>
#include <tuple>
#include <QtCore/QtCore>
#include <QtGui>
//class Shape;
namespace shape {
#define DEFAULT_LINE_COLOR QColor(0, 255, 0, 128)
#define DEFAULT_FILL_COLOR QColor(255, 0, 0, 128)
#define DEFAULT_SELECT_LINE_COLOR QColor(255, 255, 255)
#define	DEFAULT_SELECT_FILL_COLOR QColor(0, 128, 255, 155)
#define	DEFAULT_VERTEX_FILL_COLOR QColor(0, 255, 0, 255)
#define	DEFAULT_HVERTEX_FILL_COLOR QColor(255, 0, 0)
	enum SHAPE_TYPE { CIRCLE = 8, LINE,GEN_ARC_WELD,GEN_LINE_WELD };

	class SHAPE_EXPORT AbstractShape {
		friend class Shape;
	public:
		static QColor line_color;
		static QColor fill_color;
		static QColor select_line_color;
		static QColor select_fill_color;
		static QColor vertex_fill_color;
		static QColor hvertex_fill_color;
		static qreal scale;
		static int point_type;
		static int point_size;
		static double break_var;
		//int accept_inliner;
		enum { P_SQUARE = 0, P_ROUND };
		enum { MOVE_VERTEX = 0, NEAR_VERTEX };
		AbstractShape(QColor = DEFAULT_LINE_COLOR, int = 0, int = 0);
		AbstractShape(const AbstractShape&);
		inline void close() { _closed = true; }
		inline void setOpen() { _closed = false; }
		void addPoint(const QPointF&);
		QPointF popPoint();
		inline void insertPoint(int i, QPointF p) { points.insert(points.begin() + i, p); }
		virtual void paint(QPainter*) = 0;
		virtual QPainterPath makePath() = 0;
		void drawVertex(QPainterPath&, int);
		int nearestVertex(QPointF, qreal);
		int nearestEdge(QPointF, qreal);
		inline QRectF boundingRect() { return this->makePath().boundingRect(); }
		inline bool containsPoint(QPointF p) { return this->makePath().contains(p); }
		inline void moveBy(QPointF offset); //{ for (auto p = points.begin(); p != points.end(); ++p) *p += offset; }
		inline void moveVertexBy(int i, QPointF offset) { points[i] += offset; }
		inline void highlightVertex(int idx, int activate) { _highlightIndex = idx; _highlightMode = activate; }
		inline void highlightClear() { _highlightIndex = -1; }
		inline QPointF& operator[](int i) { return points[i]; }
		static QRectF getRectFromLine(QPointF, QPointF);
		static QRectF getCircleRectFromLine(QPair<QPointF, QPointF>);
		static qreal distanceToLine(QPointF, QPair<QPointF, QPointF>);
		virtual QPointF determineRefPos(const QPixmap&) = 0;
		virtual void generateWeldPoints(std::vector<QPointF>&) {}
		QColor _line_color;
		int rulerLength = 20;
		float conf = 0.75;
		int ransec_iter = 100;
		int num_welds = 8;
		int genWeldSize = 32;
	protected:
		//const double break_var = rulerLength*0.9;
		int _label, _shape_type, _flags;
		std::vector<QPointF> points;
		int _highlightIndex;
		int _highlightMode;
		std::map<int, std::tuple<float, int> > _highlightSetting;
		bool _fill, _selected, _closed, _isVisible;
		QPointF _refPoint;
	};
	class SHAPE_EXPORT CirCleShape :public AbstractShape {
		float _radius;
	public:
		CirCleShape(int lb, int flg) :AbstractShape(lb, flg) { this->_shape_type = SHAPE_TYPE::CIRCLE; }
		CirCleShape(QColor c) :AbstractShape(c) { this->_shape_type = SHAPE_TYPE::CIRCLE; }
		QPainterPath makePath() override;
		void paint(QPainter*) override;
		QPointF determineRefPos(const QPixmap&) override;
	};
	class SHAPE_EXPORT LineShape :public AbstractShape {
	public:
		QVector3D _linePresent;
		LineShape(int lb, int flg) : AbstractShape(lb, flg) { this->_shape_type = SHAPE_TYPE::LINE; }
		LineShape(QColor c) :AbstractShape(c) { this->_shape_type = SHAPE_TYPE::LINE; }
		QPainterPath makePath() override;
		void paint(QPainter*) override;
		QPointF determineRefPos(const QPixmap&) override;
	};
	class SHAPE_EXPORT GenArcWelds :public AbstractShape {
	public:
		qreal sweepLength = 240;
		GenArcWelds(QPair<QPointF,QPointF> , QColor);// : AbstractShape(c) { _refPoint = pos; _shape_type = SHAPE_TYPE::GEN_ARC_WELD; }
		GenArcWelds(Shape);
		GenArcWelds(const GenArcWelds&);
		QPainterPath makePath() override;
		void paint(QPainter*) override;
		QPointF determineRefPos(const QPixmap&) override { return _refPoint; }
		void generateWeldPoints(std::vector<QPointF>&)override;
	};
	class SHAPE_EXPORT GenLineWelds :public LineShape {
	public:
		int line_length = 1024;
		GenLineWelds(Shape);
		GenLineWelds(const GenLineWelds&);
		QPainterPath makePath() override;
		void paint(QPainter*) override;
		QPointF determineRefPos(const QPixmap&) override { return _refPoint; }
		void generateWeldPoints(std::vector<QPointF>&)override;
	};

	class SHAPE_EXPORT Shape {
	public:
		Shape() :geometry(nullptr), use(new size_t(1)) {}
		Shape(const Shape& copy) :geometry(copy.geometry), use(copy.use) { ++*this->use; this->shape_type = copy.shape_type; }
		~Shape() { if (--*use == 0) { delete geometry; delete use; geometry = nullptr; } }
		Shape& operator=(const Shape& rhs) { ++*rhs.use; this->~Shape(); geometry = rhs.geometry; this->use = rhs.use; shape_type = rhs.shape_type; return *this; }
		Shape& operator=(const CirCleShape& rhs) { return this->operator=(new CirCleShape(rhs)); }
		Shape& operator=(const LineShape& rhs) { return this->operator=(new LineShape(rhs)); }
		Shape& operator=(const GenArcWelds& rhs){ return this->operator=(new GenArcWelds(rhs)); }
		Shape& operator=(const GenLineWelds& rhs){ return this->operator=(new GenLineWelds(rhs)); }
		bool operator==(const Shape& rhs)const { return *use == *rhs.use && geometry == rhs.geometry; }
		operator bool() { return geometry != nullptr; }
		inline int whatsShape() const { return shape_type; }
		bool& seletcted() { return geometry->_selected; }
		bool& fill() { return geometry->_fill; }
		bool& visible() { return geometry->_isVisible; }
		void paint(QPainter* p) { geometry->paint(p); }
		void highlightClear()const { geometry->highlightClear(); }
		void highlightVertex(int idx, int activate)const { geometry->highlightVertex(idx, activate); }
		void addPoint(const QPointF& p) { geometry->addPoint(p); }
		QRectF boundingRect()const { return geometry->boundingRect(); }
		bool containsPoint(QPointF p)const { return geometry->containsPoint(p); }
		std::vector<QPointF>& points()const { return geometry->points; }
		int nearestVertex(QPointF p, qreal eps) { return geometry->nearestVertex(p, eps); }
		int nearestEdge(QPointF p, qreal eps) { return geometry->nearestEdge(p, eps); }
		void moveBy(QPointF p) { geometry->moveBy(p); }
		void moveVertexBy(int idx, QPointF p) { geometry->moveVertexBy(idx, p); }
		void close() { geometry->close(); }
		QColor drawLineColor()const { return geometry->_line_color; }
		QPointF determineRefPos(const QPixmap& pix) { return geometry->determineRefPos(pix); }
		const QPointF& refPoint() { return geometry->_refPoint; }
		//QVector3D linePresent() { assert(geometry->_shape_type == SHAPE_TYPE::LINE ); return dynamic_cast<LineShape*>(geometry)->_linePresent;}
		QVector3D linePresent() { return geometry->_shape_type == SHAPE_TYPE::LINE ? dynamic_cast<LineShape*>(geometry)->_linePresent : QVector3D(); }

		int& rulerRef() { return geometry->rulerLength; }
		int& iterRef() { return geometry->ransec_iter; }
		float& confRef() { return geometry->conf; }
	private:
		Shape(AbstractShape* ap) :geometry(ap), use(new std::size_t(1)) { this->shape_type = ap->_shape_type; }
		AbstractShape* geometry;
		std::size_t* use;
		int shape_type;
	};

}//namespace shape

namespace popwin {
	SHAPE_EXPORT Matqt_<float> toHomogeneous2D(const QVector<QPointF>&);
}
#endif //SHAPE_H_

