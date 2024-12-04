#pragma once

#include <QTableWidget>
#include <QTableWidgetItem>
#include <QColor>
#include <QBrush>
#include <iostream>

class CustomTable : public QTableWidget
{
public:
	CustomTable(unsigned int, unsigned int, const QStringList);
	~CustomTable();
	void insertRow(QStringList strings);
	void setEditabilityOfAllCells(bool isEditable);
private:
	void makeEditable(QTableWidgetItem* i);
	void makeReadOnly(QTableWidgetItem* i);

	QBrush editableBrush;
	QBrush nonEditableBrush;
};