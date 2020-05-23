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
#include "generictools.h"
#include <QListWidget>
#include <QListWidgetItem>
#include <QToolBar>
#include <QAction>
#include <QtWidgets/QMainWindow>
#include <QVariant>
#include <QStyledItemDelegate>

namespace popwin {
	class POPWIDGETS_EXPORT FlowWidget : public QMainWindow {
		Q_OBJECT
	public:
		FlowWidget(QWidget* = nullptr);
		QListWidget* flowList;
	protected:
		void addModule(QAction*);
	private:
		//QListWidget* flowList;
		QToolBar* tools;
		QList<QAction*>* actList;
		//QList<QAction**>* actAddress;
	};

	class POPWIDGETS_EXPORT ModuleDelegate : public QStyledItemDelegate {
		Q_OBJECT
	public:
		explicit ModuleDelegate(QObject* = nullptr);
		void paint(QPainter*, const QStyleOptionViewItem&, const QModelIndex&) const override;
		QSize sizeHint(const QStyleOptionViewItem&, const QModelIndex&) const override;
		inline void setContentsMargins(int, int, int, int);
		inline void setIconSize(int, int);
		inline void setHorizontalSpacing(int);
		inline void setVerticalSpacing(int);
	private:
		QSize iconSize;
		QMargins margins;
		int spacingHorizontal;
		int spacingVertical;
	};
}