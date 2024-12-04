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
        i->setFlags(i->flags() & ~Qt::ItemIsEditable);
        i->setBackground(Qt::red);
        setItem(rowCount() - 1, index, i);
    }
    std::cout << rowCount() << ' ' << columnCount() << '\n';
}

    void CustomTable::allAssignEditability()
    {
        isEditable = !isEditable;
        for (int r = 0; r < rowCount(); ++r)
            for (int c = 0; c < columnCount(); ++c) {
                QTableWidgetItem* i = item(r, c);
                if (isEditable == false) {
                    i->setFlags(i->flags() & ~Qt::ItemIsEditable);
                    i->setBackground(Qt::red);
                }
                else {
                    i->setFlags(i->flags() | Qt::ItemIsEditable);
                    i->setBackground(Qt::green);
                }

            }
    }
