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
      ptr_brief_info(nullptr),
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
    if (other.ptr_brief_info != nullptr) {
        ptr_brief_info->company_name = other.ptr_brief_info->company_name;
        ptr_brief_info->old_names = other.ptr_brief_info->old_names;
        ptr_brief_info->company_website = other.ptr_brief_info->company_website;
        ptr_brief_info->register_address = other.ptr_brief_info->register_address;
        ptr_brief_info->staff_num = other.ptr_brief_info->staff_num;
        ptr_brief_info->register_capital = other.ptr_brief_info->register_capital;    // 注册资本
        ptr_brief_info->law_office = other.ptr_brief_info->law_office;                // 律师事务所
        ptr_brief_info->accounting_office = other.ptr_brief_info->accounting_office;  // 会计事务所
        ptr_brief_info->ceo = other.ptr_brief_info->ceo;                              // 公司董事长
        ptr_brief_info->board_secretary = other.ptr_brief_info->board_secretary;      // 董秘
        ptr_brief_info->office_address = other.ptr_brief_info->office_address;        // 办公地址
        ptr_brief_info->company_profile = other.ptr_brief_info->company_profile;      // 公司简介
    } else {
        ptr_brief_info = nullptr;
    }
    if (other.ptr_basic_info != nullptr) {
        ptr_basic_info->history_bonus = other.ptr_basic_info->history_bonus;
        ptr_basic_info->total_bonus = other.ptr_basic_info->total_bonus;
        ptr_basic_info->total_fund = other.ptr_basic_info->total_fund;
        ptr_basic_info->capital_change_history = other.ptr_basic_info->capital_change_history;
        ptr_basic_info->holder_change_history = other.ptr_basic_info->holder_change_history;
        ptr_basic_info->share_investment_funds = other.ptr_basic_info->share_investment_funds;
        for (int i = 0; i < 10; i++) {
            ptr_basic_info->top10_share_holders[i] = other.ptr_basic_info->top10_share_holders[i];
        }
        for (int i = 0; i < 10; i++) {
            ptr_basic_info->top10_trade_share_holders[i] = other.ptr_basic_info->top10_trade_share_holders[i];
        }
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
    if (other.ptr_brief_info != nullptr) {
        ptr_brief_info->company_name = other.ptr_brief_info->company_name;
        ptr_brief_info->old_names = other.ptr_brief_info->old_names;
        ptr_brief_info->company_website = other.ptr_brief_info->company_website;
        ptr_brief_info->register_address = other.ptr_brief_info->register_address;
        ptr_brief_info->staff_num = other.ptr_brief_info->staff_num;
        ptr_brief_info->register_capital = other.ptr_brief_info->register_capital;    // 注册资本
        ptr_brief_info->law_office = other.ptr_brief_info->law_office;                // 律师事务所
        ptr_brief_info->accounting_office = other.ptr_brief_info->accounting_office;  // 会计事务所
        ptr_brief_info->ceo = other.ptr_brief_info->ceo;                              // 公司董事长
        ptr_brief_info->board_secretary = other.ptr_brief_info->board_secretary;      // 董秘
        ptr_brief_info->office_address = other.ptr_brief_info->office_address;        // 办公地址
        ptr_brief_info->company_profile = other.ptr_brief_info->company_profile;      // 公司简介
    } else {
        ptr_brief_info = nullptr;
    }
    if (other.ptr_basic_info != nullptr) {
        ptr_basic_info->history_bonus = other.ptr_basic_info->history_bonus;
        ptr_basic_info->total_bonus = other.ptr_basic_info->total_bonus;
        ptr_basic_info->total_fund = other.ptr_basic_info->total_fund;
        ptr_basic_info->capital_change_history = other.ptr_basic_info->capital_change_history;
        ptr_basic_info->holder_change_history = other.ptr_basic_info->holder_change_history;
        ptr_basic_info->share_investment_funds = other.ptr_basic_info->share_investment_funds;
        for (int i = 0; i < 10; i++) {
            ptr_basic_info->top10_share_holders[i] = other.ptr_basic_info->top10_share_holders[i];
        }
        for (int i = 0; i < 10; i++) {
            ptr_basic_info->top10_trade_share_holders[i] = other.ptr_basic_info->top10_trade_share_holders[i];
        }
    } else {
        ptr_basic_info = nullptr;  // 必须赋初值，否则std::sort会拿到野指针
    }
}

bool Share::operator<(const Share& other) const {
    return code < other.code;
}

/**
 * @todo 检查股票当天是否涨停
 * @param kline 日K线
 * @param pShare 股票
 * @return boolean
 */
