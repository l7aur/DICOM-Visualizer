#include "CustomTable.hpp"

CustomTable::CustomTable(unsigned int r, unsigned int c, const QStringList labels)
    : QTableWidget(r, c)
{
    this->setColumnCount(c);
    this->setRowCount(r);
    this->setHorizontalHeaderLabels(labels);
}

CustomTable::~CustomTable()
{
}

void CustomTable::insertRow(QStringList strings)
{
    if (strings.size() != columnCount()) {
        std::cerr << "WRONG NUMBER OF ITEMS TO BE PLACED IN THE TABLE!\n";
        return;
    }
    QTableWidget::insertRow(rowCount());
    for (int index = 0; index < columnCount(); ++index) {
        QTableWidgetItem* i = new QTableWidgetItem(strings.at(index));
        makeReadOnly(i);
        setItem(rowCount() - 1, index, i);
    }
}

void CustomTable::setEditabilityOfAllCells(bool isEditable)
{
    for (int r = 0; r < rowCount(); ++r) {
        QTableWidgetItem* i = item(r, columnCount() - 1);
        (isEditable) ? makeEditable(i) : makeReadOnly(i);
    }
}

void CustomTable::makeEditable(QTableWidgetItem* i)
{
    i->setFlags(i->flags() | Qt::ItemIsEditable);
    i->setBackground(Qt::green);
}

void CustomTable::makeReadOnly(QTableWidgetItem* i)
{
    i->setFlags(i->flags() & ~Qt::ItemIsEditable);
    i->setBackground(Qt::transparent);
}