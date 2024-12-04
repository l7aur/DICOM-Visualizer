#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_test1.h"
#include "CustomTable.hpp"
#include "CustomFileReader.hpp"
#include <QTableWidget>
#include <QTableWidgetItem>
#include <string>
#include <QIcon>
#include <QLabel>

const unsigned int WINDOW_WIDTH = 1080;
const unsigned int WINDOW_HEIGHT = 720;
const QStringList COLUMN_HEADER_NAMES{ "TAG NAME", "VR", "VM", "LENGTH", "DESCRIPTION", "VALUE"};
const unsigned int NUMBER_OF_ROWS = 0;
const unsigned int NUMBER_OF_COLUMNS = COLUMN_HEADER_NAMES.size();
const std::string PATH_TO_FILE_TO_BE_OPEN_AT_STARTUP{ "C:\\PXD\\Studii\\zzTest1^zzTest2 2024-12-02.12_47_55\\ST0\\SE3\\1.3.12.2.1107.5.1.4.65117.30000020080305334601100000869.dcm" };
const std::string PATH_TO_STUDIES_ROOT_FOLDER{ "C:\\PXD\\Studii\\zzTest1^zzTest2 2024-12-02.12_47_55\\ST0" };
const std::string EDIT_ENABLE_MSG{ "EDITING HAS BEEN ENABLED" };
const std::string EDIT_DISABLE_MSG{ "EDITING IS DISABLED" };

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
    QString getNewFilePath();
    void fetchData();

    Ui::test1Class ui;
    CustomTable* table;
    CustomFileReader* reader;
    QMenu* fileMenu;
    QStatusBar* statBar;
    QString currentFilePath;
    QLabel* editLabel;
    CustomFileReader* fr;
    bool init{ true };
    bool toggleEdit{ false };
};
