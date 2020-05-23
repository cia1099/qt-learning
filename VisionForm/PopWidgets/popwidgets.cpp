#pragma execution_character_set("utf-8")
//#pragma comment(lib, "opencv_world400d.lib")
//#pragma comment(lib, "../x64/Debug/Shape.lib") ////used CMake building need command it
#include "popwidgets.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <opencv2/opencv.hpp>
#include <string>
#include <cstdlib>
using namespace popwin;
using namespace cv;
using std::string;

//qRegisterMetaType<popwin::Matqt_<float> >("Matqt1f");

QSize CalibrationWidget::frameSize(100, 40);
QSize CalibrationWidget::btnSize(150, 40);
const int CalibrationWidget::labelWidth(50);
CalibrationWidget::CalibrationWidget(QWidget* parent) :
	QWidget(parent),sample(new SampleTable(this)),delegate(new SpinBoxDelegate(this)),
	tableView(new TableViewer),affineMatrix(2,3) {
	QVBoxLayout *mainLayout = new QVBoxLayout;
	groupBox0 = new QGroupBox;
	groupBox[0] = groupBox0;
	groupBox1 = new QGroupBox;
	groupBox[1] = groupBox1;
	groupBox2 = new QGroupBox;
	groupBox[2] = groupBox2;
	groupBox3 = new QGroupBox;
	groupBox[3] = groupBox3;
	groupBox4 = new QGroupBox;
	groupBox[4] = groupBox4;
	for (int i = 0; i != GROUP_NUM; ++i) {
		groupBox[i]->setParent(this);
		groupBox[i]->setTitle(GroupName[i]);
	}
	this->spinArray = QSharedPointer<QSpinBox>(new QSpinBox[SPIN_NUM], [](QSpinBox* s) {delete[] s; });
	for (int i = 0; i != SPIN_NUM; ++i) {
		spinArray.get()[i].setMinimumSize(frameSize);
		spinArray.get()[i].setMinimum(-99999);
		spinArray.get()[i].setMaximum(99999);
	}
	this->display = QSharedPointer<QLabel>(new QLabel[DISPLAY_IDX::STATUS + 1], [](QLabel* lab) {delete[] lab; });
	for (int i = 0; i != DISPLAY_IDX::STATUS + 1; ++i) {
		display.get()[i].setMinimumSize(frameSize);
		display.get()[i].setFrameStyle(QFrame::Panel | QFrame::Plain);
		//display.get()[i].setLineWidth(0);
		display.get()[i].setMidLineWidth(2);
	}
	//----build first groupbox
	QWidget *container = new QWidget(groupBox[0]);
	QVBoxLayout *btnLayout = new QVBoxLayout(container);
	this->btn = QSharedPointer<QPushButton>(new QPushButton[BUTTON_NUM], [](QPushButton* b) {delete[] b; });
	for (int i = 0; i != BUTTON_NUM; ++i) {
		btn.get()[i].setParent(container);
		//btn.get()[i].setMinimumSize(btnSize);
		btn.get()[i].setMaximumSize(btnSize);
		btn.get()[i].setText(BtnName[i]);
		//btn[i].addAction(); need actions
		btnLayout->addWidget(btn.get() + i);
	}
	container->setLayout(btnLayout);
	QHBoxLayout *first_groupLayout = new QHBoxLayout;
	tableView->setModel(this->sample);
	tableView->resizeColumnsToContents();
	tableView->setFixedWidth(485);
	tableView->setItemDelegate(this->delegate);
	first_groupLayout->addWidget(this->tableView);
	first_groupLayout->addWidget(container);
	groupBox[0]->setLayout(first_groupLayout);
	//-----build second groupbox
	this->second_group_label = QSharedPointer<QLabel>(new QLabel[6], [](QLabel* lab) {delete[] lab; });
	QString label_str[] = { "Pix_X:","Pix_Y:","X:","mm","Y:","mm" };
	QFont labelFont;
	labelFont.setPointSize(8);
	labelFont.setBold(true);
	for (int i = 0; i != 6; ++i) {
		second_group_label.get()[i].setText(label_str[i]);
		second_group_label.get()[i].setFont(labelFont);
		//second_group_label[i].setMinimumSize(frameSize);
		second_group_label.get()[i].setMaximumWidth(labelWidth);
		second_group_label.get()[i].setAlignment(Qt::AlignCenter);
	}
	QHBoxLayout *label_layout = new QHBoxLayout;//(groupBox.get()+1);
	label_layout->addWidget(second_group_label.get());
	label_layout->addWidget(this->spinArray.get() + SPINBOX_IDX::PIX_X);
	label_layout->addWidget(second_group_label.get() + 1);
	label_layout->addWidget(this->spinArray.get() + SPINBOX_IDX::PIX_Y);
	label_layout->addWidget(second_group_label.get() + 2);
	label_layout->addWidget(this->display.get() + DISPLAY_IDX::VAILID_X);
	label_layout->addWidget(second_group_label.get() + 3);
	label_layout->addWidget(second_group_label.get() + 4);
	label_layout->addWidget(this->display.get() + DISPLAY_IDX::VAILID_Y);
	label_layout->addWidget(second_group_label.get() + 5);
	this->tranform_coordinate = new QPushButton;
	tranform_coordinate->setText(tr("映射計算"));
	tranform_coordinate->setFixedSize(btnSize);
	//tranform_coordinate->addAction();
	QHBoxLayout *btnHLayout = new QHBoxLayout;
	btnHLayout->addWidget(tranform_coordinate);
	QVBoxLayout *second_groupLayout = new QVBoxLayout;
	second_groupLayout->addLayout(label_layout);
	second_groupLayout->addLayout(btnHLayout);
	groupBox[1]->setLayout(second_groupLayout);
	//----build third groupbox
	this->third_group_label = QSharedPointer<QLabel>(new QLabel[4], [](QLabel* lab) {delete[] lab; });
	QString third_str[] = { tr("X軸"),"mm",tr("Y軸"),"mm" };
	for (int i = 0; i != 4; ++i) {
		third_group_label.get()[i].setText(third_str[i]);
		third_group_label.get()[i].setFont(labelFont);
		third_group_label.get()[i].setMaximumWidth(labelWidth);
		third_group_label.get()[i].setAlignment(Qt::AlignCenter);
	}
	QHBoxLayout* third_layout = new QHBoxLayout;
	third_layout->addWidget(third_group_label.get());
	third_layout->addWidget(spinArray.get() + SPINBOX_IDX::MC_X);
	third_layout->addWidget(third_group_label.get() + 1);
	third_layout->addWidget(third_group_label.get() + 2);
	third_layout->addWidget(spinArray.get() + SPINBOX_IDX::MC_Y);
	third_layout->addWidget(third_group_label.get() + 3);
	groupBox[2]->setLayout(third_layout);
	//----build fourth groupbox
	this->fourth_group_label = QSharedPointer<QLabel>(new QLabel[4], [](QLabel* lab) {delete[] lab; });
	QString fourth_str[] = { "Pixo_X:","Pixo_Y:",tr("與水平方向夾角"),"rad" };
	for (int i = 0; i != 4; ++i) {
		fourth_group_label.get()[i].setText(fourth_str[i]);
		fourth_group_label.get()[i].setFont(labelFont);
		if(fourth_str[i] != tr("與水平方向夾角"))
			fourth_group_label.get()[i].setMaximumWidth(labelWidth);
		fourth_group_label.get()[i].setAlignment(Qt::AlignCenter);
	}
	QHBoxLayout *fourth_layout = new QHBoxLayout;
	fourth_layout->addWidget(fourth_group_label.get());
	fourth_layout->addWidget(display.get() + DISPLAY_IDX::NEW_X);
	fourth_layout->addWidget(fourth_group_label.get() + 1);
	fourth_layout->addWidget(display.get() + DISPLAY_IDX::NEW_Y);
	fourth_layout->addWidget(fourth_group_label.get() + 2);
	fourth_layout->addWidget(display.get() + DISPLAY_IDX::NEW_RAD);
	fourth_layout->addWidget(fourth_group_label.get() + 3);
	groupBox[3]->setLayout(fourth_layout);
	//----build fifth groupbox
	this->fifth_group_label = QSharedPointer<QLabel>(new QLabel[3], [](QLabel* lab) {delete[] lab; });
	QString fifth_str[] = { "X:","Y:","angle:" };
	for (int i = 0; i != 3; ++i) {
		fifth_group_label.get()[i].setText(fifth_str[i]);
		fifth_group_label.get()[i].setFont(labelFont);
		fifth_group_label.get()[i].setMaximumWidth(labelWidth);
		fifth_group_label.get()[i].setAlignment(Qt::AlignCenter);
	}
	QHBoxLayout *fifth_layout = new QHBoxLayout;
	fifth_layout->addWidget(display.get() + DISPLAY_IDX::STATUS);
	fifth_layout->addWidget(fifth_group_label.get());
	fifth_layout->addWidget(display.get() + DISPLAY_IDX::RESULT_X);
	fifth_layout->addWidget(fifth_group_label.get() + 1);
	fifth_layout->addWidget(display.get() + DISPLAY_IDX::RESULT_Y);
	fifth_layout->addWidget(fifth_group_label.get() + 2);
	fifth_layout->addWidget(display.get() + DISPLAY_IDX::RESULT_ANGLE);
	groupBox[4]->setLayout(fifth_layout);

	for (int i = 0; i != GROUP_NUM; ++i)
		mainLayout->addWidget(groupBox[i]);
	this->setLayout(mainLayout);

	Mat1f affine_init = Mat1f::eye(2,3);
	//memcpy(this->affineMatrix, affine_init[0], sizeof(affineMatrix));
	affineMatrix.writeFrom(affine_init[0]);
	connect(btn.get(), &QPushButton::clicked, this, &CalibrationWidget::insertRowEvent);
	connect(btn.get() + 1, &QPushButton::clicked, this, &CalibrationWidget::deleteRowEvent);
	connect(btn.get() + 3, &QPushButton::clicked, this, &CalibrationWidget::estimateAffine2D);
	connect(btn.get() + 4, &QPushButton::clicked, this, &CalibrationWidget::saveAffineMatrix);
	connect(tranform_coordinate, &QPushButton::clicked, this, &CalibrationWidget::affineTransform);
}

