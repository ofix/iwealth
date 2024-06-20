///////////////////////////////////////////////////////////////////////////////
// Name:        iwealth/src/stock/Stock.cpp
// Purpose:     iwealth stock shares' core structures
// Author:      songhuabiao
// Created:     2024-05-15 10:58
// Copyright:   (C) Copyright 2024, Wealth Corporation, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////

#include "stock/Stock.h"
Share::Share()
    : id(0),
      code(""),
      name(""),
      old_names(std::vector<std::string>()),
      change_amount(0.0),
      change_rate(0.0),
      volume(0),
      amount(0),
      price_yesterday_close(0.0),
      price_now(0.0),
      price_max(0.0),
      price_min(0.0),
      price_open(0.0),
      price_close(0.0),
      price_amplitude(0.0),
      qrr(0.0),
      pe(0.0),
      pb(0.0),
      roe(0.0),
      turnover_rate(0.0),
      revenue(0.0),
      bonus(0.0),
      history_bonus(0.0),
      history_fund(0.0),
      operation_analysis(""),
      total_capital(0),
      trade_capital(0),
      industry(nullptr),
      industry_name(""),
      market(Market::ShangHai),
      province("") {
}

Share::Share(const Share& other)
    : id(other.id),
      code(other.code),
      name(other.name),
      change_amount(other.change_amount),
      change_rate(other.change_rate),
      volume(other.volume),
      amount(other.amount),
      price_yesterday_close(other.price_yesterday_close),
      price_now(other.price_now),
      price_max(other.price_max),
      price_min(other.price_min),
      price_open(other.price_open),
      price_close(other.price_close),
      price_amplitude(other.price_amplitude),
      qrr(other.qrr),
      pe(other.pe),
      pb(other.pb),
      roe(other.roe),
      turnover_rate(other.turnover_rate),
      revenue(other.revenue),
      bonus(other.bonus),
      history_bonus(other.history_bonus),
      history_fund(other.history_fund),
      operation_analysis(other.operation_analysis),
      total_capital(other.total_capital),
      trade_capital(other.trade_capital),
      industry(other.industry),
      industry_name(other.industry_name),
      market(other.market),
      province(other.province) {
    for (size_t i = 0; i < other.old_names.size(); i++) {
        old_names.push_back(other.old_names[i]);
    }
}

bool Share::operator<(const Share& other) const {
    return code < other.code;
}
