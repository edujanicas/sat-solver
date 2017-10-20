#ifndef _VAR_
#define _VAR_

// ---------- Bool datatype ----------
typedef short bool;
#define unassigned 2
#define true 1
#define false 0
// -----------------------------------

typedef struct var* Var;
struct var
{
	unsigned int id;    // Unique variable > 0 (literal) id (Eg.: 1, 2, 3)
  bool sign;          // Does it have a not? (Eg.: -1)
  bool value;         // Assigned value (True, False)
};

Var VARinit(unsigned int id, bool sign)
{
    Var v = (Var)malloc(sizeof(struct var));
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

bool VARisAssigned(Var v)
{
  return (v->value != 2);
}

#endif
