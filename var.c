#include "var.h"

Var VARinit(unsigned int id, bool sign) {
    Var v = (Var) malloc(sizeof(struct var));
    v->id = id;
    v->sign = sign;
    v->value = unassigned;
    return v;
}

bool VARgetValue(Var v)
// Only can be called if variable is assigned
{
    if (v->sign) {
        return v->value;
    }
    return !(v->value);
}

bool VARisAssigned(Var v) {
    return (v->value != 2);
}

bool VARisPositive(Var v) {
    return (v->sign);
}

Var neg(Var p) {
    bool newSign;
    if (p->sign == 1) {
        newSign = 0;
    } else {
        newSign = 1;
    }

    Var output = VARinit(p->id, newSign);
    return output;
}