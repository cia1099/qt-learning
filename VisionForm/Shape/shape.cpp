//#pragma  comment(lib, "../x64/Debug/PopWidgets.lib")
#pragma comment(lib, "opencv_world400d.lib")
#include "shape.h" //Qt create libaray
//#include "../PopWidgets/generictools.h"
#include <opencv2/opencv.hpp>
#include <QtGlobal>
#include <QVector2D>
#include <vector>
#include <tuple>
#include <random>
#include <set>
#include <cstdlib>
#include <algorithm>
#include <cmath>
//#include <Eigen/Eigen>
//#include <Eigen/Dense>
using namespace std;
using namespace shape;
using namespace popwin;
using namespace cv;
//using namespace Eigen;
Matqt_<float> popwin::toHomogeneous2D(const QVector<QPointF>& points) {
	const int N(points.size());
	Matqt_<float> vec(3, N);
	for (int i = 0; i != N; ++i) {
		vec[i] = points[i].x();
		vec[i + N] = points[i].y();
		vec[i + (N << 1)] = 1.0;
	}
	return vec;
}
#define _SCL_SECURE_NO_WARNINGS
void non_zeroPts(InputArray, OutputArray);
std::tuple<vector<int>, double> circle_inliners(InputArray, InputArray, float, float);
tuple<vector<int>, double> line_inliners(InputArray src, InputArray src_direct, float margin);
popwin::Matqt_<int> generateIndices(int exclude, int=3);
QColor AbstractShape::line_color(DEFAULT_LINE_COLOR);
QColor AbstractShape::fill_color(DEFAULT_FILL_COLOR);
QColor AbstractShape::select_line_color(DEFAULT_SELECT_LINE_COLOR);
QColor AbstractShape::select_fill_color(DEFAULT_SELECT_FILL_COLOR);
QColor AbstractShape::vertex_fill_color(DEFAULT_VERTEX_FILL_COLOR);
QColor AbstractShape::hvertex_fill_color(DEFAULT_HVERTEX_FILL_COLOR);
int AbstractShape::point_type(AbstractShape::P_ROUND);
int AbstractShape::point_size(8);
qreal AbstractShape::scale(1.0);
double AbstractShape::break_var(std::sqrt(10.0));
AbstractShape::AbstractShape(QColor line_color, int flag, int label) :
	_label(label), _flags(flag), _line_color(line_color)
{
	_highlightMode = NEAR_VERTEX;
	_highlightIndex = -1;
	_highlightSetting[NEAR_VERTEX] = make_tuple(4.0, P_ROUND);
	_highlightSetting[MOVE_VERTEX] = make_tuple(1.5, P_SQUARE);
	_fill = false;
	_selected = false;
	_closed = false;
	_isVisible = true;
}
AbstractShape::AbstractShape(const AbstractShape& rhs) :
	_label(rhs._label), _shape_type(rhs._shape_type), _flags(rhs._flags), _line_color(rhs._line_color),
	points(rhs.points)
{
	_highlightMode = rhs._highlightMode;
	_highlightIndex = rhs._highlightIndex;
	_highlightSetting[MOVE_VERTEX] = rhs._highlightSetting.at(MOVE_VERTEX);
	_highlightSetting[NEAR_VERTEX] = rhs._highlightSetting.at(NEAR_VERTEX);
	_fill = rhs._fill;
	_selected = false;
	_closed = rhs._closed;
}
void AbstractShape::addPoint(const QPointF& p) {
	if (!points.empty() && p == points[0])
		this->close();
	else
		points.push_back(p);
}
QPointF AbstractShape::popPoint() {
	if (points.empty())
		return QPointF();
	QPointF tmp = points.rbegin()[0];
	points.pop_back();
	return tmp;
}
QRectF AbstractShape::getRectFromLine(QPointF pt1, QPointF pt2) {
	qreal x1(pt1.x()), y1(pt1.y());
	qreal x2(pt2.x()), y2(pt2.y());
	return QRectF(x1, y1, x2 - x1, y2 - y1);
}
void AbstractShape::drawVertex(QPainterPath& path, int idx) {
	qreal d = this->point_size / this->scale;
	int whatsShape = this->point_type;
	QPointF point = this->points[idx];
	if (_highlightIndex >= 0) {
		this->vertex_fill_color = this->hvertex_fill_color;
		if (idx == _highlightIndex) {
			d *= std::get<0>(_highlightSetting[_highlightMode]);
			whatsShape = std::get<1>(_highlightSetting[_highlightMode]);
		}
	}
	else
		this->vertex_fill_color = AbstractShape::vertex_fill_color;
	if (whatsShape == AbstractShape::P_SQUARE)
		path.addRect(point.x() - d*0.5, point.y() - d*0.5, d, d);
	else if (whatsShape == AbstractShape::P_ROUND)
		path.addEllipse(point, d*0.5, d*0.5);
	else
		assert(false);
}
int AbstractShape::nearestVertex(QPointF p, qreal epsilon) {
	qreal min_distance = 1e10;
	int min_idx(-1);
	for (int i = 0; i != points.size(); ++i) {
		QPointF diff_pos = points[i] - p;
		qreal dist = std::sqrt(QPointF::dotProduct(diff_pos, diff_pos));
		if (dist <= epsilon && dist < min_distance) {
			min_distance = dist;
			min_idx = i;
		}
	}
	return min_idx;
}
int AbstractShape::nearestEdge(QPointF p, qreal epsilon) {
	qreal min_distance(1e10);
	int post_i(-1);
	for (int i = 0; i != points.size(); ++i) {
		int pre_i = i - 1;
		pre_i += pre_i < 0 ? static_cast<int>(points.size()) : 0;
		qreal dist = distanceToLine(p, qMakePair(points[pre_i], points[i]));
		if (dist <= epsilon && dist < min_distance) {
			min_distance = dist;
			post_i = i;
		}
	}
	return post_i;
}
QRectF AbstractShape::getCircleRectFromLine(QPair<QPointF, QPointF> line) {
	QPointF dir_R = line.first - line.second;
	qreal R = std::sqrt(QPointF::dotProduct(dir_R, dir_R));
	QPointF c = line.first;
	return QRectF(c.x() - R, c.y() - R, 2 * R, 2 * R);
}

