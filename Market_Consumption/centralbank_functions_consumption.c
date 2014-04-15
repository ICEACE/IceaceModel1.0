#include "../header.h"
#include "../centralbank_agent_header.h"


/*
 * \fn: int centralbank_trace_goods_prices()
 * \brief:
 */
int centralbank_trace_goods_prices()
{
    START_GOODS_TRANSACTIONS_SUMMARY_MESSAGE_LOOP
    GOODS.quantity = goods_transactions_summary_message->quantity;
    GOODS.avg_price = goods_transactions_summary_message->avg_price;
	FINISH_GOODS_TRANSACTIONS_SUMMARY_MESSAGE_LOOP

    for (int i = 0; i < 3; i++) {
        WEEKLY_PRICE_AVERAGES[i] = WEEKLY_PRICE_AVERAGES[i+1];
    }
    WEEKLY_PRICE_AVERAGES[3] = GOODS.avg_price;
    
	return 0; /* Returning zero means the agent is not removed */
}

/*
 * \fn: int centralbank_update_price_indices()()
 * \brief: The function updates monthly consumption goods prices.
 */
int centralbank_update_price_indices(){
    double price_avg, prices, quarterly_price_change;
    double price_housing = 0;
    int i;
    
    prices = 0;
    for(i = 0; i < 4; i++){
        prices += WEEKLY_PRICE_AVERAGES[i];
    }
    price_avg = prices / 4;
    price_housing = HOUSES.avg_price;
    
    for(i = 0; i < 11; i++){
        CONSUMPTION_GOODS_PRICES[i] = CONSUMPTION_GOODS_PRICES[i+1];
    }
    CONSUMPTION_GOODS_PRICES[11] = 0.85 * price_avg + 0.15 * price_housing;

    if(CONSUMPTION_GOODS_PRICES[8] == 0){
        quarterly_price_change = 0;
    }
    else {
        quarterly_price_change = (CONSUMPTION_GOODS_PRICES[11] - CONSUMPTION_GOODS_PRICES[8]) / CONSUMPTION_GOODS_PRICES[8];
    }
    
    // MONTHLY_PRICE_INDEX = MONTHLY_PRICE_INDEX + MONTHLY_PRICE_INDEX * monthly_price_change;
    
    add_centralbank_households_quarterly_cpi_message(quarterly_price_change);
    
	return 0; /* Returning zero means the agent is not removed */
}