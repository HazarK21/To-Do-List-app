#include "deletetasksclass.h"

deleteTasksClass::deleteTasksClass() {}

void deleteTasksClass::deleteTasks(QTableWidget *tableWidget, const QString &tableName) {
    QList<QTableWidgetItem*> selectedItems = tableWidget->selectedItems();
    if (selectedItems.isEmpty())
        return;
    QList<int> rowsToDelete;
    for (QTableWidgetItem *item : selectedItems) {
        int row = item->row();
        if (!rowsToDelete.contains(row))
            rowsToDelete.append(row);
    }

    std::sort(rowsToDelete.begin(), rowsToDelete.end(), std::greater<int>());
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isOpen()) {
        qDebug() << "No database connection!";
        return;
    }

    QSqlQuery query(db);
    query.prepare(QString("DELETE FROM %1 WHERE ROWID = (:rowID)").arg(tableName));
    QSqlQuery updateQuery(db);
    updateQuery.prepare(QString("UPDATE %1 SET ROWID = (:newRowID) WHERE ROWID = (:oldRowID)").arg(tableName));

    for (int rowToDelete : rowsToDelete) {
        QTableWidgetItem *IDItem = tableWidget->item(rowToDelete, 0);
        if (IDItem) {
            int taskID = IDItem->text().toInt();
            query.bindValue(":rowID", taskID);
            if (!query.exec()) {
                qDebug() << "Error deleting from" << tableName << ":" << query.lastError().text();
            }
        }

        tableWidget->removeRow(rowToDelete);
        for (int row = rowToDelete; row < tableWidget->rowCount(); ++row) {
            QTableWidgetItem *currentIDItem = tableWidget->item(row, 0);
            if (currentIDItem) {
                int updatedTaskID = currentIDItem->text().toInt() - 1;
                currentIDItem->setText(QString::number(updatedTaskID));
                updateQuery.bindValue(":newRowID", updatedTaskID);
                updateQuery.bindValue(":oldRowID", updatedTaskID + 1);
                if (!updateQuery.exec()) {
                    qDebug() << "Error updating task ID in" << tableName << ":" << updateQuery.lastError().text();
                }
            }
        }
    }
}
