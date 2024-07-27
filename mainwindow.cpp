#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QKeyEvent>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , sql(new SQLInsertion)
    , dlt(new deleteTasksClass)
{
    ui->setupUi(this);
    connect(ui->NewTaskLineEdit, &QLineEdit::returnPressed, ui->AddNewBtn, &QPushButton::click);
    connect(ui->AddNewBtn, &QPushButton::clicked, this, &MainWindow::newTask);
    // connect(ui->AddNewBtn, &QPushButton::clicked, this, &MainWindow::ResizeTables);
    connect(ui->DeleteBtn, &QPushButton::clicked, this, &MainWindow::deleteActive);
    connect(ui->DeleteBtn, &QPushButton::clicked, this, &MainWindow::deleteFinished);
    connect(ui->CheckBtn, &QPushButton::clicked, this, &MainWindow::checkTask);
    connect(ui->closeButton, &QPushButton::clicked, this, &MainWindow::close);
    ui->activeTasks->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->finishedTasks->setEditTriggers(QAbstractItemView::NoEditTriggers);
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::CustomizeWindowHint);
    ui->activeTasks->setStyleSheet("QTableCornerButton::section { background-color: #3498db; }");
    ui->finishedTasks->setStyleSheet("QTableCornerButton::section { background-color: #3498db; }");
    setFixedWidth(480);
    ui->endDate->setMinimumDate(QDate::currentDate());
    ui->endDate->setDate(QDate::currentDate());
    QString dirPath = QCoreApplication::applicationDirPath();
    QString dbPath = dirPath + "/ToDoListDatabase.db";
    QSqlDatabase mydb=QSqlDatabase::addDatabase("QSQLITE");
    mydb.setDatabaseName(dbPath);
    mydb.open();
    if(!mydb.open())
    {
        ui->ConnectionLabel->setText("<font color='red'>Failed to open the database</font>");
    }
    else
    {
        ui->ConnectionLabel->setText("<font color='green'>Connected to database</font>");
    }
    setButtonImage();
    updateTables();
    QTimer::singleShot(0, this, &MainWindow::ResizeTables); // Call after constructor finishes
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateTables()
{
    updateTable(ui->activeTasks, "ActiveTasks");
    updateTable(ui->finishedTasks, "FinishedTasks");
}

void MainWindow::updateTable(QTableWidget *tableWidget, const QString &tableName)
{
    tableWidget->clearContents();
    QSqlQuery query(QString("SELECT TaskName, StartDate, EndDate FROM %1").arg(tableName));
    int row = 0;
    while (query.next()) {
        QString taskName = query.value(0).toString();
        QDate startDate = query.value(1).toDate();
        QDate endDate = query.value(2).toDate();
        QTableWidgetItem *itemID = new QTableWidgetItem(QString::number(row + 1));
        QTableWidgetItem *itemName = new QTableWidgetItem(taskName);
        QTableWidgetItem *itemStartDate = new QTableWidgetItem(startDate.toString("yyyy-MM-dd"));
        QTableWidgetItem *itemEndDate = new QTableWidgetItem(endDate.toString("yyyy-MM-dd"));
        tableWidget->insertRow(row);
        tableWidget->setItem(row, 0, itemID);
        tableWidget->setItem(row, 1, itemName);
        tableWidget->setItem(row, 2, itemStartDate);
        tableWidget->setItem(row, 3, itemEndDate);
        ++row;

        for (int row = tableWidget->rowCount() - 1; row >= 0; --row) {
            bool rowIsEmpty = true;
            for (int col = 0; col < tableWidget->columnCount(); ++col) {
                QTableWidgetItem *item = tableWidget->item(row, col);
                if (item && !item->text().isEmpty()) {
                    rowIsEmpty = false;
                    break;
}}
            if (rowIsEmpty) {
                tableWidget->removeRow(row);
}}}}

void MainWindow::newTask()
{   qint64 TaskID = ui->activeTasks->rowCount() + 1;
    QDate date = QDate::currentDate();
    QDate EndDate = ui->endDate->date();
    QString strDate = date.toString("yyyy-MM-dd");
    QString strEndDate = EndDate.toString("yyyy-MM-dd");
    QString taskName = ui->NewTaskLineEdit->text();

    if (!taskName.isEmpty())
    {
        sql->insertActive(TaskID, taskName, date, EndDate);
        ui->activeTasks->clearContents();
        ui->activeTasks->setRowCount(0);
        updateTables();
        ui->NewTaskLineEdit->clear();
        qDebug() << "Active Tasks Row Count" << ui->activeTasks->rowCount();
        ui->warning->setText("");
    }else
    {
        ui->warning->setText("<font color='red'>Enter the task name!</font>");
    }
}

void MainWindow::deleteActive() {
    dlt->deleteTasks(ui->activeTasks, "ActiveTasks");
    qDebug() << "Active Tasks Row Count" << ui->activeTasks->rowCount();
}

void MainWindow::deleteFinished() {
    dlt->deleteTasks(ui->finishedTasks, "FinishedTasks");
    qDebug() << "Finished Tasks Row Count" << ui->finishedTasks->rowCount();
}


void MainWindow::checkTask() {
    QList<QTableWidgetItem*> activeSelectedItems = ui->activeTasks->selectedItems();
    QList<QTableWidgetItem*> finishedSelectedItems = ui->finishedTasks->selectedItems();

    if (!activeSelectedItems.isEmpty()) {
        moveRows(ui->activeTasks, ui->finishedTasks);
    }

    if (!finishedSelectedItems.isEmpty()) {
        moveRows(ui->finishedTasks, ui->activeTasks);
    }
    updateTables();
}


