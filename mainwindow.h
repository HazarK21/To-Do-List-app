#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets/QWidget>
#include <QDate>
#include <QSqlDatabase>
#include <QTableWidget>
#include "sqlinsertion.h"
#include "deletetasksclass.h"
#include <QFile>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void updateTables();
    void updateTable(QTableWidget *tableWidget, const QString &tableName);

    void checkTask();
    void newTask();
    void deleteActive();
    void deleteFinished();
    void moveRows(QTableWidget *sourceTable, QTableWidget *targetTable);

private:
    Ui::MainWindow *ui;
    QString strDate;
    QString strEndDate;
    QString taskName;
    SQLInsertion *sql;
    deleteTasksClass *dlt;
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    int m_nMouseClick_X_Coordinate;
    int m_nMouseClick_Y_Coordinate;
    void ResizeTables();
    QIcon closeButtonIcon;
    void setButtonImage();
};
#endif // MAINWINDOW_H
