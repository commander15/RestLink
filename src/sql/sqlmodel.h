#ifndef RESTLINK_SQLMODEL_H
#define RESTLINK_SQLMODEL_H

#include "sqldatabase.h"

#include <QtCore/qjsonobject.h>

#include <QtSql/qsqlquery.h>

namespace RestLink {

class SqlDatabase;

class SqlModel
{
public:
    typedef QPair<QString, QList<SqlModel>> Relation;

    struct Query {
        QStringList filters;
        QList<QPair<QString, Qt::SortOrder>> orders;
        int page = 1;
    };

    SqlModel(const QString &table, SqlDatabase *database);
    SqlModel(const SqlModel &other) = default;

    SqlModel &operator=(const SqlModel &other) = default;

    QVariant primary() const;
    void setPrimary(const QVariant &id);

    QVariant value(const QString &field);
    void setValue(const QString &field, const QVariant &value);

    QJsonObject data(bool all = true) const;

    void fill(const QJsonObject &object);
    void fill(const QSqlRecord &record);

    bool get();
    bool get(const QVariant &id);

    bool save();
    bool insert();
    bool update();

    bool deleteModel();

    QSqlQuery query() const;

    static SqlModel find(const QVariant &id, const QString &table, SqlDatabase *database);
    static QList<SqlModel> find(const Query &query, const QString &table, SqlDatabase *database);

private:
    QJsonObject m_data;
    QList<Relation> m_relations;

    QString m_table;
    QJsonObject m_configuration;

    QSqlQuery m_query;
    SqlDatabase *m_db;
};

} // namespace RestLink

#endif // RESTLINK_SQLMODEL_H
