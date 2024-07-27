#ifndef DELETETASKSCLASS_H
#define DELETETASKSCLASS_H

#include <QObject>
#include <QWidget>
#include <QTableWidget>
#include "sqlinsertion.h"


class deleteTasksClass
{
public:
    deleteTasksClass();

public slots:
    void deleteTasks(QTableWidget *tableWidget, const QString &tableName);

private:

};

#endif // DELETETASKSCLASS_H