bool is_up_limit_price(uiKline& kline, Share* pShare) {
    // 检查股票是否是ST股票
    if (is_st_share(pShare)) {
        if (kline.change_rate >= 0.05) {  // 检查涨幅是否达到5%
            return true;                  // 涨停板
        }
        return false;
    }

    // 检查股票是否是上市第一天，名称中带N的，没有涨跌幅限制
    if (pShare->name.find("N") == 0) {
        return false;
    }

    if (pShare->market == Market::ShangHai || pShare->market == Market::ShenZhen) {
        // 检查股票所在市场是否是深圳主板和上海主板，10%的涨幅限制
        if (kline.change_rate >= 0.093) {
            return true;
        }
        return false;
    } else if (pShare->market == Market::ChuangYeBan || pShare->market == Market::KeChuangBan) {
        // 检查股票所在市场是否是创业板或者科创板，20%的涨幅限制
        if (kline.change_rate >= 0.1993) {
            return true;
        }
        return false;
    } else if (pShare->market == Market::BeiJiaoSuo) {
        // 检查股票所在市场是否是北交所，30%的涨幅限制
        if (kline.change_rate >= 0.2993) {
            return true;
        }
        return false;
    }
    return false;
}

/**
 * @todo 检查股票当天是否跌停
 * @param kline 日K线
 * @param pShare 股票
 * @return boolean
 */
bool is_down_limit_price(uiKline& kline, Share* pShare) {
    // 检查股票是否是ST股票
    if (is_st_share(pShare)) {
        if (kline.change_rate <= -0.05) {  // 检查涨幅是否达到5%
            return true;                   // 跌停板
        }
        return false;
    }

    // 检查股票是否是上市第一天，名称中带N的，没有涨跌幅限制
    if (pShare->name.find("N") == 0) {
        return false;
    }

    if (pShare->market == Market::ShangHai || pShare->market == Market::ShenZhen) {
        // 检查股票所在市场是否是深圳主板和上海主板，10%的跌幅限制
        if (kline.change_rate <= -0.093) {
            return true;
        }
        return false;
    } else if (pShare->market == Market::ChuangYeBan || pShare->market == Market::KeChuangBan) {
        // 检查股票所在市场是否是创业板或者科创板，20%的跌幅限制
        if (kline.change_rate <= -0.1993) {
            return true;
        }
        return false;
    } else if (pShare->market == Market::BeiJiaoSuo) {
        // 检查股票所在市场是否是北交所，30%的跌幅限制
        if (kline.change_rate <= -0.2993) {
            return true;
        }
        return false;
    }
    return false;
}

bool is_st_share(Share* pShare) {
    if (pShare->name.find("ST") == 0) {  // 检查股票名称是否以"ST"开头
        return true;
    }
    return false;
}

double get_share_up_limit_price(Share* pShare) {
    if (pShare->code.substr(0, 1) == "N") {  // 新股，没有涨跌幅限制
        return pShare->price_yesterday_close * (1 + 0.44);
    } else if (pShare->code.substr(0, 2) == "ST") {  // ST股，5%涨跌幅限制
        return pShare->price_yesterday_close * (1 + 0.05);
    } else if (pShare->code.substr(0, 3) == "688" ||
               pShare->code.substr(0, 3) == "300") {  // 科创板和创业板股票, 20%涨跌幅限制
        return pShare->price_yesterday_close * (1 + 0.2);
    } else if (pShare->code.substr(0, 1) == "8") {  // 北交所股票，30%涨跌幅限制
        return pShare->price_yesterday_close * (1 + 0.3);
    } else {  // 沪深主板股票，10%涨跌幅限制
        return pShare->price_yesterday_close * (1 + 0.1);
    }
}

double get_share_down_limit_price(Share* pShare) {
    if (pShare->code.substr(0, 1) == "N") {  // 新股，没有涨跌幅限制
        return pShare->price_yesterday_close * (1 - 0.44);
    } else if (pShare->code.substr(0, 2) == "ST") {  // ST股，5%涨跌幅限制
        return pShare->price_yesterday_close * (1 - 0.05);
    } else if (pShare->code.substr(0, 3) == "688" ||
               pShare->code.substr(0, 3) == "300") {  // 科创板和创业板股票, 20%涨跌幅限制
        return pShare->price_yesterday_close * (1 - 0.2);
    } else if (pShare->code.substr(0, 1) == "8") {  // 北交所股票，30%涨跌幅限制
        return pShare->price_yesterday_close * (1 - 0.3);
    } else {  // 沪深主板股票，10%涨跌幅限制
        return pShare->price_yesterday_close * (1 - 0.1);
    }
}