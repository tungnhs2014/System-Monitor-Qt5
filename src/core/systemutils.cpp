/**
 * @file systemutils.cpp
 * @brief System utilities imlementation for PI 3B+ monitoring
 * @author TungNHS
 * @version 1.0.0
 */

#include "systemutils.h"
#include "constants.h"

#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QStorageInfo>
#include <QDebug>
#include <QDir>
#include <QProcess>

// ===================================================================
// FILE I/O OPERATIONS
// ===================================================================

QString SystemUtils::readFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Cannot read file: " << filePath << "Error: " << file.errorString();
        return QString();
    }

    QString content = file.readAll();
    file.close();

    return content.trimmed();
}

QStringList SystemUtils::readFileLines(const QString &filePath)
{
    QString content = readFile(filePath);
    if (content.isEmpty()) {
        return QStringList();
    }

    return content.split('\n', Qt::SkipEmptyParts);
}

bool SystemUtils::fileExists(const QString &filePath)
{
    QFile file(filePath);
    return file.exists() && file.open(QIODevice::ReadOnly);
}
