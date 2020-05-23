//#pragma comment(lib, "../x64/Debug/Shape.lib") //used CMake building need command it
//#define BUILD_STATIC
#include "canvas.h"
#include <QLabel>
#include <QMainWindow>
#include <algorithm>
//#include "../VisionForm/visionform.h"
using namespace shape;
#ifdef BUILD_STATIC
qreal AbstractShape::scale;
#endif
Canvas::Canvas(QWidget *parent)
	: QWidget(parent), _scale(1.0), _hideBackround(false), cursor_state(CURSOR_DEFAULT), mode(Canvas::EDIT)
{
	drawingLineColor = QColor(0, 0, 255);
	drawingCircleColor = QColor(0, 0, 255);
	lineColor = QColor(0, 0, 255);
	myPainter = new QPainter();
	myLine = LineShape(drawingLineColor);
	this->setMouseTracking(true);
	this->setFocusPolicy(Qt::WheelFocus);
	this->isDrawing = false;
	this->verified = false;
	this->_cursor = CURSOR_DEFAULT;
}

void Canvas::paintEvent(QPaintEvent *event) {
	if (this->_pixmap.isNull())
		return QWidget::paintEvent(event);
	//myPainter->save();
	QPainter* p = this->myPainter;
	p->begin(this);
	p->setRenderHint(QPainter::Antialiasing);
	p->setRenderHint(QPainter::HighQualityAntialiasing);
	p->setRenderHint(QPainter::SmoothPixmapTransform);
	p->scale(this->_scale, this->_scale);
	p->translate(this->offsetToCenter());
	p->drawPixmap(0, 0, this->_pixmap);

	AbstractShape::scale = this->_scale;
	for (int i = 0; i != myShapes.size(); ++i) {
		if (myShapes[i].seletcted() || !this->_hideBackround) {
			myShapes[i].fill() = myShapes[i].seletcted() | myShapes[i] == hShape;
			myShapes[i].paint(p);
		}
	}
	if (this->current) {
		this->current.paint(p);
		this->myLine.paint(p);
	}

	//...rescale shapes...need continue
	this->setAutoFillBackground(true);
	if (this->verified) {
		QPalette pal = this->palette();
		pal.setColor(this->backgroundRole(), QColor(184, 239, 38, 128));
		this->setPalette(pal);
	}
	else {
		QPalette pal = this->palette();
		pal.setColor(this->backgroundRole(), QColor(232, 232, 232, 255));
		this->setPalette(pal);
	}
	p->end();
	//myPainter->restore();
}

QPointF Canvas::offsetToCenter() {
	qreal s = this->_scale;
	QSize area = this->QWidget::size();
	qreal w(this->_pixmap.width()*s), h(this->_pixmap.height()*s);
	qreal aw(area.width()), ah(area.height());
	qreal x = aw > w ? (aw - w) / (2 * s) : 0;
	qreal y = ah > h ? (ah - h) / (2 * s) : 0;
	return QPointF(x, y);
}
QPointF Canvas::transformPos(QPointF point) {
	/*Convert from widget-logical coordinates to painter-logical ones.*/
	return point / this->_scale - this->offsetToCenter();
}
bool Canvas::outOfPixmap(QPointF p) {
	int w(this->_pixmap.width()), h(this->_pixmap.height());
	bool x_dir = 0 <= p.toPoint().x() && p.toPoint().x() <= w - 1;
	bool y_dir = 0 <= p.toPoint().y() && p.toPoint().y() <= w - 1;
	return !(x_dir && y_dir);
}