void MainWindow::moveRows(QTableWidget *sourceTable, QTableWidget *targetTable) {
    QList<int> selectedRows;

    foreach (QTableWidgetItem *item, sourceTable->selectedItems()) {
        int row = item->row();
        if (!selectedRows.contains(row)) {
            selectedRows.append(row);
        }
    }
    std::sort(selectedRows.begin(), selectedRows.end(), std::greater<int>());

    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isOpen()) {
        qDebug() << "No database connection!";
        return;
    }

    QString sourceTableName, targetTableName;
    if (sourceTable == ui->activeTasks) {
        sourceTableName = "ActiveTasks";
        targetTableName = "FinishedTasks";
    } else if (sourceTable == ui->finishedTasks) {
        sourceTableName = "FinishedTasks";
        targetTableName = "ActiveTasks";
    } else {
        qDebug() << "Unsupported source table!";
        return;
    }

    QSqlDatabase::database().transaction();
    int initialRowCount = targetTable->rowCount();

    foreach (int rowToMove, selectedRows) {
        QSqlQuery query(db);
        query.prepare(QString("SELECT TaskName, StartDate, EndDate FROM %1 WHERE ROWID = :taskID").arg(sourceTableName));
        query.bindValue(":taskID", sourceTable->item(rowToMove, 0)->text().toInt());

        if (query.exec() && query.next()) {
            QString taskName = query.value(0).toString();
            QDate startDate = query.value(1).toDate();
            QDate endDate = query.value(2).toDate();

            QSqlQuery insertQuery(db);
            insertQuery.prepare(QString("INSERT INTO %1 (TaskName, StartDate, EndDate) VALUES (:taskName, :startDate, :endDate)").arg(targetTableName));
            insertQuery.bindValue(":taskName", taskName);
            insertQuery.bindValue(":startDate", startDate);
            insertQuery.bindValue(":endDate", endDate);

            if (!insertQuery.exec()) {
                qDebug() << "Error inserting task into" << targetTableName << ":" << insertQuery.lastError().text();
                continue;
            }

            int newRow = initialRowCount;
            targetTable->insertRow(newRow);
            targetTable->setItem(newRow, 0, new QTableWidgetItem(QString::number(newRow)));
            targetTable->setItem(newRow, 1, new QTableWidgetItem(taskName));
            targetTable->setItem(newRow, 2, new QTableWidgetItem(startDate.toString("yyyy-MM-dd")));
            targetTable->setItem(newRow, 3, new QTableWidgetItem(endDate.toString("yyyy-MM-dd")));
            QSqlQuery updateQuery(db);
            QString updateStatement =
                QString("UPDATE %1 SET Finished = %2 WHERE ROWID = :taskID").arg(sourceTableName);
            if (sourceTableName == "ActiveTasks") {
                updateStatement.append(" AND Finished = 1");

            } else if (sourceTableName == "FinishedTasks") {
                updateStatement.append(" AND Finished = 0");
            }
            updateQuery.prepare(updateStatement);
            updateQuery.bindValue(":taskID", sourceTable->item(rowToMove, 0)->text().toInt());
            updateQuery.exec();
            QSqlQuery deleteQuery(db);
            deleteQuery.prepare(QString("DELETE FROM %1 WHERE ROWID = :taskID").arg(sourceTableName));
            deleteQuery.bindValue(":taskID", sourceTable->item(rowToMove, 0)->text().toInt());
            deleteQuery.exec();
            QSqlQuery decrementQuery(db);
            decrementQuery.prepare(QString("UPDATE %1 SET ROWID = ROWID - 1 WHERE ROWID > :taskID").arg(sourceTableName));
            decrementQuery.bindValue(":taskID", sourceTable->item(rowToMove, 0)->text().toInt());
            decrementQuery.exec();
            initialRowCount++;
            sourceTable->removeRow(rowToMove);
            qDebug() << "Active Tasks Row Count" << ui->activeTasks->rowCount();
            qDebug() << "Finished Tasks Row Count" << ui->finishedTasks->rowCount();
        } else {
            qDebug() << "Error fetching task details from" << sourceTableName << ":" << query.lastError().text();
        }
    }
    QSqlDatabase::database().commit();
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
    m_nMouseClick_X_Coordinate = event->position().x();
    m_nMouseClick_Y_Coordinate = event->position().y();
}

void MainWindow::mouseMoveEvent(QMouseEvent *event) {
    move(event->globalPosition().x() - m_nMouseClick_X_Coordinate,
         event->globalPosition().y() - m_nMouseClick_Y_Coordinate);
}

void MainWindow::ResizeTables(){
    ui->finishedTasks->setFixedHeight(130);
    ui->activeTasks->hideColumn(0);
    ui->activeTasks->setColumnWidth(1, 249);
    ui->activeTasks->setColumnWidth(2, 80);
    ui->activeTasks->setColumnWidth(3, 78);
    ui->finishedTasks->hideColumn(0);
    ui->finishedTasks->setColumnWidth(1, 249);
    ui->finishedTasks->setColumnWidth(2, 80);
    ui->finishedTasks->setColumnWidth(3, 78);
}

void MainWindow::setButtonImage(){
    ui->groupBox->setContentsMargins(0, 0, 0, 0);
    closeButtonIcon = QIcon("icons/closebutton.png");
    ui->closeButton->setIcon(this->closeButtonIcon);
    ui->closeButton->setFixedSize(40,40);
    ui->closeButton->setIconSize(QSize(40, 40));
    ui->closeButton->setStyleSheet(
        "QPushButton { border: none; background-color: transparent; }"
        "QPushButton:hover { background-color: rgba(193, 193, 193, 0.3); }"
        "QPushButton:pressed { background-color: rgba(100, 100, 100, 0.5); }"
        );
}

