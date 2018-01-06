#include <QTreeView>
#include <QHeaderView>
#include <QAbstractItemView>
#include <QStandardItemModel>

#include "world.h"
#include "engine.h"
#include "hierarchy.h"

Hierarchy* Hierarchy::get() {
	static Hierarchy instance;
	return &instance;
}

Hierarchy::Hierarchy() : model_(nullptr) {
}

Hierarchy::~Hierarchy() {
}

void Hierarchy::initialize() {
	if (model_ != nullptr) { delete model_; }
	model_ = new QStandardItemModel(view_);
	
	tree_ = view_->findChild<QTreeView*>("tree", Qt::FindDirectChildrenOnly);
	tree_->setModel(model_);
	tree_->setHeaderHidden(true);
	view_->setSize(80, 200);

	connect(tree_->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), 
		this, SLOT(onSpriteClicked(const QItemSelection&, const QItemSelection&)));
}

void Hierarchy::update(Sprite root) {
	model_->setRowCount(0);
	
	updateRecursively(root, nullptr);

	tree_->header()->resizeSections(QHeaderView::ResizeToContents);
}

Sprite Hierarchy::selectedSprite() {
	QModelIndex index = tree_->selectionModel()->currentIndex();
	if (!index.isValid()) { return nullptr; }

	uint id = model_->itemFromIndex(index)->data().toUInt();
	return WorldInstance()->GetSprite(id);
}

bool Hierarchy::selectedSprites(QList<Sprite>& sprites) {
	QModelIndexList indexes = tree_->selectionModel()->selectedIndexes();

	foreach (QModelIndex index, indexes) {
		uint id = model_->itemFromIndex(index)->data().toUInt();
		sprites.push_back(WorldInstance()->GetSprite(id));
	}

	return !sprites.empty();
}

void Hierarchy::onSpriteClicked(const QItemSelection& selected, const QItemSelection& deselected) {
	EnableItemsOutline(selected, true);
	EnableItemsOutline(deselected, false);
}

void Hierarchy::updateRecursively(Sprite pp, QStandardItem* pi) {
	for (int i = 0; i < pp->GetChildCount(); ++i) {
		Sprite child = pp->GetChildAt(i);

		QStandardItem* item = new QStandardItem(child->GetName().c_str());
		item->setData(child->GetInstanceID());

		if (pi != nullptr) {
			pi->appendRow(item);
		}
		else {
			model_->appendRow(item);
		}
		
		updateRecursively(child, item);
	}
}

void Hierarchy::EnableSpriteOutline(Sprite sprite, bool enable) {
	if (!sprite || !sprite->GetRenderer()) {
		return;
	}

	for (int i = 0; i < sprite->GetRenderer()->GetMaterialCount(); ++i) {
		Material material = sprite->GetRenderer()->GetMaterial(i);
		if (enable) {
			material->EnablePass("Outline");
		}
		else {
			material->DisablePass("Outline");
		}
	}
}

void Hierarchy::EnableItemsOutline(const QItemSelection& items, bool enable) {
	foreach(QItemSelectionRange r, items) {
		foreach(QModelIndex index, r.indexes()) {
			uint id = model_->itemFromIndex(index)->data().toUInt();
			Sprite sprite = WorldInstance()->GetSprite(id);
			EnableSpriteOutline(sprite, enable);
		}
	}
}
