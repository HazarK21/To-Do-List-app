#include "sqlinsertion.h"

SQLInsertion::SQLInsertion(QObject *parent) : QObject(parent) {
}

SQLInsertion::~SQLInsertion() {
}

void SQLInsertion::insertActive(const qint16 TaskID, const QString& taskName, const QDate& startDate, const QDate& endDate) {
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isOpen()) {
        qDebug() << "Database connection not open!";
        return;
    }

    QSqlQuery query(db);
    query.prepare("INSERT INTO ActiveTasks (TaskID, TaskName, StartDate, EndDate) VALUES (:itemID, :taskName, :startDate, :endDate)");
    query.bindValue(":itemID", TaskID);
    query.bindValue(":taskName", taskName);
    query.bindValue(":startDate", startDate.toString("yyyy-MM-dd"));
    query.bindValue(":endDate", endDate.toString("yyyy-MM-dd"));

    if (!query.exec()) {
        qDebug() << "Error:" << query.lastError().text();
    } else {
        qDebug() << "Values inserted into ActiveTasks!";
    }
}

void SQLInsertion::insertFinished(const qint16 taskID, const QString& taskName, const QDate& startDate, const QDate& endDate) {
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isOpen()) {
        qDebug() << "Database connection is not open!";
        return;
    }

    QSqlQuery query(db);
    query.prepare("INSERT INTO FinishedTasks (TaskID, TaskName, StartDate, EndDate) VALUES (:itemID, :taskName, :startDate, :endDate)");
    query.bindValue(":itemID", taskID);
    query.bindValue(":taskName", taskName);
    query.bindValue(":startDate", startDate.toString("yyyy-MM-dd"));
    query.bindValue(":endDate", endDate.toString("yyyy-MM-dd"));

    if (!query.exec()) {
        qDebug() << "Error:" << query.lastError().text();
    } else {
        qDebug() << "Values inserted into FinishedTasks!";
    }
}