qreal AbstractShape::distanceToLine(QPointF point, QPair<QPointF, QPointF> line) {
	QVector2D p1(line.first), p2(line.second), p3(point);
	if (QVector2D::dotProduct(p3 - p1, p2 - p1) < 0)
		return (p3 - p1).length();
	if (QVector2D::dotProduct(p3 - p2, p1 - p2) < 0)
		return (p3 - p2).length();
	return p3.distanceToLine(p1, (p2 - p1).normalized());
}

void AbstractShape::moveBy(QPointF offset) {
	for (int i = 0; i != points.size(); ++i)
		points[i] += offset;
}

void CirCleShape::paint(QPainter* painter) {
	if (this->points.empty()) return;
	QColor tmp_color = this->_selected ? select_line_color : _line_color;
	QPen pen(tmp_color);
	pen.setWidth(qMax(1, qRound(2.0 / AbstractShape::scale)));
	painter->setPen(pen);
	QPainterPath line_path, ruler_path;
	QPainterPath vrtx_path;
	QRectF boundingBox;

	if (points.size() == 2) {
		boundingBox = getCircleRectFromLine(qMakePair(points[0], points[1]));
		line_path.addEllipse(boundingBox);
		QPointF dir = points[1] - points[0];
		dir /= qSqrt(QPointF::dotProduct(dir, dir));
		QPointF innerRadius = points[1] - dir*(rulerLength >> 1);
		QPointF outerRadius = points[1] + dir*(rulerLength >> 1);
		ruler_path.addEllipse(getCircleRectFromLine(qMakePair(points[0], innerRadius)));
		ruler_path.addEllipse(getCircleRectFromLine(qMakePair(points[0], outerRadius)));
	}
	for (int i = 0; i != points.size(); ++i)
		drawVertex(vrtx_path, i);

	painter->drawPath(line_path);
	painter->drawPath(vrtx_path);
	painter->fillPath(vrtx_path, vertex_fill_color);
	if (this->_fill) {
		tmp_color = _selected ? select_fill_color : fill_color;
		painter->fillPath(line_path, tmp_color);
	}
	painter->setPen(QPen(Qt::gray, pen.width()>>1, Qt::DashDotDotLine));
	painter->drawPath(ruler_path);
}
QPainterPath CirCleShape::makePath() {
	QPainterPath line_path;
	if (points.size() == 2) {
		QRectF boundingBox = getCircleRectFromLine(qMakePair(points[0], points[1]));
		line_path.addEllipse(boundingBox);
	}
	return line_path;
}
QPointF CirCleShape::determineRefPos(const QPixmap& pix){
	QImage qImg = pix.toImage();
	//if ((qImg.depth() >> 3) == 3)
	qImg = qImg.convertToFormat(QImage::Format_Grayscale8);
	Mat1b img(qImg.height(), qImg.width(), qImg.bits(), qImg.bytesPerLine()), edge_roi;
	cv::Canny(img, edge_roi, 100, 200); //here can be rewritten adaptive to find thresholds
	QPoint qc0(points.front().toPoint());
	cv::Point c0(qc0.x(), qc0.y());
	QPointF dir_r(points.back() - points.front());
	int mid_r = qRound(qSqrt(QPointF::dotProduct(dir_r, dir_r)));
	int r1(mid_r - (this->rulerLength >> 1)), r2(mid_r + (this->rulerLength >> 1));
	Mat1b mask = Mat1b::zeros(img.rows, img.cols), mask1, mask2;
	mask.copyTo(mask1);
	mask.copyTo(mask2);
	circle(mask1, c0, r1, Scalar(255), cv::FILLED);
	circle(mask2, c0, r2, Scalar(255), cv::FILLED);
	bitwise_xor(mask1, mask2, mask);
	bitwise_and(edge_roi, mask, mask);
	Mat1f pts;
	non_zeroPts(mask, pts);
	
	const int atLeastInliner = cvRound(pts.rows*this->conf);
	Mat1f bst_c0(Mat1f::zeros(1, 2));
	float bst_r0(0.0);
	vector<int> bst_inliner;
	double min_var(1e6);
	for (int iter = 0; iter != this->ransec_iter && min_var >= this->break_var; ++iter) {
		Matqt_<int> select_idx = generateIndices(pts.rows);
		QVector<QPointF> qDenominator, qNumerator_x, qNumerator_y;
		for (int i = 0; i != 3; ++i) {
			int idx = select_idx[i];
			QPointF select_pt(pts[idx][0], pts[idx][1]);
			qreal square_sum = QPointF::dotProduct(select_pt, select_pt);
			qDenominator.push_back(select_pt);
			qNumerator_x.push_back(QPointF(square_sum, select_pt.y()));
			qNumerator_y.push_back(QPointF(select_pt.x(), square_sum));
		}
		Matqt_<float> denomi = toHomogeneous2D(qDenominator);
		Mat1f denominator(Mat1f::zeros(3, 3)), numeratorX(Mat1f::zeros(3, 3)), numeratorY(Mat1f::zeros(3, 3));
		denomi.writeTo(denominator[0]);
		double Cramer = cv::determinant(denominator) * 2;
		if(std::abs(Cramer) < 1e-3)
			continue;
		Matqt_<float> nuX = toHomogeneous2D(qNumerator_x);
		Matqt_<float> nuY = toHomogeneous2D(qNumerator_y);
		nuX.writeTo(numeratorX[0]);
		nuY.writeTo(numeratorY[0]);
		double Cramer_x = cv::determinant(numeratorX);
		double Cramer_y = cv::determinant(numeratorY);
		Mat1f center = (Mat1f(1, 2) << Cramer_x / Cramer, Cramer_y / Cramer);
		//---determine radius
		Mat1f a = pts.row(select_idx[1]) - pts.row(select_idx[0]);
		Mat1f b = pts.row(select_idx[2]) - pts.row(select_idx[0]);
		Mat1f c = pts.row(select_idx[1]) - pts.row(select_idx[2]);
		double S = 0.5*qAbs(a[0][0] * b[0][1] - a[0][1] * b[0][0]);//cv::norm(a.cross(b), cv::NORM_L2);
		float R = cv::norm(a)*cv::norm(b)*cv::norm(c) / 4 / S;
		std::tuple<vector<int>, double> inliner_var = circle_inliners(pts, center, R, rulerLength);
		if (std::get<0>(inliner_var).size() >= atLeastInliner) {
			if (std::get<1>(inliner_var) < min_var) {
				min_var = std::get<1>(inliner_var);
				bst_inliner = std::get<0>(inliner_var);
				bst_c0 = center;
				bst_r0 = R;
			}
		}
	}//for(iter)
	_refPoint = bst_inliner.size() == 0 ? QPointF() : QPointF(bst_c0[0][0], bst_c0[0][1]);
	_radius = bst_r0;
	return _refPoint;
}
void LineShape::paint(QPainter* painter) {
	if (this->points.empty()) return;
	QColor tmp_color = this->_selected ? select_line_color : _line_color;
	QPen pen(tmp_color);
	pen.setWidth(qMax(1, qRound(2.0 / AbstractShape::scale)));
	painter->setPen(pen);
	QPainterPath line_path, ruler_path;
	QPainterPath vrtx_path;
	QRectF boundingBox;

	line_path.moveTo(this->points[0]);
	for (int i = 0; i != points.size(); ++i) {
		line_path.lineTo(points[i]);
		drawVertex(vrtx_path, i);
		if (i > 0) {
			QPointF dir = points[i] - points[i - 1];
			dir /= qSqrt(QPointF::dotProduct(dir, dir));
			QPointF v_dir(-dir.y(), dir.x());
			ruler_path.moveTo(points[i - 1] - v_dir*(rulerLength >> 1));
			ruler_path.lineTo(points[i] - v_dir*(rulerLength >> 1));
			ruler_path.moveTo(points[i - 1] + v_dir*(rulerLength >> 1));
			ruler_path.lineTo(points[i] + v_dir*(rulerLength >> 1));
		}
	}

	painter->drawPath(line_path);
	painter->drawPath(vrtx_path);
	painter->fillPath(vrtx_path, vertex_fill_color);
	if (this->_fill) {
		tmp_color = _selected ? select_fill_color : fill_color;
		painter->fillPath(line_path, tmp_color);
	}
	painter->setPen(QPen(Qt::gray, pen.width() >> 1, Qt::DashDotDotLine));
	painter->drawPath(ruler_path);
}
QPainterPath LineShape::makePath() {
	QPainterPath path(this->points[0]);
	if (points.size() == 2) {
		QPointF dir = points[1] - points[0];
		dir /= qSqrt(QPointF::dotProduct(dir, dir));
		QPointF v_dir(dir.y(), -dir.x());
		QPointF tl = points[1] - v_dir*(rulerLength); //>> 1);//for nice grabing it
		QPointF br = points[1] + v_dir*(rulerLength); //>> 1);
		path.lineTo(points[1]);
		path.lineTo(tl);
		path.lineTo(br);
	}
	return path;
}
QPointF LineShape::determineRefPos(const QPixmap& pix) {
	QImage qImg = pix.toImage();
	//if ((qImg.depth() >> 3) == 3)
	qImg = qImg.convertToFormat(QImage::Format_Grayscale8);
	Mat1b img(qImg.height(), qImg.width(), qImg.bits(), qImg.bytesPerLine()), edge_roi;
	cv::Canny(img, edge_roi, 100, 200); //here can be adaptive to find thresholds
	QPoint qpt1(points.front().toPoint());
	Vec2f pt1(qpt1.x(), qpt1.y());
	QPoint qpt2(points.back().toPoint());
	Vec2f pt2(qpt2.x(), qpt2.y());
	Vec2f dir_line = pt2 - pt1;
	dir_line /= cv::norm(dir_line);
	Vec2f normal_line(dir_line[1], -dir_line[0]); 
	Vec2f v_pt1 = pt1 + normal_line*(this->rulerLength >> 1);
	Vec2f v_pt2 = pt2 + normal_line*(this->rulerLength >> 1);
	Mat1b mask = Mat1b::zeros(img.rows, img.cols);
	line(mask, Point2i(v_pt1[0], v_pt1[1]), Point2i(v_pt2[0], v_pt2[1]), Scalar(255), rulerLength);
	bitwise_and(edge_roi, mask, mask);
	Mat1f pts;
	non_zeroPts(mask, pts);

	const int atLeastInliner = cvRound(pts.rows*this->conf);
	Vec3f bst_line(0, 0, 0);
	vector<int> bst_inliner;
	double min_var(1e6);
	for (int iter = 0; iter != this->ransec_iter&&min_var > this->break_var; ++iter) {
		Matqt_<int> select_idx = generateIndices(pts.rows, 2);
		Vec3f select_pt1(pts[select_idx[0]][0], pts[select_idx[0]][1], 1);
		Vec3f select_pt2(pts[select_idx[1]][0], pts[select_idx[1]][1], 1);
		Vec3f line_vec = select_pt1.cross(select_pt2);
		double norm_line_vec = cv::norm(line_vec);
		if(norm_line_vec < 1e-5)
			continue;
		line_vec /= norm_line_vec;
		tuple<vector<int>, double> inliner_var = line_inliners(pts, line_vec, this->rulerLength);
		if (std::get<0>(inliner_var).size() >= atLeastInliner) {
			if (std::get<1>(inliner_var) < min_var) {
				min_var = std::get<1>(inliner_var);
				bst_inliner = std::get<0>(inliner_var);
				bst_line = line_vec;
			}
		}
	}//for(iter)
	//---project point to line
	Vec3f proj_L_pt1 = Vec3f(pt1[0], pt1[1], 1);
	proj_L_pt1 = proj_L_pt1 - proj_L_pt1.dot(bst_line)*bst_line;
	Vec3f proj_L_pt2 = Vec3f(pt2[0], pt2[1], 1);
	proj_L_pt2 = proj_L_pt2 - proj_L_pt2.dot(bst_line)*bst_line;
	Vec3f line_middle_pt = 0.5*(proj_L_pt1 + proj_L_pt2);
	_refPoint = bst_inliner.size() == 0 ? QPointF() : QPointF(line_middle_pt[0],line_middle_pt[1]);
	_linePresent = QVector3D(bst_line[0], bst_line[1], bst_line[2]);

	return _refPoint;
}

