///////////////////////////////////////////////////////////////////////////////
// Name:        src/formula/FormulaBase.cpp
// Purpose:     formula base class for stock shares
// Author:      songhuabiao
// Modified by:
// Created:     2024-07-14 12:39
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////

#ifndef FORMULA_BASE_H
#define FORMULA_BASE_H

enum class FormulaType {
    NONE,
    EMA,
};

class FormulaBase {
   public:
    FormulaBase(FormulaType type);
    virtual ~FormulaBase();
    FormulaType GetType();

   protected:
    FormulaType m_type;
};

#endif