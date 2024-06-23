///////////////////////////////////////////////////////////////////////////////
// Name:        iwealth/src/stock/Stock.cpp
// Purpose:     iwealth stock shares' core structures
// Author:      songhuabiao
// Created:     2024-05-15 10:58
// Copyright:   (C) Copyright 2024, songhuabiao, All Rights Reserved.
// Licence:     GNU GENERAL PUBLIC LICENSE, Version 3
///////////////////////////////////////////////////////////////////////////////

#include "stock/Stock.h"

Share::Share()
    : id(0),
      code(""),
      name(""),
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
      province(""),
      ptr_basic_info(nullptr) {
}

Share& Share::operator=(const Share& other) {
    this->id = other.id;
    this->code = other.code;
    this->name = other.name;
    this->change_amount = other.change_amount;
    this->change_rate = other.change_rate;
    this->volume = other.volume;
    this->amount = other.amount;
    this->price_yesterday_close = other.price_yesterday_close;
    this->price_now = other.price_now;
    this->price_max = other.price_max;
    this->price_min = other.price_min;
    this->price_open = other.price_open;
    this->price_close = other.price_close;
    this->price_amplitude = other.price_amplitude;
    this->qrr = other.qrr;
    this->pe = other.pe;
    this->pb = other.pb;
    this->roe = other.roe;
    this->turnover_rate = other.turnover_rate;
    this->revenue = other.revenue;
    this->bonus = other.bonus;
    this->history_bonus = other.history_bonus;
    this->history_fund = other.history_fund;
    this->operation_analysis = other.operation_analysis;
    this->total_capital = other.total_capital;
    this->trade_capital = other.trade_capital;
    this->industry = other.industry;
    this->industry_name = other.industry_name;
    this->market = other.market;
    this->province = other.province;
    if (other.ptr_basic_info != nullptr) {
        ptr_basic_info->capital_change_history = other.ptr_basic_info->capital_change_history;
        ptr_basic_info->company_name = other.ptr_basic_info->company_name;
        ptr_basic_info->core_bussiness = other.ptr_basic_info->core_bussiness;
        ptr_basic_info->history_bonus = other.ptr_basic_info->history_bonus;
        ptr_basic_info->holder_change_history = other.ptr_basic_info->holder_change_history;
        ptr_basic_info->ipo_date = other.ptr_basic_info->ipo_date;
        ptr_basic_info->share_investment_funds = other.ptr_basic_info->share_investment_funds;
        for (int i = 0; i < 10; i++) {
            ptr_basic_info->top10_share_holders[i] = other.ptr_basic_info->top10_share_holders[i];
        }
        for (int i = 0; i < 10; i++) {
            ptr_basic_info->top10_trade_share_holders[i] = other.ptr_basic_info->top10_trade_share_holders[i];
        }
        ptr_basic_info->total_bonus = other.ptr_basic_info->total_bonus;
        ptr_basic_info->total_fund = other.ptr_basic_info->total_fund;
    } else {
        ptr_basic_info = nullptr;  // 必须赋初值，否则std::sort会拿到野指针
    }
    return *this;
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
    if (other.ptr_basic_info != nullptr) {
        ptr_basic_info->capital_change_history = other.ptr_basic_info->capital_change_history;
        ptr_basic_info->company_name = other.ptr_basic_info->company_name;
        ptr_basic_info->core_bussiness = other.ptr_basic_info->core_bussiness;
        ptr_basic_info->history_bonus = other.ptr_basic_info->history_bonus;
        ptr_basic_info->holder_change_history = other.ptr_basic_info->holder_change_history;
        ptr_basic_info->ipo_date = other.ptr_basic_info->ipo_date;
        ptr_basic_info->share_investment_funds = other.ptr_basic_info->share_investment_funds;
        for (int i = 0; i < 10; i++) {
            ptr_basic_info->top10_share_holders[i] = other.ptr_basic_info->top10_share_holders[i];
        }
        for (int i = 0; i < 10; i++) {
            ptr_basic_info->top10_trade_share_holders[i] = other.ptr_basic_info->top10_trade_share_holders[i];
        }
        ptr_basic_info->total_bonus = other.ptr_basic_info->total_bonus;
        ptr_basic_info->total_fund = other.ptr_basic_info->total_fund;
    } else {
        ptr_basic_info = nullptr;  // 必须赋初值，否则std::sort会拿到野指针
    }
}

bool Share::operator<(const Share& other) const {
    return code < other.code;
}
