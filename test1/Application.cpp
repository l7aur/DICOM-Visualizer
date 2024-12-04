#include "Application.hpp"
#include <QDesktopServices>
#include <QUrl>
#include <QFileDialog>
#include <iostream>
#include "Application.hpp"

Application::Application(QWidget* parent) 
    : QMainWindow(parent)
{
    setWindowDimensions(WINDOW_WIDTH, WINDOW_HEIGHT);
    table = new CustomTable(NUMBER_OF_ROWS, NUMBER_OF_COLUMNS, COLUMN_HEADER_NAMES);
    fr = new CustomFileReader();
    setCentralWidget(table);
    open();
    addMenu();
    addStateDisplay();
}

void Application::addStateDisplay()
{
    editLabel = new QLabel(EDIT_DISABLE_MSG.c_str());
    statBar = statusBar();
    statBar->addPermanentWidget(editLabel, 1);
}

Application::~Application() 
{
}

void Application::setWindowDimensions(unsigned int w, unsigned int h)
{
    this->setMinimumWidth(w);
    this->setMinimumHeight(h);
}

void Application::addMenu()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    createAction("Open", "C:\\Users\\user\\source\\repos\\test1\\test1\\open_icon.svg", &Application::open);
    createAction("Edit", "C:\\Users\\user\\source\\repos\\test1\\test1\\edit_icon.svg", &Application::edit);
    createAction("Save", "C:\\Users\\user\\source\\repos\\test1\\test1\\save_icon.svg", &Application::save);
}

void Application::createAction(std::string name, std::string iconPath, void (Application::*method)())
{
    QAction* newAct = new QAction(QIcon(iconPath.c_str()), name.c_str(), this);
    fileMenu->addAction(newAct);
    connect(newAct, &QAction::triggered, this, method);
}

void Application::open()
{
    if (init) {
        currentFilePath = QString(PATH_TO_FILE_TO_BE_OPEN_AT_STARTUP.c_str());
        init = false;
    }
    else
        currentFilePath = getNewFilePath();
    fetchData();
}

void Application::fetchData()
{
    int stat = fr->fopen(currentFilePath.toStdString());
    if (stat < 0) {
        COUT << "File was not opened!\n";
        return;
    }
    // fr->printFile(); /*debug*/
    table->insertRow({ "test1", "test2", "test3", "test4", "test5", "test6" });
    table->insertRow({ "test1", "test2", "test3", "test4", "test5", "test6" });
    table->insertRow({ "test1", "test2", "test3", "test4", "test5", "test6" });
    table->insertRow({"test1", "test2", "test3", "test4", "test5", "test6"});
}

QString Application::getNewFilePath()
{
    QString newPath = QFileDialog::getOpenFileName(this, tr("Open File"), PATH_TO_STUDIES_ROOT_FOLDER.c_str(), tr("*dcm"));
    return (newPath.isEmpty()) ? currentFilePath : newPath;
}

void Application::save()
{
}

void Application::edit()
{
    editLabel->setText((toggleEdit) ? EDIT_DISABLE_MSG.c_str() : EDIT_ENABLE_MSG.c_str());
    table->allAssignEditability();
    toggleEdit = !toggleEdit;
}
