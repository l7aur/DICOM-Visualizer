#include "Application.hpp"

namespace {
    const unsigned int WINDOW_WIDTH = 1080;
    const unsigned int WINDOW_HEIGHT = 720;
    const QStringList COLUMN_HEADER_NAMES{ "TAG NAME", "VR", "VM", "LENGTH", "DESCRIPTION", "VALUE" };
    const unsigned int NUMBER_OF_ROWS = 0;
    const unsigned int NUMBER_OF_COLUMNS = COLUMN_HEADER_NAMES.size();
    const std::string PATH_TO_FILE_TO_BE_OPEN_AT_STARTUP{ "C:\\PXD\\Studii\\zzTest1^zzTest2 2024-12-02.12_47_55\\ST0\\SE0\\1.3.12.2.1107.5.1.4.65117.30000020080305334601100000687.dcm" };
    const std::string PATH_TO_STUDIES_ROOT_FOLDER{ "C:\\PXD\\Studii\\zzTest1^zzTest2 2024-12-02.12_47_55\\ST0" };
    const std::string EDIT_ENABLE_MSG{ "EDITING HAS BEEN ENABLED" };
    const std::string EDIT_DISABLE_MSG{ "EDITING IS DISABLED" };
    const char DEPTH_MARKER = '\t';
}

Application::Application(QWidget* parent) 
    : QMainWindow(parent)
{
    setWindowDimensions(WINDOW_WIDTH, WINDOW_HEIGHT);
    table = new CustomTable(NUMBER_OF_ROWS, NUMBER_OF_COLUMNS, COLUMN_HEADER_NAMES);
    fr = new CustomFileReaderWriter();
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

/**
 * todos.
 *  > clean code
 *  > testing
 *  > force insert sequence end to make overall logic simpler
 *  > update the save() method logic - remove the \t check
 *  > maybe add 'Save as ...' functionality
 */

void Application::save()
{
    if (!toggleEdit && table->rowCount() > 0)
        return;
    std::vector<std::pair<QString, QString>> tableData = table->getContentOfEditableCells();
    std::stack<DcmTag> sequenceTag;
    for (int index = 0; index < data.size(); ++index)
    {
        if (!strcmp(data[index].getVr().getVRName(), "SQ")) {
            sequenceTag.push(data[index].getTag().first);
            continue;
        }
        if (!strcmp(data[index].getVr().getVRName(), "na"))
            continue;
        if (tableData[index].first.at(0) != '\t' && !sequenceTag.empty())
            sequenceTag.pop();
        OFString tableValueOfString = tableData[index].second.toStdString().c_str();
        if (!(data[index].value == tableValueOfString)) {
            if (sequenceTag.empty())
                fr->writeValueAtTag(data[index].getTag().first, tableValueOfString);
            else
                fr->writeValueAtTag(sequenceTag.top(), data[index].getTag().first, tableValueOfString); /*test*/
        }
    }
    edit();
    fr->saveOnDisk("C:\\Users\\user\\Desktop\\edited.dcm"); /*path for debug*/
    //fr->saveOnDisk(currentFilePath);
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

    data = fr->getAll();
    for (auto& i : data) {
        //if (strcmp(i.getVr().getVRName(), "UN")) { //remove unknown tags
            rowToBeInserted.push_back(computeTagString(i).c_str());
            rowToBeInserted.push_back(i.getVr().getVRName());
            rowToBeInserted.push_back(std::to_string(i.getVm()).c_str());
            rowToBeInserted.push_back(std::to_string(i.getLength()).c_str());
            rowToBeInserted.push_back(i.getDescription().c_str());
            rowToBeInserted.push_back(i.getValue().c_str());

            table->insertRow(rowToBeInserted);
            rowToBeInserted.clear();
        //}
    }
}

OFString Application::computeTagString(Tuple& rowData)
{
    OFString s = "";
    for (int d = 0; d < rowData.getTag().second; d++)
        s += DEPTH_MARKER;
    s += rowData.getTag().first.toString();
    return s;
}

const OFString Application::removeMarkers(std::string s)
{
    if (s.size() <= 0)
        return "";
    if (s.at(0) == DEPTH_MARKER)
        s.erase(0, 1);
    return (OFString)(s.c_str());
}

QString Application::getNewFilePath()
{
    QString newPath = QFileDialog::getOpenFileName(this, tr("Open File"), PATH_TO_STUDIES_ROOT_FOLDER.c_str(), tr("*dcm"));
    return (newPath.isEmpty()) ? currentFilePath : newPath;
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
