#pragma once

// 百度股市通 https://gushitong.baidu.com/stock/ab-300059
#define KLINE_URL_FINANCE_BAIDU(share_code, kline_type, extra) \
    "https://finance.pae.baidu.com/vapi/v1/getquotation"       \
    "?srcid=5353"                                              \
    "&pointType=string"                                        \
    "&group=quotation_kline_ab"                                \
    "&query=" +                                                \
        share_code + "&code=" + share_code +                   \
        "&market_type=ab"                                      \
        "&newFormat=1"                                         \
        "&is_kc=0"                                             \
        "&ktype=" +                                            \
        kline_type + "&finClientType=pc" + extra + "&finClientType=pc"

// 百度股市通分时走势图
#define KLINE_URL_FINANCE_BAIDU_MINUTE(share_code)       \
    "https://finance.pae.baidu.com/vapi/v1/getquotation" \
    "?srcid=5353"                                        \
    "&pointType=string"                                  \
    "&group=quotation_minute_ab"                         \
    "&query=" +                                          \
        share_code + "&code=" + share_code +             \
        "&market_type=ab"                                \
        "&new_Format=1"                                  \
        "&finClientType=pc"

// 百度股市通近5日分时走势图
#define KLINE_URL_FINANCE_BAIDU_FIVE_DAY(share_code, share_name)     \
    "https://finance.pae.baidu.com/vapi/v1/getquotation"             \
    "?srcid=5353"                                                    \
    "&pointType=string"                                              \
    "&group=quotation_fiveday_ab"                                    \
    "&query=" +                                                      \
        share_code + "&code=" + share_code + "&name=" + share_name + \
        "&market_type=ab"                                            \
        "&new_Format=1"                                              \
        "&finClientType=pc"

// 东方财富分时K线
#define KLINE_URL_EAST_MONEY_MINUTE(share_code, market)              \
    "https://83.push2.eastmoney.com/api/qt/stock/trends2/"           \
    "sse?fields1=f1,f2,f3,f4,f5,f6,f7,f8,f9,f10,f11,f12,f13,f14,f17" \
    "&fields2=f51,f52,f53,f54,f55,f56,f57,f58"                       \
    "&mpi=1000"                                                      \
    "&secid=" +                                                      \
        std::to_string(market) + "." + share_code +                  \
        "&ndays=1"                                                   \
        "&iscr=0"                                                    \
        "&iscca=0"
// 东方财富5日分时K线
#define KLINE_URL_EAST_MONEY_FIVE_DAY(share_code, market)            \
    "https://48.push2.eastmoney.com/api/qt/stock/trends2/"           \
    "sse?fields1=f1,f2,f3,f4,f5,f6,f7,f8,f9,f10,f11,f12,f13,f14,f17" \
    "&fields2=f51,f52,f53,f54,f55,f56,f57,f58"                       \
    "&mpi=1000"                                                      \
    "&secid=" +                                                      \
        std::to_string(market) + "." + share_code +                  \
        "&ndays=5"                                                   \
        "&iscr=0"                                                    \
        "&iscca=0"

// 东方财富行情中心 https://quote.eastmoney.com/concept/sz300729.html
#define KLINE_URL_EAST_MONEY(share_code, market, kline_type) \
    "https://push2his.eastmoney.com/api/qt/stock/kline/get"  \
    "?fields1=f1,f2,f3,f4,f5,f6,f7,f8,f9,f10,f11,f12,f13"    \
    "&fields2=f51,f52,f53,f54,f55,f56,f57,f58,f59,f60,f61"   \
    "&begin=0"                                               \
    "&end=20500101"                                          \
    "&rtntype=6"                                             \
    "&lmt=1000000"                                           \
    "&secid=" +                                              \
        std::to_string(market) + "." + share_code + "&klt=" + std::to_string(kline_type) + "&fqt=1"

// 腾讯财经行情页 https://gu.qq.com/sz002252/gp
#define KLINE_URL_TENCENT(share_code, market_abbr, kline_type, year, start_time, end_time)                             \
    "https://proxy.finance.qq.com/ifzqgtimg/appstock/app/newfqkline/"                                                  \
    "get?_var=kline_" +                                                                                                \
        kline_type + "qfq" + year + "&param=" + market_abbr + share_code + "," + kline_type + "," + start_time + "," + \
        end_time + ",640,qfq"