bool Canvas::isVisible(QList<Shape>::iterator it_shape) {
	return it_shape->visible();//this->visible.find(it_shape) != visible.end();
}
void Canvas::setShapeVisible(QList<Shape>::iterator it_shape, int value) {
	//this->visible[it_shape] = value;
	//this->visible.insert(it_shape);
	it_shape->visible() = value;
	this->repaint();
}
void Canvas::resetState() {
	this->restoreCursor();
	this->_pixmap = QPixmap();
	//self.shapesBackups = []
	this->update();
}
QCursor* Canvas::currentCursor() {
	QCursor *pCursor = QApplication::overrideCursor();
	if (pCursor != nullptr)
		cursor_state = pCursor->shape();
	return pCursor;
}
void Canvas::overrideCursor(QCursor cursor) {
	cursor_state = cursor.shape();
	if (this->currentCursor() == nullptr)
		QApplication::setOverrideCursor(cursor);
	else
	{
		QApplication::changeOverrideCursor(cursor);
	}
}
inline void Canvas::setHiding(bool enable) {
	this->_hideBackround = enable ? this->hideBackround : false;
}

void Canvas::loadPixmap(QPixmap pix) {
	this->_pixmap = pix;
	myShapes.clear();
	//myShapes.shrink_to_fit();
	this->repaint();
}

QSize Canvas::sizeHint() const {
	return this->minimumSizeHint();
}

QSize Canvas::minimumSizeHint()const {
	if (!this->_pixmap.isNull())
		return this->_scale*this->_pixmap.size();
	return this->QWidget::minimumSizeHint();
}

void Canvas::setEditing(bool value) {
	this->mode = value ? EDIT : CREATE;
	if (!value) {
		this->unHighlight();
		this->deSelectShape();
	}
	this->prevPoint = QPointF();
	this->repaint();
}
void Canvas::unHighlight() {
	if (this->hShape)
		this->hShape.highlightClear();
	hShape = Shape();
	hVertex.reset();
}
void Canvas::deSelectShape() {
	if (!this->selectedShapes.empty()) {
		this->setHiding(false);
		emit selectionChanged(QVector<Shape>());
		this->update();
	}
}
QVector<Shape> Canvas::deleteSelected() {
	QVector<Shape> deleted_shape;
	if (!this->selectedShapes.empty()) {
		//QList<Shape>::iterator::difference_type j(0);
		for (int i = 0; i != this->selectedShapes.size(); ++i) {
			auto a_shape = std::find(myShapes.begin(), myShapes.end(), selectedShapes[i]);
			if (a_shape != myShapes.end()) {
				deleted_shape.push_back(*a_shape);
				this->myShapes.erase(a_shape);
			}
		}
		//this->storeShapes();
		this->selectedShapes.clear();
		this->setHiding(false);
		this->update();
	}
	return deleted_shape;
}
void Canvas::selectShapes(QVector<Shape> a_shapes) {
	this->setHiding();
	emit selectionChanged(a_shapes);
	this->update();
}
void Canvas::selectShapePoint(QPointF point, bool multiple_selection_mode) {
	/*Select the first shape created which contains this point.*/
	if (this->selectedVertex())
		this->hShape.highlightVertex(*this->hVertex, AbstractShape::MOVE_VERTEX);
	else {
		for (auto a_shape = this->myShapes.rbegin(); a_shape != myShapes.rend(); ++a_shape) {
			if (this->isVisible(a_shape.base() - 1) && a_shape->containsPoint(point)) {
				this->calculateOffsets(*a_shape, point);
				this->setHiding();
				if (multiple_selection_mode) {
					auto gotIt = std::find(this->selectedShapes.begin(), selectedShapes.end(), *(a_shape.base() - 1));
					if (gotIt == this->selectedShapes.end())
						emit selectionChanged(this->selectedShapes + QVector<Shape>{*(a_shape.base() - 1)});
				}
				else
					emit selectionChanged(QVector<Shape>{*(a_shape.base() - 1)});
				return;
			}
		}
		this->deSelectShape();
	}
}

