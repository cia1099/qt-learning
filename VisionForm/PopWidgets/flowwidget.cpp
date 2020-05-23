#pragma execution_character_set("utf-8")
#include "flowwidget.h"
#include <functional>
#include <QToolButton>
#include <QPainter>
#include "../MySource/ResourceManager.h"
//#pragma comment(lib, "../x64/Debug/MySource.lib") //used CMake building need command it

using namespace popwin;
using namespace std::placeholders;
using std::move;

QAction* newAction(QObject* parent, QString text, QString shortcut = "",
	QString icon = "", QString tip = "", bool checkable = false, bool enabled = true) {
	QAction* a = new QAction(text, parent);
	if (icon != "") {
		auto newIcon = [](QString ic) { return QIcon(":/" + ic); };
		a->setIcon(newIcon(icon));
	}
	if (shortcut != "") {
		a->setShortcut(QKeySequence(shortcut));
	}
	if (tip != "") {
		a->setToolTip(tip);
		a->setStatusTip(tip);
	}
	if (checkable)
		a->setCheckable(true);
	a->setEnabled(enabled);
	return a;
}

FlowWidget::FlowWidget(QWidget* parent) :QMainWindow(parent) {
	using namespace vf;
	//====Action
	//all action whose trigger still need to connect slot function
	auto action = std::bind(newAction, this, _1, _2, _3, _4, false, true);
	//auto a_capture = action(tr("Capture"), "Ctrl+C", "camera", tr("take picture"));
	//auto a_calculate = action(tr("Measurement"), "Ctrl+t", "cut", tr("measurement tools"));
	auto a_capture = action(tr("圖像採集"), "Ctrl+C", "camera", ResourceManager::str_catagory[FUNC_CATAGORY::CAPTURE]);
	auto a_calculate = action(tr("尺寸檢測"), "Ctrl+t", "cut", ResourceManager::str_catagory[FUNC_CATAGORY::SIZE_MEASURE]);
	auto a_generatePoint = action(tr("輸出設置"), "Ctrl+G", "verify", ResourceManager::str_catagory[FUNC_CATAGORY::OUTPUT]);
	auto a_imgProcess = action(tr("外觀檢測"), "", "lana", ResourceManager::str_catagory[FUNC_CATAGORY::EXTRIOR_MEASURE]);
	auto a_findRef = action(tr("模板匹配"), "", "app", ResourceManager::str_catagory[FUNC_CATAGORY::TEMPLATE_MATCH]);
	this->actList = new QList<QAction*>{ a_capture, a_calculate, a_generatePoint, a_imgProcess, a_findRef };
	//this->actAddress = new QList<QAction**>{ &a_capture, &a_calculate, &a_generatePoint, &a_imgProcess, &a_findRef };


	flowList = new QListWidget(this);
	flowList->setMovement(QListView::Snap);
	flowList->setSpacing(10);
	flowList->setAcceptDrops(true);
	flowList->setDropIndicatorShown(true);
	flowList->setDragDropMode(QAbstractItemView::InternalMove);
	
	auto* itemDelegate = new ModuleDelegate(this->flowList);
	//itemDelegate->setContentsMargins(8, 8, 8, 8);
	//itemDelegate->setIconSize(32, 32);
	//itemDelegate->setHorizontalSpacing(8);
	//itemDelegate->setVerticalSpacing(4);
	flowList->setItemDelegate(itemDelegate);

	this->setCentralWidget(flowList);
	tools = new QToolBar(this);
	tools->addActions(*actList);
	this->addToolBar(tools);

	connect(a_capture, &QAction::triggered, [&]() {this->addModule(this->actList->at(0)); });
	connect(a_calculate, &QAction::triggered, [&]() {this->addModule(this->actList->at(1)); });
	connect(a_generatePoint, &QAction::triggered, [&]() {this->addModule(this->actList->at(2)); });
	connect(a_imgProcess, &QAction::triggered, [&]() {this->addModule(this->actList->at(3)); });
	connect(a_findRef, &QAction::triggered, [&]() {this->addModule(this->actList->at(4)); });
	//for (int i = 0; i!=actList->size()-1;++i)
		//connect(actList->at(i), &QAction::triggered, [&]() {this->addModule(actList->at(i)); });
	//foreach(QAction** act, *actAddress)
		//connect(*act, &QAction::triggered, [&]() {this->addModule(*act); });
}
void FlowWidget::addModule(QAction* act) {
	QString userName = tr("計量") + QString("%1").arg(flowList->count() + 1);
	QListWidgetItem* item = new QListWidgetItem(act->icon(), userName, this->flowList);
	item->setToolTip(act->toolTip());
	//item->setSizeHint(QSize(50, 100));
	/*QToolButton *btn = new QToolButton();
	btn->setText(item->text());
	btn->setIcon(item->icon());
	btn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	flowList->setItemWidget(item, btn);*/
}

