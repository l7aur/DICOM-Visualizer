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

Application::~Application() 
{
}

void Application::addStateDisplay()
{
    editLabel = new QLabel(EDIT_DISABLE_MSG.c_str());
    statBar = statusBar();
    statBar->addPermanentWidget(editLabel, 1);
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
    deleteTableContents();
    if (init) {
        currentFilePath = QString(PATH_TO_FILE_TO_BE_OPEN_AT_STARTUP.c_str());
        init = false;
    }
    else
        currentFilePath = getNewFilePath();
    fetchData();
    resizeTableCells();
}

void Application::edit()
{
    toggleEdit = !toggleEdit;
    editLabel->setText((toggleEdit) ? EDIT_ENABLE_MSG.c_str() : EDIT_DISABLE_MSG.c_str());
    table->setEditabilityOfAllCells(toggleEdit);
}

void Application::save()
{
    //todo list
    // -> add store functionality
    // -> create indentation of groups -> remove item number, make child tags have more indentation
    //                                    if below the same parent tag
}

QString Application::getNewFilePath()
{
    QString newPath = QFileDialog::getOpenFileName(this, tr("Open File"), PATH_TO_STUDIES_ROOT_FOLDER.c_str(), tr("*dcm"));
    return (newPath.isEmpty()) ? currentFilePath : newPath;
}

void Application::fetchData()
{
    int stat = fr->fopen(currentFilePath.toStdString());
    if (stat < 0) {
        COUT << "File was not opened!\n";
        return;
    }
    // fr->printFile(); /*debug*/
    QStringList rowToBeInserted{};

    std::vector<Tuple> data = fr->getAll();
    for (auto& rowData : data) {
        
        rowToBeInserted.push_back(computeTagString(rowData).c_str());
        rowToBeInserted.push_back(rowData.getVr().getVRName());
        rowToBeInserted.push_back(std::to_string(rowData.getVm()).c_str());
        rowToBeInserted.push_back(std::to_string(rowData.getLength()).c_str());
        rowToBeInserted.push_back(rowData.getDescription().c_str());
        rowToBeInserted.push_back(rowData.getValue().c_str());

        table->insertRow(rowToBeInserted);
        rowToBeInserted.clear();
    }
}

OFString Application::computeTagString(Tuple& rowData)
{
    OFString s = "";
    for (int d = 0; d < rowData.getTag().second; d++)
        s += "|>";
    s += rowData.getTag().first.toString();
    return s;
}

void Application::deleteTableContents()
{
    table->setRowCount(0);
}

void Application::resizeTableCells()
{
    table->resizeColumnsToContents();
    table->resizeRowsToContents();
}