void Canvas::calculateOffsets(Shape s, QPointF p) {
	QRectF rect = s.boundingRect();
	qreal x1(rect.x() - p.x()), y1(rect.y() - p.y());
	qreal x2(x1 + rect.width() - 1), y2(y1 + rect.height() - 1);
	this->offsets = std::make_pair(QPoint(x1, y1), QPoint(x2, y2));
}

void Canvas::wheelEvent(QWheelEvent *event) {
	QPoint delta = event->angleDelta();
	int h_delta(delta.x()), v_delta(delta.y());
	auto mods = event->modifiers();
	if (Qt::ControlModifier == (int)mods && v_delta)
		emit zoomRequest(v_delta);
	else {
		if (v_delta)
			emit scrollRequest(v_delta, Qt::Vertical);
		if (h_delta)
			emit scrollRequest(h_delta, Qt::Horizontal);
	}
	event->accept();
}

void Canvas::mouseMoveEvent(QMouseEvent *event) {
	QPointF pos = this->transformPos(event->localPos());
	//# Update coordinates in status bar if image is opened
	if (!this->outOfPixmap(pos))
		emit showCoordinate(pos.toPoint().x(), pos.toPoint().y());

	this->prevMovePoint = pos;
	this->restoreCursor();
	// Polygon drawing
	if (this->mode == CREATE) {
		std::vector<QPointF> tmp_pos;
		this->overrideCursor(CURSOR_DRAW);
		if (!this->current)
			return;
		QColor color = this->lineColor;
		if (this->outOfPixmap(pos))
			/*# Don't allow the user to draw outside the pixmap.
			# Project the point to the pixmap's edges.*/
			pos = this->intersectionPoint(current.points().back(), pos);
		if (this->createMode == SHAPE_TYPE::CIRCLE || createMode == SHAPE_TYPE::LINE)
			tmp_pos = std::vector<QPointF>{ current.points()[0], pos };
		switch (this->createMode) {
		case SHAPE_TYPE::CIRCLE:
			this->myLine = CirCleShape(color);
			break;
		case SHAPE_TYPE::LINE:
			this->myLine = LineShape(color);
			break;
		default:
			break;
		}//end switch
		this->myLine.points() = std::move(tmp_pos);
		this->repaint();
		this->current.highlightClear();
		return;
	}
	// Polygon/Vertex moving.
	this->movingShape = false;
	if (Qt::LeftButton & event->buttons()) {
		if (this->selectedVertex()) {
			this->boundedMoveVertex(pos);
			this->repaint();
			this->movingShape = true;
		}
		else if (!this->selectedShapes.empty() && !this->prevPoint.isNull()) {
			this->overrideCursor(CURSOR_MOVE);
			this->boundedMoveShapes(this->selectedShapes, pos);
			this->repaint();
			this->movingShape = true;
		}
		return;
	}
	/*# Just hovering over the canvas, 2 posibilities:
	# - Highlight shapes
	# - Highlight vertex
	# Update shape / vertex fill and tooltip value accordingly.*/
	this->setToolTip("Photo");
	auto a_shape = myShapes.rbegin();
	for (; a_shape != myShapes.rend() && this->isVisible(a_shape.base() - 1); ++a_shape) {
		/*# Look for a nearby vertex to highlight.If that fails,
		# check if we happen to be inside a shape.*/
		int idx = a_shape->nearestVertex(pos, CANVAS_epsilon / this->_scale);
		int idx_edge = a_shape->nearestEdge(pos, CANVAS_epsilon / this->_scale); //different in labelImg
		if (idx >= 0) {
			if (this->selectedVertex())
				this->hShape.highlightClear();
			this->hVertex.reset(new int(idx));
			this->hShape = Shape(*a_shape);//.reset(new Shape(*a_shape));
			this->hEdge.reset(new int(idx_edge));
			a_shape->highlightVertex(idx, AbstractShape::MOVE_VERTEX);
			this->overrideCursor(CURSOR_POINT);
			this->setToolTip("Click & drag to move point");
			this->setStatusTip(this->toolTip());
			this->update();
			break;
		}
		else if (a_shape->containsPoint(pos)) {
			if (this->selectedVertex())
				this->hShape.highlightClear();
			this->hVertex.reset();
			this->hShape = Shape(*a_shape);//.reset(new Shape(*a_shape));
			this->hEdge.reset(new int(idx_edge));
			this->setToolTip("Click & drag to move shape");
			this->setStatusTip(this->toolTip());
			this->overrideCursor(CURSOR_GRAB);
			this->update();
			break;
		}
	}//end for(a_shape)
	if (a_shape == myShapes.rend()) {
		/*# Nothing found, clear highlights, reset state. else cannot understand at 265*/
		if (this->hShape) {
			this->hShape.highlightClear();
			this->update();
		}
		this->hVertex.reset();
		this->hShape = Shape();
		this->hEdge.reset();
	}
	//emit edgeSelected(!this->hEdge);
}
void Canvas::mousePressEvent(QMouseEvent *event) {
	QPointF pos = this->transformPos(event->pos());
	if (event->button() == Qt::LeftButton) {
		if (this->mode == CREATE) {
			if (this->current) {
				switch (this->createMode)
				{
				case SHAPE_TYPE::CIRCLE:
				case SHAPE_TYPE::LINE:
					assert(this->current.points().size() == 1);
					this->current.points() = this->myLine.points();
					this->finalise();
					break;
				default:
					break;
				}
			}
			else if (!this->outOfPixmap(pos)) {
				Shape tmp_shape;
				switch (this->createMode) {
				case SHAPE_TYPE::CIRCLE:
					tmp_shape = CirCleShape(QColor(0,255,0));
					break;
				case SHAPE_TYPE::LINE:
					tmp_shape = LineShape(QColor(0,255,0));
					break;
				default:
					break;
				}
				this->current = tmp_shape;
				this->current.addPoint(pos);

				this->myLine.points() = std::vector<QPointF>(2, pos);
				this->setHiding();
				//emit drawingPolygon(true);
				this->update();
			}
		}//end if(CREATE)
		else {
			bool group_mode = event->modifiers() == Qt::ControlModifier;
			this->selectShapePoint(pos, group_mode);
			this->prevPoint = pos;
			this->repaint();
		}
	}
	else if (event->button() == Qt::RightButton && this->mode == EDIT) {
		bool group_mode = event->modifiers() == Qt::ControlModifier;
		this->selectShapePoint(pos, group_mode);
		this->prevPoint = pos;
		this->repaint();
	}
}
void Canvas::mouseReleaseEvent(QMouseEvent *event) {
	if (event->button() == Qt::RightButton) {

	}
	else if (Qt::LeftButton && !this->selectedShapes.empty())
		this->overrideCursor(CURSOR_GRAB);
	if (this->movingShape)
		emit shapeMoved();
}

