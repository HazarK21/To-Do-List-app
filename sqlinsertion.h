#ifndef SQLINSERTION_H
#define SQLINSERTION_H

#include <QObject>
#include <QWidget>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QDate>
#include <QSqlError>


class SQLInsertion : public QObject
{
    Q_OBJECT
public:
    explicit SQLInsertion(QObject *parent = nullptr);
    ~SQLInsertion();

signals:

public slots:
    void insertActive(const qint16 TaskID, const QString& taskName, const QDate& startDate, const QDate& endDate);
    void insertFinished(const qint16 TaskID, const QString& taskName, const QDate& startDate, const QDate& endDate);

};

#endif // SQLINSERTION_H
