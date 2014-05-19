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
    
    for(i = 0; i < 12; i++){
        CONSUMPTION_GOODS_PRICES[i] = CONSUMPTION_GOODS_PRICES[i+1];
        HOUSING_PRICES[i] = HOUSING_PRICES[i+1];
    }
    CONSUMPTION_GOODS_PRICES[12] = price_avg;
    HOUSING_PRICES[12] = price_housing;
    double qpi_goods, qpi_housing;
    

    if(CONSUMPTION_GOODS_PRICES[9] == 0){
        qpi_goods = 0;
    }
    else {
        qpi_goods = (CONSUMPTION_GOODS_PRICES[12] - CONSUMPTION_GOODS_PRICES[9]) / CONSUMPTION_GOODS_PRICES[9];
    }
    if(HOUSING_PRICES[9] == 0) {
        qpi_housing = 0;
    }
    else {
        qpi_housing = (HOUSING_PRICES[12]/HOUSING_PRICES[9] - 1);
    }
    quarterly_price_change = 0.85 * qpi_goods + 0.15 * qpi_housing;
        
    // MONTHLY_PRICE_INDEX = MONTHLY_PRICE_INDEX + MONTHLY_PRICE_INDEX * monthly_price_change;
    
    add_centralbank_households_quarterly_cpi_message(quarterly_price_change);
    
	return 0; /* Returning zero means the agent is not removed */
}