QPointF Canvas::intersectionPoint(QPointF p1, QPointF p2) {
	QSize pixSize = this->_pixmap.size();
	const QVector<QPoint> points{ QPoint(0,0),QPoint(pixSize.width() - 1,0),
		QPoint(pixSize.width() - 1,pixSize.height() - 1),
		QPoint(0,pixSize.height() - 1) };
	int x1 = qMin(qMax(p1.toPoint().x(), 0), pixSize.width() - 1);
	int y1 = qMin(qMax(p1.toPoint().y(), 0), pixSize.height() - 1);
	int x2(p2.toPoint().x()), y2(p2.toPoint().y());
	int dst_idx(0);
	QPoint dst_point(x2, y2); //= p2.toPoint();
	qreal dst = intersectingEdge(QPoint(x1, y1), dst_point, dst_idx, points);
	int x3(points[dst_idx].x()), y3(points[dst_idx].y());
	int x4(points[(dst_idx + 1) & 0x03].x()), y4(points[(dst_idx + 1) & 0x03].y());
	if (dst_point == QPoint(x1, y1)) {
		if (x3 == x4)
			return QPointF(x3, qMin(qMax(0, y2), qMax(y3, y4)));
		else
			return QPointF(qMin(qMax(0, x2), qMax(x3, x4)), y3);
	}
	return dst_point;
	/**** too complicated****/
	//std::vector<qreal> list_dist; 
	//std::vector<int> list_idx; 
	//std::vector<QPoint> list_pos;
	//int dst_i(0);
	//for (QPoint dst_point = p2.toPoint(); dst_i < 4; dst_point = p2.toPoint()) {
	//	list_dist.push_back(intersectionEdge(QPoint(x1,y1), dst_point, points, dst_i));
	//	list_idx.push_back(dst_i);
	//	list_pos.push_back(dst_point);
	//}
	//auto smallest_it = std::min_element(list_dist.begin(), list_dist.end());
	//std::ptrdiff_t diff_it = smallest_it - list_dist.begin();
	//if (!list_dist.empty() && diff_it < list_dist.size()) {
	//	int x3(points[diff_it].x()), y3(points[diff_it].y());
	//	int x4(points[(diff_it + 1) & 0x03].x()), y4(points[(diff_it + 1) & 0x03].y());
	//	if (list_pos[diff_it] == QPoint(x1, y1)) {
	//		if (x3 == x4)
	//			return QPointF(x3, qMin(qMax(0, y2), qMax(y3, y4)));
	//		else
	//			return QPointF(qMin(qMax(0, x2), qMax(x3, x4)), y3);
	//	}
	//	return QPointF(list_pos[diff_it]);
	//}
	//return QPointF();
}
qreal Canvas::intersectionEdge(QPoint p1, QPoint& inputOutput_point, const QVector<QPoint>& vp, int& dst_i) {
	static int i;
	int x1, x2, x3, x4, x, y1, y2, y3, y4, y;
	long long denom, nua, nub;
	bool is_ua, is_ub;
	qreal ua, ub, d;
	QPoint m, minus_m;
	x1 = p1.x(); y1 = p1.y();
	x2 = inputOutput_point.x(); y2 = inputOutput_point.y();
	if (dst_i >= 0 && dst_i < 4)
		i = dst_i;
	crBegin;
	for (/*i = 0*/; i != 4; ++i) {
		x3 = vp[i].x(); y3 = vp[i].y();
		x4 = vp[(i + 1) & 0x03].x(); y4 = vp[(i + 1) & 0x03].y();
		denom = (y4 - y3)*(x2 - x1) - (x4 - x3)*(y2 - y1);
		nua = (x4 - x3) * (y1 - y3) - (y4 - y3) * (x1 - x3);
		nub = (x2 - x1) * (y1 - y3) - (y2 - y1) * (x1 - x3);
		if (denom == 0)
			continue;
		ua = nua / denom;
		ub = nub / denom;
		is_ua = 0 <= ua && ua <= 1;
		is_ub = 0 <= ub && ub <= 1;
		if (is_ua && is_ub) {
			x = x1 + ua * (x2 - x1);
			y = y1 + ua * (y2 - y1);
			m.setX((x3 + x4) >> 1); m.setY((y3 + y4) >> 1);
			minus_m.setX(x2); minus_m.setY(y2);
			d = std::sqrt(QPointF::dotProduct(m - minus_m, m - minus_m));
			dst_i = i;
			inputOutput_point = QPoint(x, y);
			crReturn(d);
			crFinish;// end switch(state)
		}
	}
	return 1e10;
}
qreal Canvas::intersectingEdge(QPoint p1, QPoint& inputOutput_point, int& dst_i, const QVector<QPoint>& vp) {
	int x1(p1.x()), y1(p1.y());
	int x2(inputOutput_point.x()), y2(inputOutput_point.y());
	qreal min_d(1e10);
	for (int i = 0; i != vp.size(); ++i) {
		int x3(vp[i].x()), y3(vp[i].y());
		int x4(vp[(i + 1) & 0x03].x()), y4(vp[(i + 1) & 0x03].y());
		long long denom = (y4 - y3)*(x2 - x1) - (x4 - x3)*(y2 - y1);
		long long nua = (x4 - x3) * (y1 - y3) - (y4 - y3) * (x1 - x3);
		long long nub = (x2 - x1) * (y1 - y3) - (y2 - y1) * (x1 - x3);
		if (denom == 0)
			continue;
		qreal ua = nua / denom;
		qreal ub = nub / denom;
		bool is_ua = 0 <= ua && ua <= 1;
		bool is_ub = 0 <= ub && ub <= 1;
		if (is_ua && is_ub) {
			int x = x1 + ua * (x2 - x1);
			int y = y1 + ua * (y2 - y1);
			QPoint m((x3 + x4) >> 1, (y3 + y4) >> 1);
			QPoint minus_m(x2, y2);
			qreal d = std::sqrt(QPointF::dotProduct(m - minus_m, m - minus_m));
			if (d < min_d) {
				min_d = d;
				dst_i = i;
				inputOutput_point = QPoint(x, y);
			}
		}
	}
	return min_d;
}