void CalibrationWidget::insertRowEvent() {
	QModelIndex idx = tableView->currentIndex();
	if (idx.row() >= 0)
		sample->insertRows(idx.row(), 1, idx);
	else
		sample->insertRows(sample->rowCount() - 1, 1, idx);
}

void CalibrationWidget::deleteRowEvent() {
	QModelIndex idx = tableView->currentIndex();
	if (idx.row() >= 0)
		sample->removeRows(idx.row(), 1, idx);
}

void CalibrationWidget::estimateAffine2D() {
	Matqt_<float> points = sample->getTableData();
	Mat1f img_points, machine_points;
	const int cols(sample->columnCount());
	for (int i = 0; i != sample->rowCount(); ++i) {
		Mat1f img_point = (Mat1f(1, 2) << points[i*cols], points[i*cols + 1]);
		Mat1f machine_point = (Mat1f(1, 2) << points[i*cols + 2], points[i*cols + 3]);
		img_points.push_back(img_point);
		machine_points.push_back(machine_point);
	}
	Mat1f affine = cv::estimateAffine2D(img_points, machine_points);
	QSpinBox* spinbox = this->spinArray.get();
	for (int i = 0; i != SPIN_NUM; ++i)
		spinbox[i].interpretText();
	affine[0][2] += spinbox[SPINBOX_IDX::MC_X].value();
	affine[0][5] += spinbox[SPINBOX_IDX::MC_Y].value();
	memcpy(this->affineMatrix.data, affine[0], sizeof(float)*affine.total());
}

