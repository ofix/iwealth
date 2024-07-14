#include "formula/FormulaBase.h"

FormulaBase::FormulaBase(FormulaType type) : m_type(type) {
}

FormulaBase::~FormulaBase() {
}

FormulaType FormulaBase::GetType() {
    return m_type;
}
