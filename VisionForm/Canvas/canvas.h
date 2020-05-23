#pragma once

#include <QtWidgets/QWidget>
#include <QPainter>
#include <QVector>
#include <QList>
#include <QMenu>
#include <QApplication>
#include <QPointer>
#include <QPoint>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <memory>
#include <set>
#include "../Shape/shape.h"

#ifndef BUILD_STATIC
#ifdef CANVAS_DLL_EXPORTS
#define CANVAS_API __declspec(dllexport)
#else
#define CANVAS_API __declspec(dllimport)
#endif
#else
# define CANVAS_API
#endif

class CANVAS_API Canvas : public QWidget
{
	Q_OBJECT
#define CURSOR_DEFAULT Qt::ArrowCursor
#define CURSOR_POINT Qt::PointingHandCursor
#define CURSOR_DRAW Qt::CrossCursor
#define CURSOR_MOVE Qt::ClosedHandCursor
#define CURSOR_GRAB Qt::OpenHandCursor
#define CANVAS_epsilon 11.0
public:
	int createMode;
	QVector<shape::Shape> selectedShapes;
	//QList<shape::Shape> myShapes; //debug Genline
	Canvas(QWidget *parent = Q_NULLPTR);
	inline void setDrawingColor(QColor qColor) { drawingLineColor = qColor; drawingCircleColor = qColor; }
	void loadPixmap(QPixmap);
	QSize minimumSizeHint() const override;
	QSize sizeHint() const override;
	inline const QPixmap& getPixmap() const { return _pixmap; }
	inline void setScale(qreal s) { _scale = s; }
	void setFillDrawing(int v) { this->_fill_drawing = v; }
	inline void setHiding(bool = true);
	void setEditing(bool = true);
	void appendShape(shape::Shape);
	QVector<shape::Shape> deleteSelected();
	void deSelectShape();
	enum { CREATE = 0, EDIT };
protected:
	void overrideCursor(QCursor);
	void restoreCursor() { QApplication::restoreOverrideCursor(); }
	QCursor* currentCursor();
	void resetState();
	void enterEvent(QEvent *event) { this->overrideCursor(this->cursor_state); }
	void leaveEvent(QEvent *event) { this->restoreCursor(); }
	void focusOutEvent(QFocusEvent *event) { this->restoreCursor(); }
	void wheelEvent(QWheelEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void mousePressEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
	void paintEvent(QPaintEvent *event);
	QPointF offsetToCenter();
	QPointF transformPos(QPointF);
	QPointF intersectionPoint(QPointF, QPointF);
#define crBegin static int state=0;switch(state){case 0:
#define crReturn(x) do{ state=__LINE__; return x;case __LINE__:;}while(0)
#define crFinish }
	qreal intersectionEdge(QPoint, QPoint&, const QVector<QPoint>&, int&);
	qreal intersectingEdge(QPoint, QPoint&, int&, const QVector<QPoint>&);
	bool selectedVertex() { return !!hVertex; }
	bool outOfPixmap(QPointF);
	bool isVisible(QList<shape::Shape>::iterator);
	int fillDrawing()const { return this->_fill_drawing; }
	void setShapeVisible(QList<shape::Shape>::iterator, int);
	void unHighlight();
	void selectShapes(QVector<shape::Shape>);
	void selectShapePoint(QPointF, bool);
	void calculateOffsets(shape::Shape, QPointF);
	void boundedShiftShapes(QVector<shape::Shape>); //used to copy shape
	void boundedMoveVertex(QPointF);
	bool boundedMoveShapes(QVector<shape::Shape>, QPointF);
	void boundedShiftShape(QVector<shape::Shape>);
	void finalise();
private:
	QPointF prevPoint, prevMovePoint; //used to move shape, addPointToEdge
	std::pair<QPoint, QPoint> offsets;
	std::pair<QMenu, QMenu> menus;
	qreal _scale;
	QPixmap _pixmap;
	bool _hideBackround, hideBackround;
	bool isDrawing;
	bool verified;
	bool mode, movingShape;
	int _fill_drawing;
	QPainter* myPainter;
	Qt::CursorShape cursor_state;
	QCursor _cursor;
	QColor drawingLineColor, drawingCircleColor, lineColor;
	QList<shape::Shape> myShapes;
	shape::Shape myLine, current, hShape;
	std::shared_ptr<int> hVertex, hEdge; //highlight vertex
	std::set<QList<shape::Shape>::iterator> visible;
signals:
	void zoomRequest(int);
	void scrollRequest(int, int);
	void selectionChanged(QVector<shape::Shape>);
	void shapeMoved();
	//void edgeSelected(bool);
	//void drawingPolygon(bool);
	void newShape();
	void showCoordinate(int, int);
};