void CalibrationWidget::affineTransform() {
	QSpinBox* spinbox = this->spinArray.get();
	for (int i = 0; i != SPIN_NUM; ++i)
		spinbox[i].interpretText();
	QPoint vailid_point(spinbox[SPINBOX_IDX::PIX_X].value(), spinbox[SPINBOX_IDX::PIX_Y].value());
	Matqt_<float> rawPos = toHomogeneous2D(QVector<QPointF>{vailid_point});
	Mat1f affine(2, 3, this->affineMatrix.data, Mat::AUTO_STEP), pos(rawPos.rows,rawPos.cols);
	rawPos.writeTo(pos[0]);
	Mat1f transPos = affine*pos;
	QPointF transPoint(transPos[0][0], transPos[0][1]);
	//QPointF offsetPoint(spinbox[SPINBOX_IDX::MC_X].value(), spinbox[SPINBOX_IDX::MC_Y].value());
	//transPoint += offsetPoint;
	
	QLabel* vailid_x = this->display.get() + DISPLAY_IDX::VAILID_X;
	QLabel* vailid_y = this->display.get() + DISPLAY_IDX::VAILID_Y;
	vailid_x->setText(QString("%1").arg(transPoint.x()));
	vailid_y->setText(QString("%1").arg(transPoint.y()));
}
void CalibrationWidget::saveAffineMatrix() {
	////need to declaim this line above when connecting this signal!!
	//look my NPI computer in chrome Qt bookmark_1
	//e.g. 
	//qRegisterMetaType<popwin::Matqt_<float> >("Matqt1f");
	//connect(someone, &CalibrationWidget::transportAffineMatrix,this,&MainWindow::transprotAffineMatrix);
	emit transportAffineMatrix(this->affineMatrix);
}

