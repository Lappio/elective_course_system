#include<QString>
#include"validators.h"
#include<qregularexpression.h>
bool validator::validate_pass(const QString pass){
    static const QRegularExpression re(
        "^(?=.*[a-z])(?=.*[A-Z])(?=.*[0-9]).{8,}$"
        );
    return re.match(pass).hasMatch();
}
bool validator::validate_user(const QString user){
    if(user.length()==0){
        return false;
    }
    return true;
}