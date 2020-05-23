#include "popwidgets.h"
#include <QJsonValue>
#include <QJsonObject>
#include <random>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <QPushButton>

using namespace popwin;
using namespace std;

ResultPreviewForm::ResultPreviewForm(QWidget *parent) :QWidget(parent) {
	ui.setupUi(this);
	listModel = new QStandardItemModel(this);
	ui.listView->setModel(listModel);
	connect(ui.pushButton_2, &QPushButton::clicked, [&]() {this->onClickGenerate(); });
	connect(ui.treeWidget, &QTreeWidget::itemChanged, this, &ResultPreviewForm::onItemCheckState);
	connect(ui.pushButton, &QPushButton::clicked, this, &ResultPreviewForm::onClickReflesh);
}

void ResultPreviewForm::onClickGenerate(){
	//unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine genrnd(time(nullptr));
	uniform_real_distribution<double> unif_drnd(0.0, 100.0);
	QJsonObject root, member;
	const string keys_str[] = { "point", "line", "circle", "rectangle" };
	const int N = sizeof(keys_str) / sizeof(*keys_str);
	for (int i = 0; i != sizeof(keys_str) / sizeof(*keys_str); ++i)
		member[keys_str[i].c_str()] = "";
	
	if (N != 0) {
		uniform_int<int> unif_irnd(0, N-1);
		int key_idx = unif_irnd(genrnd);
		//member[keys_str[key_idx].c_str()] = to_string(round(unif_drnd(genrnd)*1e3) / 1e3).c_str();//round(unif_drnd(genrnd)*1e3)/1e3
		stringstream ss;
		ss << setprecision(3) << fixed << unif_drnd(genrnd);
		member[keys_str[key_idx].c_str()] = ss.str().c_str();
		QString m_name = QString(keys_str[key_idx].c_str()) + QString("_%1").arg(key_idx);
		root.insert(m_name, member);
		QJsonDocument djson(root);
		updateTreeView(djson.toJson());
	}
	//ref. https://forum.qt.io/topic/65874/create-json-using-qjsondocument/3
}

void ResultPreviewForm::updateTreeView(QString jsonStr) {
	QJsonDocument d = QJsonDocument::fromJson(jsonStr.toUtf8());
	QJsonObject root = d.object();
	foreach(QString member_name, root.keys()) {
		QJsonValue val = root.value(member_name);
		QJsonObject member = val.toObject();
		QTreeWidgetItem *m_item = new QTreeWidgetItem(ui.treeWidget);
		m_item->setText(0, member_name);
		m_item->setCheckState(0, Qt::CheckState::Unchecked);
		foreach(QString key, member.keys()) {
			QTreeWidgetItem *key_item = new QTreeWidgetItem(m_item);
			key_item->setText(0, key);
			key_item->setCheckState(0, Qt::CheckState::Unchecked);
			if (member[key].toString() != "")
				key_item->setData(0, Qt::ToolTipRole, member.value(key).toString());
		}
	}
}

void ResultPreviewForm::onItemCheckState(QTreeWidgetItem* currentItem, int col){
	QTreeWidgetItem* parent = currentItem->parent();
	if (parent) {
		int my_check(0);
		for (int i = 0; i != parent->childCount(); ++i) {
			my_check += parent->child(i)->checkState(col);
		}
		parent->setCheckState(col, my_check > 0 ? Qt::PartiallyChecked : Qt::Unchecked);
	}
	/*else if(currentItem != ui.treeWidget->invisibleRootItem()) {
		for (int i = 0; i != currentItem->childCount(); ++i)
			currentItem->child(i)->setCheckState(col, currentItem->checkState(col));
	}*/
}

void ResultPreviewForm::onClickReflesh() {
	listModel->clear();
	QTreeWidgetItem* root = ui.treeWidget->invisibleRootItem();
	for (int i = 0; i != root->childCount(); ++i) {
		QTreeWidgetItem* parent = root->child(i);
		if (parent->checkState(0)) {
			for (int j = 0; j != parent->childCount(); ++j) {
				QTreeWidgetItem* child = parent->child(j);
				if (child->checkState(0) && child->toolTip(0) != "") {
					QString itemText = parent->text(0) + ":" + child->toolTip(0);
					listModel->appendRow(new QStandardItem(itemText));
				}
			}
		}
	}
}