SampleTable::SampleTable(QObject* parent) :
	QAbstractTableModel(parent) {
	for (int i = 0; i != 3; ++i) {
		string *ptr_str = new string[this->cols];
		std::shared_ptr<string> share_str(ptr_str, [](string s[]) {delete[] s; });
		tableData.append(share_str);
	}
}
int SampleTable::rowCount(const QModelIndex& qidx) const {
	return tableData.size();
}
int SampleTable::columnCount(const QModelIndex& qidx)const {
	return cols;
}
QVariant SampleTable::data(const QModelIndex &index, int role)const {
	if (!index.isValid())
		return QVariant();
	if (index.row() >= tableData.size() || index.column() >= cols)
		return QVariant();
	if (role == Qt::DisplayRole || role == Qt::EditRole) {
		const int y(index.row()), x(index.column());
		return QString(tableData.at(y).get()[x].c_str());
	}
	else
		return QVariant();
}
QVariant SampleTable::headerData(int section, Qt::Orientation orientation, int role)const {
	if (role != Qt::DisplayRole)
		return QVariant();
	if (orientation == Qt::Vertical)
		return QString("%1").arg(section + 1);
	else
	{
		switch (section)
		{
		case 0:
			return QString("Image X [px]");
			break;
		case 1:
			return QString("Image Y [px]");
			break;
		case 2:
			return QString("Machine X [mm]");
			break;
		case 3:
			return QString("Machine Y [mm]");
			break;
		default:
			return QVariant();
		}
	}
}
Qt::ItemFlags SampleTable::flags(const QModelIndex& index)const {
	if (!index.isValid())
		return Qt::ItemIsEnabled;
	return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}
bool SampleTable::setData(const QModelIndex &index, const QVariant &value, int role){
	if (index.isValid() && role == Qt::EditRole) {
		std::shared_ptr<string> ptr_str = tableData.at(index.row());
		ptr_str.get()[index.column()] = value.toString().toStdString();
		emit dataChanged(index, index);
		return true;
	}
	return false;
}
bool SampleTable::insertRows(int row, int count, const QModelIndex &parent) {
	beginInsertRows(QModelIndex(), row, row + count - 1);
	for (int i = 0; i != count; ++i) {
		string *ptr_str = new string[this->cols];
		std::shared_ptr<string> share_str(ptr_str, [](string s[]) {delete[] s; });
		tableData.insert(row, share_str);
	}
	endInsertRows();
	return true;
}
bool SampleTable::removeRows(int row, int count, const QModelIndex &parent) {
	beginRemoveRows(QModelIndex(), row, row + count - 1);
	for (int i = 0; i != count; ++i)
		tableData.removeAt(i);
	endRemoveRows();
	return true;
}
Matqt_<float> SampleTable::getTableData()const {
	assert(this->rowCount() >= 3);
	Matqt_<float> sample_data(this->rowCount()*cols);
	float value;
	for (int i = 0; i != tableData.size(); ++i) {
		for (int j = 0; j != this->cols; ++j) {
			value = std::strtof(tableData.at(i).get()[j].c_str(), NULL);
			sample_data[i*cols + j] = value;
		}
	}
	return sample_data;
}


SpinBoxDelegate::SpinBoxDelegate(QObject *parent,int min,int max) :
	QItemDelegate(parent),min_value(min),max_value(max){}