GenArcWelds::GenArcWelds(QPair<QPointF,QPointF> pos, QColor c) :
	AbstractShape(c) {
	this->points.emplace_back(pos.second);
	this->_refPoint = pos.first;
	this->_shape_type = SHAPE_TYPE::GEN_ARC_WELD;
}
GenArcWelds::GenArcWelds(Shape copy) :
	AbstractShape(copy.drawLineColor()) {
	this->points.emplace_back(copy.points().back());
	this->_refPoint = copy.refPoint();
	this->_shape_type = SHAPE_TYPE::GEN_ARC_WELD;
}
GenArcWelds::GenArcWelds(const GenArcWelds& copy):
AbstractShape(copy._line_color){
	this->points = copy.points;
	this->_refPoint = copy._refPoint;
	this->_shape_type = SHAPE_TYPE::GEN_ARC_WELD;
}
void GenArcWelds::paint(QPainter* painter) {
	if (this->points.empty()) return;
	QColor tmp_color = this->_selected ? select_line_color : _line_color;
	QPen pen(tmp_color);
	pen.setWidth(qMax(1, qRound(2.0 / AbstractShape::scale)));
	painter->setPen(pen);
	QPainterPath line_path, support_path;
	QPainterPath vrtx_path;
	QRectF boundingBox;
	if (points.size() == 1) {
		boundingBox = getCircleRectFromLine(qMakePair(_refPoint, points[0]));
		QPointF dir = points[0] - _refPoint;
		qreal outside_acos = dir.y() > 0 ? -1 : 1;
		dir /= qSqrt(QPointF::dotProduct(dir, dir));
		qreal startAngle = std::acos(QPointF::dotProduct(dir, QPointF(1, 0))) * 180.0 / std::acos(-1);
		support_path.moveTo(_refPoint);
		support_path.arcTo(boundingBox, startAngle*outside_acos, 0.5*this->sweepLength);
		support_path.moveTo(_refPoint);
		support_path.arcTo(boundingBox, startAngle*outside_acos, -0.5*this->sweepLength);
		vector<QPointF> gen_pos;
		this->generateWeldPoints(gen_pos);
		const int rad = genWeldSize >> 1;
		for (int i = 0; i != gen_pos.size(); ++i) {
			QRectF box(gen_pos[i] - QPointF(rad, rad), gen_pos[i] + QPointF(rad, rad));
			line_path.addEllipse(box);
		}
	}
	//line_path = this->makePath();
	for (int i = 0; i != points.size(); ++i)
		drawVertex(vrtx_path, i);

	painter->drawPath(line_path);
	painter->drawPath(vrtx_path);
	painter->fillPath(vrtx_path, vertex_fill_color);
	if (this->_fill) {
		tmp_color = _selected ? select_fill_color : fill_color;
		painter->fillPath(line_path, tmp_color);
	}
	painter->setPen(QPen(Qt::gray, pen.width() >> 1, Qt::DotLine));
	painter->drawPath(support_path);
}
QPainterPath GenArcWelds::makePath() {
	QPainterPath line_path;
	if (points.size() == 1) {
		vector<QPointF> gen_pos;
		this->generateWeldPoints(gen_pos);
		const int rad = genWeldSize >> 1;
		for (int i = 0; i != gen_pos.size(); ++i) {
			QRectF box(gen_pos[i] - QPointF(rad, rad), gen_pos[i] + QPointF(rad, rad));
			line_path.addEllipse(box);
		}
	}
	return line_path;
}
void GenArcWelds::generateWeldPoints(vector<QPointF>& dst) {
	QPointF dir = points[0] - _refPoint;
	const qreal r = qSqrt(QPointF::dotProduct(dir, dir));
	dir /= r;
	Mat1f vec_corr = (Mat1f(2, 2) << dir.y(), dir.x(), -dir.x(), dir.y());
	int N = num_welds > 2 ? num_welds : num_welds == 1 ? 3 : 4;
	int j = num_welds > 2 ? 0 : 1;
	const qreal ang_step(sweepLength / (N - 1));
	qreal ang_start = 0.5*(180.0 - sweepLength);
	vector<QPointF> gen_pos;
	for (int i = 0+j; i != N-j; ++i) {
		qreal theta = (ang_start + i*ang_step)*acos(-1)/180.0;
		Mat1f Oxy = vec_corr*(Mat1f(2, 1) << r*std::cos(theta), r*std::sin(theta));
		gen_pos.push_back(QPointF(Oxy[0][0], Oxy[0][1]) + _refPoint);
	}
	dst = gen_pos;
}

