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
	void allAssignEditability();
private:
	bool isEditable{ false };
	QBrush editableBrush;
	QBrush nonEditableBrush;
};