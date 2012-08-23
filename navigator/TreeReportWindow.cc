#include "TreeReportWindow.h"
#include "ui_TreeReportWindow.h"
#include "TreeReport.h"
#include <QDebug>
#include <QKeySequence>
#include <QShortcut>
#include <QStringList>
#include <QTreeWidgetItem>
#include <cstdlib>

namespace Nav {

TreeReportWindow::TreeReportWindow(TreeReport *treeReport, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TreeReportWindow),
    treeReport(treeReport)
{
    ui->setupUi(this);

    setWindowTitle(treeReport->getTitle());

    // Register Ctrl+Q.
    QShortcut *shortcut = new QShortcut(QKeySequence("Ctrl+Q"), this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(close()));

    // Populate the tree widget.
    ui->treeWidget->setHeaderLabels(treeReport->getColumns());
    ui->treeWidget->addTopLevelItems(
                createChildTreeWidgetItems(TreeReport::Index()));

    // Fixup the column widths.
    for (int i = 0; i < ui->treeWidget->columnCount(); ++i) {
        ui->treeWidget->resizeColumnToContents(i);
        ui->treeWidget->setColumnWidth(
                    i, ui->treeWidget->columnWidth(i) + 10);
    }

    // Sort the tree view arbitrarily.
    for (int i = 0; i < ui->treeWidget->columnCount(); ++i) {
        ui->treeWidget->sortByColumn(i, Qt::AscendingOrder);
    }
    ui->treeWidget->sortByColumn(0, Qt::AscendingOrder);
    ui->treeWidget->setSortingEnabled(true);

    // TODO: This code uses the font height as an approximation for the tree
    // widget row height.  That's not quite right, but it seems to work OK.
    // Do something better?
    // TODO: The rowCount() code won't work for trees.
    int preferredSize = ui->treeWidget->fontMetrics().height() * ui->treeWidget->model()->rowCount();
    resize(width(), height() + preferredSize);
}

QList<QTreeWidgetItem*> TreeReportWindow::createChildTreeWidgetItems(
        const TreeReport::Index &parent)
{
    int childCount = treeReport->getChildCount(parent);
    QList<QTreeWidgetItem*> result;
    for (int i = 0; i < childCount; ++i) {
        result << createTreeWidgetItem(treeReport->getChildIndex(parent, i));
    }
    return result;
}

QTreeWidgetItem *TreeReportWindow::createTreeWidgetItem(
        const TreeReport::Index &index)
{
    TreeWidgetItem *item = new TreeWidgetItem();
    QList<QVariant> data = treeReport->getText(index);
    for (int col = 0; col < data.size(); ++col) {
        item->setData(col, Qt::DisplayRole, data[col]);
    }
    item->addChildren(createChildTreeWidgetItems(index));
    item->treeReportIndex = index;
    return item;
}

TreeReportWindow::~TreeReportWindow()
{
    delete ui;
    delete treeReport;
}

void TreeReportWindow::on_treeWidget_itemSelectionChanged()
{
    QList<QTreeWidgetItem*> selection = ui->treeWidget->selectedItems();
    if (selection.size() == 1) {
        treeReport->select(
            static_cast<TreeWidgetItem*>(selection[0])->treeReportIndex);
    }
}

void TreeReportWindow::on_treeWidget_itemActivated(QTreeWidgetItem *item, int column)
{
    treeReport->activate(static_cast<TreeWidgetItem*>(item)->treeReportIndex);
}

} // namespace Nav