GenLineWelds::GenLineWelds(Shape copy) :
	LineShape(copy.drawLineColor()) {
	this->points.emplace_back(copy.refPoint());
	this->_refPoint = copy.refPoint();
	this->_linePresent = copy.linePresent();
	this->_shape_type = SHAPE_TYPE::GEN_LINE_WELD;
}
GenLineWelds::GenLineWelds(const GenLineWelds& copy) :
	LineShape(copy._line_color) {
	this->points = copy.points;
	this->_refPoint = copy._refPoint;
	this->_linePresent = copy._linePresent;
	this->_shape_type = SHAPE_TYPE::GEN_LINE_WELD;
}
void GenLineWelds::paint(QPainter* painter) {
	if (this->points.empty()) return;
	QColor tmp_color = this->_selected ? select_line_color : _line_color;
	QPen pen(tmp_color);
	pen.setWidth(qMax(1, qRound(2.0 / AbstractShape::scale)));
	painter->setPen(pen);
	QPainterPath line_path, support_line(this->_refPoint);
	QPainterPath vrtx_path;
	QRectF boundingBox;
	if (points.size() == 1) {
		QPointF dir(_linePresent.y(), -_linePresent.x());
		QPointF pt1 = points[0] + dir*(this->line_length >> 1);
		QPointF pt2 = points[0] - dir*(this->line_length >> 1);
		support_line.lineTo(points[0]);
		support_line.lineTo(pt1);
		support_line.moveTo(this->_refPoint);
		support_line.lineTo(points[0]);
		support_line.lineTo(pt2);
		vector<QPointF> gen_pos;
		this->generateWeldPoints(gen_pos);
		const int rad = genWeldSize >> 1;
		for (int i = 0; i != gen_pos.size(); ++i) {
			QRectF box(gen_pos[i] - QPointF(rad, rad), gen_pos[i] + QPointF(rad, rad));
			line_path.addEllipse(box);
		}
	}
	//line_path = this->makePath();
	for (int i = 0; i != points.size(); ++i)
		drawVertex(vrtx_path, i);
	
	painter->drawPath(line_path);
	painter->drawPath(vrtx_path);
	painter->fillPath(vrtx_path, vertex_fill_color);
	if (this->_fill) {
		tmp_color = _selected ? select_fill_color : fill_color;
		painter->fillPath(line_path, tmp_color);
	}
	painter->setPen(QPen(Qt::gray, pen.width() >> 1, Qt::DotLine));
	painter->drawPath(support_line);
}
QPainterPath GenLineWelds::makePath() {
	QPainterPath line_path;// (this->_refPoint);
	if (points.size() == 1) {
		vector<QPointF> gen_pos;
		this->generateWeldPoints(gen_pos);
		const int rad = genWeldSize;// >> 1;I dont know why the paint on line the size is smaller
		for (int i = 0; i != gen_pos.size(); ++i) {
			QRectF box(gen_pos[i] - QPointF(rad, rad), gen_pos[i] + QPointF(rad, rad));
			line_path.addEllipse(box);
		}
	}
	return line_path;
}
void GenLineWelds::generateWeldPoints(std::vector<QPointF>& dst) {
	QPointF dir(_linePresent.y(), -_linePresent.x());
	int N = num_welds > 2 ? num_welds : num_welds == 1 ? 3 : 4;
	int j = num_welds > 2 ? 0 : 1;
	const qreal step((qreal)this->line_length / (qreal)(N - 1));
	QPointF start_point = points[0] - dir*(line_length >> 1);
	vector<QPointF> gen_pos;
	for (int i = 0 + j; i != N - j; ++i) {
		gen_pos.push_back(start_point + i*step*dir);
	}
	dst = gen_pos;
}

