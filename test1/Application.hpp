#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_test1.h"
#include "CustomTable.hpp"
#include "CustomFileReaderWriter.hpp"
#include <QTableWidget>
#include <QTableWidgetItem>
#include <string>
#include <QIcon>
#include <QLabel>
#include <QDesktopServices>
#include <QUrl>
#include <QFileDialog>
#include <iostream>
#include <stack>


class Application : public QMainWindow
{
    Q_OBJECT

public:
    Application(QWidget *parent = nullptr);
    ~Application();

    void addStateDisplay();
private:
    void setWindowDimensions(unsigned int, unsigned int);
    void addMenu();
    void createAction(std::string name, std::string iconPath, void (Application::*method)());
    void open();
    void edit();
    void save();
    void fetchData();
    OFString computeTagString(Tuple& rowData);
    const OFString removeMarkers(std::string s);
    QString getNewFilePath();
    void deleteTableContents();
    void resizeTableCells();

    Ui::test1Class ui;
    CustomTable* table;
    CustomFileReaderWriter* reader;
    QMenu* fileMenu;
    QStatusBar* statBar;
    QString currentFilePath;
    QLabel* editLabel;
    CustomFileReaderWriter* fr;
    std::vector<Tuple> data;
    bool init{ true };
    bool toggleEdit{ false };
};
