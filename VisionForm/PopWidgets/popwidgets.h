#pragma once

#include "popwidgets_global.h"
#include <memory>
#include <string>
#include <QWidget>
#include <QAbstractTableModel>
#include <QItemDelegate>
#include <QTableView>
#include <QLabel>
#include <QSpinBox>
#include <QPushButton>
#include <QScrollArea>
#include <QGroupBox>
#include <QTreeWidgetItem>
#include <QDialog>
#include "generictools.h"
#include "../Shape/shape.h"
#ifdef POPWIDGETS_LIB
#include "ui_CaptureForm.h"
#include "ui_ToolForm.h"
#include "ui_ResultPreviewForm.h"
#include "ui_CalibrationForm.h"
#include "ui_ComputeToolForm.h"
#include "ui_DialogButtonRight.h"
#endif
// in the VisionForm.proj these ui.h are not visible, make program cannot release memory normally 

namespace popwin {
	
	class POPWIDGETS_EXPORT SampleTable :public QAbstractTableModel {
		Q_OBJECT
	public:
		SampleTable(QObject* =nullptr);
		int rowCount(const QModelIndex& = QModelIndex()) const override;
		int columnCount(const QModelIndex& = QModelIndex()) const override;
		QVariant data(const QModelIndex &index, int role) const override;
		QVariant headerData(int section, Qt::Orientation orientation, int role /* = Qt::DisplayRole */) const override;
		Qt::ItemFlags flags(const QModelIndex&) const override;
		bool setData(const QModelIndex &index, const QVariant &value, int role /* = Qt::EditRole */) override;
		bool insertRows(int row, int count, const QModelIndex &parent /* = QModelIndex() */) override;
		bool removeRows(int row, int count, const QModelIndex &parent /* = QModelIndex() */) override;
		Matqt_<float> getTableData()const;
	private:
		QList<std::shared_ptr<std::string> > tableData;
		const int cols = 4;
	};

	class POPWIDGETS_EXPORT SpinBoxDelegate : public QItemDelegate
	{
		Q_OBJECT
	public:
		explicit SpinBoxDelegate(QObject *parent = 0, int=-99999, int=99999);
		QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
			const QModelIndex &index) const;
		void setEditorData(QWidget *editor, const QModelIndex &index) const;
		void setModelData(QWidget *editor, QAbstractItemModel *model,
			const QModelIndex &index) const;
		void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
			const QModelIndex &index) const;
	private:
		int min_value, max_value;
	};

	class POPWIDGETS_EXPORT TableViewer :public QTableView {
		Q_OBJECT
	public:
		TableViewer(QWidget* parent=0):QTableView(parent){}
	protected:
		int sizeHintForColumn(int column) const override { return 110; }
	};

	class POPWIDGETS_EXPORT CalibrationWidget : public QWidget {
		Q_OBJECT
#define GROUP_NUM 5
#define BUTTON_NUM 5
#define SPIN_NUM 4
	public:
		enum SPINBOX_IDX{PIX_X=0,PIX_Y,MC_X,MC_Y};
		enum DISPLAY_IDX{VAILID_X=0,VAILID_Y,NEW_X,NEW_Y,NEW_RAD,RESULT_X,RESULT_Y,RESULT_ANGLE,STATUS};
		CalibrationWidget(QWidget* =0);
	protected:
		QString GroupName[GROUP_NUM] = {tr("數據採樣"),tr("數據驗證"),tr("位置補償"),tr("新建坐標系"),tr("狀態")};
		QString BtnName[BUTTON_NUM] = { "Insert Row","Delete Row","Get ref. Points","Calculate Affine","Save Affine" };
		void insertRowEvent();
		void deleteRowEvent();
		void estimateAffine2D();
		void affineTransform();
		void saveAffineMatrix();
	private:
		SampleTable* sample;
		SpinBoxDelegate* delegate;
		TableViewer* tableView;
		QSharedPointer<QSpinBox> spinArray;
		QSharedPointer<QLabel> display, second_group_label, third_group_label,fourth_group_label,fifth_group_label;
		QPushButton *tranform_coordinate;
		static QSize frameSize;
		static QSize btnSize;
		static const int labelWidth;
		QGroupBox* groupBox[GROUP_NUM];
		QSharedPointer<QPushButton> btn;
		QGroupBox *groupBox0, *groupBox1, *groupBox2, *groupBox3, *groupBox4;
		//float affineMatrix[6];
		Matqt_<float> affineMatrix;
	signals:
		void transportAffineMatrix(Matqt_<float>);
	};

	//===================================================================
	class POPWIDGETS_EXPORT DrawShapeTable :public QAbstractTableModel {
		Q_OBJECT
	public:
		QList<shape::Shape> drawShape;
		DrawShapeTable(QObject* = nullptr) {}
		int rowCount(const QModelIndex& = QModelIndex()) const override;
		int columnCount(const QModelIndex& = QModelIndex()) const override;
		QVariant data(const QModelIndex &index, int role) const override;
		QVariant headerData(int section, Qt::Orientation orientation, int role /* = Qt::DisplayRole */) const override;
		Qt::ItemFlags flags(const QModelIndex&) const override;
		//bool setData(const QModelIndex &index, const QVariant &value, int role /* = Qt::EditRole */) override;
		//bool insertRows(int row, int count, const QModelIndex &parent /* = QModelIndex() */) override;
		//bool removeRows(int row, int count, const QModelIndex &parent /* = QModelIndex() */) override;
	private:
		//QList<std::shared_ptr<std::string> > drawShape;
		const int cols = 3;
	};
	//===========functions
	//POPWIDGETS_EXPORT Matqt_<float> toHomogeneous2D(const QVector<QPointF>&);

	//class POPWIDGETS_EXPORT CalibrationForm : public QWidget {
	//public:
	//	CalibrationForm(QWidget* =0);
	//protected:
	//	QString tabName[3] = { tr("相機和運動標定"), tr("雙目視覺標定"), tr("多相機標定") };
	//private:
	//	QTabWidget* tabWid;
	//	CalibrationWidget* cw;
	//};

	class POPWIDGETS_EXPORT CaptureForm : public QWidget {
	public:
		CaptureForm(QWidget* = 0);
		~CaptureForm();
	private:
#ifdef POPWIDGETS_LIB
		Ui::CaptureUI ui;
#endif
	};
	class POPWIDGETS_EXPORT ToolForm : public QWidget {
	public:
		ToolForm(QWidget* = 0);
	private:
#ifdef POPWIDGETS_LIB
		Ui::ComputeToolUI ui;
#endif
	};

	class POPWIDGETS_EXPORT ResultPreviewForm : public QWidget {
		Q_OBJECT
	public:
		ResultPreviewForm(QWidget* = 0);
	protected:
		void onClickGenerate();
		void updateTreeView(QString);
		void onItemCheckState(QTreeWidgetItem*, int);
		void onClickReflesh();
	private:
#ifdef POPWIDGETS_LIB
		Ui::ResultPreviewUI ui;
#endif
		QStandardItemModel* listModel;
	};

	class POPWIDGETS_EXPORT CalibrationForm : public QDialog {
		Q_OBJECT
	public:
		CalibrationForm(QWidget* = 0);
	private:
#ifdef POPWIDGETS_LIB
		Ui::CalibrationDialog ui;
#endif
		CalibrationWidget* cw;
	};
}//popwin