void non_zeroPts(InputArray src, OutputArray dst) {
	Mat1f pts;// (dst.getMatRef());
	Mat1b img(src.getMat());
	uchar const* ptr_img = img[0];
	const int M(img.rows), N(img.cols);
	for (int y = 0; y != M; ++y) {
		for (int x = 0; x != N; ++x) {
			if (ptr_img[y*N + x] != 0) {
				Mat1f pt = (Mat1f(1, 2) << x, y);
				pts.push_back(pt);
			}
		}
	}
	dst.create(pts.size(), pts.type());
	dst.getMatRef() = pts;
}
std::tuple<vector<int>,double> circle_inliners(InputArray src, InputArray src_center, float rad, float margin) {
	Mat1f pts(src.getMat()), center(src_center.getMat());
	vector<int> inliner;
	double var(0.0);
	const double M(pts.rows);
	for (int i = 0; i != pts.rows; ++i) {
		float var_R = qPow(pts[i][0] - center[0][0], 2) + qPow(pts[i][1] - center[0][1], 2);
		double element_var = std::fabs(std::sqrtf(var_R) - rad);
		if (element_var < 0.5*margin) {
			inliner.emplace_back(i);
			var += 1.0 / M*element_var;
		}
	}
	return std::make_tuple(inliner, var);
}
tuple<vector<int>,double> line_inliners(InputArray src, InputArray src_direct, float margin) {
	Mat1f pts(src.getMat()), line_dir(src_direct.getMat());
	float lineNorm = std::sqrtf(powf(line_dir[0][0], 2) + powf(line_dir[0][1], 2));
	if (lineNorm != 1)
		line_dir /= lineNorm;
	//int inliner(0);
	vector<int> inliner;
	double var(0.0);
	const double M(pts.rows);
	for (int i = 0; i != pts.rows; ++i) {
		float element_var = pts[i][0] * line_dir[0][0] + pts[i][1] * line_dir[0][1] + line_dir[0][2];
		if (element_var < 0.5*margin) {
			inliner.emplace_back(i);
			var += 1.0 / M*element_var;
		}
	}
	return make_tuple(inliner, var);
}
Matqt_<int> generateIndices(int exclude, int num) {
	assert(exclude >= 3);
	static default_random_engine gen(rand() % 10 + 1);
	uniform_int_distribution<int> distribution(0, exclude - 1);
	set<int> sampleSpace;
	while(sampleSpace.size() < num){
		sampleSpace.emplace(distribution(gen));
	}
	Matqt_<int> mySample(1, num);
	//microsoft law
	std::copy(sampleSpace.begin(), sampleSpace.end(), 
		stdext::checked_array_iterator<int *>(mySample.data, num));

	return mySample;
}