ModuleDelegate::ModuleDelegate(QObject* parent):QStyledItemDelegate(parent),
iconSize(50,50), margins(8,8,8,8),spacingHorizontal(4),spacingVertical(2){}
void ModuleDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
	QStyleOptionViewItem opt(option);
	initStyleOption(&opt, index);
	const QPalette &palette(opt.palette);
	const QRect &rect(opt.rect);
	const QRect &contentRect(rect.adjusted(this->margins.left(),
		this->margins.top(), -this->margins.right(), -this->margins.bottom()));
	const bool lastIndex = (index.model()->rowCount() - 1) == index.row();
	const bool hasIcon = !opt.icon.isNull();
	const int bottomEdge = rect.bottom();
	QFont font(opt.font);
	QSize mySize = this->iconSize;

	painter->save();
	painter->setClipping(true);
	painter->setClipRect(rect);
	painter->setFont(opt.font);
	// Draw background
	painter->fillRect(rect, opt.state & QStyle::State_Selected ?
		palette.highlight().color() :
		palette.light().color());
	// Draw bottom line
	painter->setPen(lastIndex ? palette.dark().color()
		: palette.mid().color());
	painter->drawLine(lastIndex ? rect.left() : this->margins.left(),
		bottomEdge, rect.right(), bottomEdge);
	// Draw message icon
	if (hasIcon) {
		painter->drawPixmap((contentRect.left() + contentRect.right() - this->iconSize.width())*0.5, contentRect.top()/*+this->iconSize.height()*0.5*/,
			opt.icon.pixmap(this->iconSize).scaled(this->iconSize,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
	}
	// Draw Text
	QRect btnNameRect(opt.fontMetrics.boundingRect(opt.text).adjusted(0,0,1,1));
	btnNameRect.moveTo((contentRect.left() + contentRect.right()-btnNameRect.width())*0.5,
		contentRect.top()+this->margins.top() + this->iconSize.height() + this->spacingVertical);
	painter->setFont(opt.font);
	painter->setPen(palette.windowText().color());
	painter->drawText(btnNameRect, Qt::TextSingleLine, opt.text);

	painter->restore();
}
QSize ModuleDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index)const {
	QStyleOptionViewItem opt(option);
	initStyleOption(&opt, index);
	QRect btnNameRect(opt.fontMetrics.boundingRect(opt.text).adjusted(0, 0, 1, 1));

	int textWidth = btnNameRect.width() + this->spacingHorizontal;
	int iconWidth = this->iconSize.width();
	int w = textWidth > iconWidth ? textWidth : iconWidth;
	int h = btnNameRect.height()*2.25 + this->spacingVertical + this->iconSize.height();

	return QSize(margins.right() - margins.left() + w, margins.top() - margins.bottom() + h);
}
void ModuleDelegate::setContentsMargins(int left, int top, int right, int bottom) {
	this->margins = QMargins(left, top, right, bottom);
}
void ModuleDelegate::setIconSize(int w, int h) {
	this->iconSize = QSize(w, h);
}
void ModuleDelegate::setVerticalSpacing(int spacing) { this->spacingVertical = spacing; }
void ModuleDelegate::setHorizontalSpacing(int spacing) { this->spacingHorizontal = spacing; }