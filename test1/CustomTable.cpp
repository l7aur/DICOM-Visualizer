#include "CustomTable.hpp"

namespace {
    std::vector<const char*> NON_EDITABLE_VR{"na", "SQ"};
    const char DEPTH_MARKER = '\t';

    bool isThisRowEditable(QString s) {
        for (auto& i : NON_EDITABLE_VR)
            if (s == i)
                return false;
        return true;
    }
}

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
    for (int r = 0; r < rowCount(); ++r) 
        if(isThisRowEditable(item(r, 1)->text())) {
            QTableWidgetItem* i = item(r, columnCount() - 1);
            isEditable ? makeEditable(i) : makeReadOnly(i);
        }
}

std::vector<std::pair<QString, QString>> CustomTable::getContentOfEditableCells() const
{
    const int tagColumn = 0;
    const int valueColumn = 5;

    std::vector<std::pair<QString, QString>> v;

    for (int i = 0; i < rowCount(); ++i) {
        QString tagString = item(i, tagColumn)->text();
        QString valueString = removeDepthMarkers(item(i, valueColumn)->text());
        v.push_back({ tagString, valueString });
    }
    return v;
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

QString CustomTable::removeDepthMarkers(const QString str) const
{
    QString s = str;
    if (s.size() <= 0)
        return "";
    if (s.at(0) == DEPTH_MARKER)
        s.remove(0, 1);
    return s;
}
