#include "sqllog.h"

#include <QtCore/qtextstream.h>

SqlLog::SqlLog()
{
    s_logs.append(this);
}

SqlLog::~SqlLog()
{
    s_logs.removeOne(this);
}

bool SqlLog::isLoggingEnabled()
{
    return s_enabled;
}

void SqlLog::enableLogging()
{
    s_enabled = true;
}

void SqlLog::disableLogging()
{
    s_enabled = false;
}

void SqlLog::log(const QString &statement)
{
    if (!s_enabled)
        return;

    for (SqlLog *log : std::as_const(s_logs))
        log->append(statement);

    static QTextStream out(stdout);
    out << statement << Qt::endl;
}

void SqlLog::logError(const QString &str)
{
    static QTextStream out(stdout);
    out << "-> " << str << Qt::endl;
}

bool SqlLog::s_enabled(true);
QList<SqlLog *> SqlLog::s_logs;
