#pragma once
#include <QString>
/*密码的加密
加盐+哈希
校验密码*/
class PasswordUtils{
public:
    static QString makeSalt();
    static QString hashPassword(const QString &password, const QString &salt);
    static bool verifyPassword(const QString &password,
                               const QString &salt,
                               const QString &savedHash);
};