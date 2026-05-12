#include "passwordutils.h"
#include <QUuid>
#include <QCryptographicHash>
QString PasswordUtils::makeSalt(){
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}
QString PasswordUtils::hashPassword(const QString &password, const QString &salt)
{
    QByteArray data = (salt + password).toUtf8();

    QByteArray hash = QCryptographicHash::hash(
        data,
        QCryptographicHash::Sha256
        );

    return hash.toHex();
}
//校验密码
bool PasswordUtils::verifyPassword(const QString &password,
                                   const QString &salt,
                                   const QString &savedHash){
    QString inputHash = hashPassword(password, salt);
    return inputHash == savedHash;
}