QWidget *SpinBoxDelegate::createEditor(QWidget *parent,
	const QStyleOptionViewItem &/* option */,
	const QModelIndex &/* index */) const
{
	QSpinBox *editor = new QSpinBox(parent);
	editor->setMinimum(this->min_value);
	editor->setMaximum(this->max_value);

	return editor;
}
void SpinBoxDelegate::setEditorData(QWidget *editor,
	const QModelIndex &index) const
{
	int value = index.model()->data(index, Qt::EditRole).toInt();

	QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
	spinBox->setValue(value);
}
void SpinBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
	const QModelIndex &index) const
{
	QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
	spinBox->interpretText();
	int value = spinBox->value();

	model->setData(index, value, Qt::EditRole);
}
void SpinBoxDelegate::updateEditorGeometry(QWidget *editor,
	const QStyleOptionViewItem &option,
	const QModelIndex &/* index */) const
{
	editor->setGeometry(option.rect);
}
//==============================================================
int DrawShapeTable::rowCount(const QModelIndex& qidx) const {
	return drawShape.size();
}
int DrawShapeTable::columnCount(const QModelIndex& qidx)const {
	return cols;
}
QVariant DrawShapeTable::headerData(int section, Qt::Orientation orientation, int role)const {
	if (role != Qt::DisplayRole)
		return QVariant();
	if (orientation == Qt::Vertical)
		return QString("%1").arg(section + 1);
	else
	{
		switch (section)
		{
		case 0:
			return QString("Position [px]");
			break;
		case 1:
			return QString("Ruler [px]");
			break;
		case 2:
			return QString("Confidence");
			break;
		case 3:
			return QString("RANSAC iter");
			break;
		default:
			return QVariant();
		}
	}
}
QVariant DrawShapeTable::data(const QModelIndex &index, int role)const {
	if (!index.isValid())
		return QVariant();
	if (index.row() >= drawShape.size() || index.column() >= cols)
		return QVariant();
	if (role == Qt::DisplayRole || role == Qt::EditRole) {
		const int y(index.row()), x(index.column());
		shape::Shape tmpShape = this->drawShape.at(y);
		QString display;
		switch (x) {
		case 1:
			display = QString("%1").arg(tmpShape.confRef(), 0, 'g', 2);
			break;
		case 2:
			display = QString("%1").arg(tmpShape.iterRef());
			break;
		default:
			display = QString("%1").arg(tmpShape.rulerRef());
			//display = QString("(%1, %2)").arg(tmpShape.points()[0].x(), tmpShape.points()[0].y());
			break;
		}
		return display;
	}
	else
		return QVariant();
}
Qt::ItemFlags DrawShapeTable::flags(const QModelIndex& index)const {
	if (!index.isValid())
		return Qt::ItemIsEnabled;
	if (index.column() > 0)
		return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
	else
		return QAbstractItemModel::flags(index);
}

//=========CalibrationForm
CalibrationForm::CalibrationForm(QWidget* parent) :QDialog(parent) {
	this->setWindowTitle("CalibrationForm");
	cw = new CalibrationWidget(this);
	ui.setupUi(this);
	QWidget* tab0 = ui.tabWidget->widget(0);
	QVBoxLayout* tab0Layout = new QVBoxLayout;
	tab0Layout->addWidget(cw);
	tab0->setLayout(tab0Layout);
	//ui.tabWidget->setCurrentWidget(cw);
	//tabWid->setCornerWidget(cw);
	//tabWid->addTab(cw, this->tabName[0]);
	/*for (int i = 1; i != 3; ++i)
		tabWid->addTab(new QWidget(this), this->tabName[i]);

	QVBoxLayout* mainLayout = new QVBoxLayout;
	mainLayout->addWidget(tabWid);
	this->setLayout(mainLayout);*/	
}

CaptureForm::CaptureForm(QWidget* parent) :QWidget(parent) {
	ui.setupUi(this);
	this->setWindowTitle("CaptureForm");
}
CaptureForm::~CaptureForm() {
	
}
ToolForm::ToolForm(QWidget* parent) : QWidget(parent) {
	ui.setupUi(this);
	this->setWindowTitle("ToolForm");
}