bool Canvas::boundedMoveShapes(QVector<Shape> a_shapes, QPointF pos) {
	if (this->outOfPixmap(pos))
		return false;
	QPointF o1 = pos + this->offsets.first;
	if (this->outOfPixmap(o1))
		pos -= QPointF(qMin(0.0, o1.x()), qMin(0.0, o1.y()));
	QPointF o2 = pos + this->offsets.second;
	if (this->outOfPixmap(o2))
		pos += QPointF(qMin(0.0, this->_pixmap.width() - o2.x()),
			qMin(0.0, this->_pixmap.height() - o2.y()));
	//# XXX: The next line tracks the new position of the cursor
	//	# relative to the shape, but also results in making it
	//	# a bit "shaky" when nearing the border and allows it to
	//	# go outside of the shape's area for some reason.
	//	# self.calculateOffsets(self.selectedShapes, pos)
	QPointF dp = pos - this->prevPoint;
	if (!dp.toPoint().isNull()) {
		foreach(Shape a_shape, a_shapes)
			a_shape.moveBy(dp);
		this->prevPoint = pos;
		return true;
	}
	return false;
}
void Canvas::boundedShiftShape(QVector<Shape> a_shapes) {
	/*# Try to move in one direction, and if it fails in another.
	# Give up if both fail.*/
	QPointF point = a_shapes[0].points()[0];
	QPointF offset(8.0, 8.0);
	this->offsets = std::make_pair(QPoint(),QPoint());
	this->prevPoint = point;
	if (!this->boundedMoveShapes(a_shapes, point - offset))
		this->boundedMoveShapes(a_shapes, point + offset);
}
void Canvas::boundedMoveVertex(QPointF pos) {
	int index = *this->hVertex;
	QPointF point = this->hShape.points()[index];
	if (this->outOfPixmap(pos))
		pos = this->intersectionPoint(point, pos);
	hShape.moveVertexBy(index, pos - point);
}
void Canvas::finalise() {
	assert(this->current);
	this->current.close();
	this->myShapes.append(this->current);
	//setShapeVisible(this->myShapes.end() - 1, 1);//debug
	this->current = Shape();//.reset();
	this->setHiding(false);
	emit newShape();
	this->update();
	this->setEditing();
}
void Canvas::appendShape(Shape genShape) {
	if (this->mode == EDIT) {
		this->myShapes.append(genShape);
		//setShapeVisible(this->myShapes.end() - 1, 1);//debug
		boundedShiftShape(QVector<Shape>{*(myShapes.end() - 1)});
		//this->deSelectShape();
		this->setHiding(false);
		emit newShape();
		this->update();
	}
}