#ifndef SQLLOG_H
#define SQLLOG_H

#include <QtCore/qstringlist.h>

class SqlLog : public QStringList
{
public:
    SqlLog();
    ~SqlLog();

    static bool isLoggingEnabled();
    static void enableLogging();
    static void disableLogging();
    static void log(const QString &statement);
    static void logError(const QString &str);

private:
    static bool s_enabled;
    static QList<SqlLog *> s_logs;
};

#endif // SQLLOG_H
