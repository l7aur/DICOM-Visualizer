#pragma once

#include <QTableWidget>
#include <QTableWidgetItem>
#include <QColor>
#include <QBrush>
#include <iostream>
#include <vector>
#include "Tuple.hpp"

class CustomTable : public QTableWidget
{
public:
	CustomTable(unsigned int, unsigned int, const QStringList);
	~CustomTable();
	void insertRow(QStringList strings);
	void setEditabilityOfAllCells(bool isEditable);
	std::vector<Tuple> getContentOfEditableCells() const;
private:
	void makeEditable(QTableWidgetItem* i);
	void makeReadOnly(QTableWidgetItem* i);
};