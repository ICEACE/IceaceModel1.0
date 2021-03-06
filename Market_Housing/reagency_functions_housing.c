#include "../header.h"
#include "../reagency_agent_header.h"
#include "../library_header.h"
#include <math.h> 


/*
 * \fn: int reagency_housing_process()
 * \brief: 
 */
int reagency_housing_process()
{
    /* Initialize **
     */
    
    /* Allocate housing buyer queues and seller listss.
     */
    hseller_array sellers_list;
    hbuyer_array buyers_list;
    hbank_array banks_list;
   
    
    /* Initialize */
    init_hseller_array(&sellers_list);
    init_hbuyer_array(&buyers_list);
    init_hbank_array(&banks_list);
    
 
    /* Collect sellers */
    int id, type;
    double price, quantity;
    int fire_seller_attempts = 0;
    int regular_seller_attempts = 0;
    int firm_seller_attempts = 0;
    START_SELL_HOUSING_MESSAGE_LOOP
    type = sell_housing_message->type;
    if (type == 0){firm_seller_attempts++;}
    else if (type == 2){regular_seller_attempts++;}
    else if (type == 1){fire_seller_attempts++;}
    else {
        if (WARNING_MODE) {
            printf("Warning @reagency_housing_process(): Unidentifed seller type is dtected! \n");
        }
        type = -1;
    }
    id = sell_housing_message->seller_id;
    price = sell_housing_message->price;
    quantity = sell_housing_message->quantity;
    add_hseller(&sellers_list, id, price, quantity, type);
	FINISH_SELL_HOUSING_MESSAGE_LOOP
    
    /* Queue the households */
    int bank, mortgage_choice;
    double cash, income, mortgage;
    START_BUY_HOUSING_MESSAGE_LOOP
    id = buy_housing_message->buyer_id;
    bank = buy_housing_message->bank_id;
    cash = buy_housing_message->liquidity;
    income = buy_housing_message->quarterly_income;
    mortgage = buy_housing_message->quarterly_mortgage_paid;
    mortgage_choice = buy_housing_message->mortgage_choice;
    add_hbuyer(&buyers_list,id,bank,cash,income,mortgage,mortgage_choice);
	FINISH_BUY_HOUSING_MESSAGE_LOOP
    
    /* Queue the banks */
    double risk;
    START_BANK_REAGENCY_CREDIBILITY_MESSAGE_LOOP
    id = bank_reagency_credibility_message->bank_id;
    cash = bank_reagency_credibility_message->equity;
    risk = bank_reagency_credibility_message->risky_assets;
    add_hbank(&banks_list, id, cash, risk, 0);
	FINISH_BANK_REAGENCY_CREDIBILITY_MESSAGE_LOOP
    
    if (sellers_list.size == 0 || buyers_list.size == 0) {
        /* Free seller list */
        free_hseller_array(&sellers_list);
        /* Free buyer queues */
        free_hbuyer_array(&buyers_list);
        /* Free bank list */
        free_hbank_array(&banks_list);
        return 0;
    }
    
    if (PRINT_DEBUG_MODE) {
        printf("\nReal Estate Agency Reports: Day= %d, Buyers = %d, Sellers = %d, Banks = %d \n", IT_NO, buyers_list.size, sellers_list.size, banks_list.size);
    }
    
    /* Do the matching *
     * Input assumptions:
     * - sellers are sorted ascendingly by price.
     * - buyers are randomly queued
     * These are arranged at message filtering stage.
     */
    
    
    /* Compute annuity for possible mortgages. */
    double annuity;
    double d1, d2;
    /* Not needed after implementing code for each mortgage type */
    //d1 = MORTGAGES_INTEREST_RATE/4;
    //d2 = d1 * pow((1 + d1), 160);
    //annuity = 1/d1 - 1/d2;
    
    int nsold = 0;
    int transaction_quantity = 0;
    double transaction_volume = 0;
    int fire_sales = 0;
    int regular_sales = 0;
    int firm_sales = 0;
    do {
        if (sellers_list.size == 0 ){break;}
        if (buyers_list.size == 0 ){break;}
        
        quantity = sellers_list.array[0].quantity;
        id = sellers_list.array[0].seller_id;
        price = sellers_list.array[0].price;
        type = sellers_list.array[0].type;
        
        if (nsold == quantity){
            add_sold_housing_message(id, nsold, price);
            remove_hseller(&sellers_list, 0);
            transaction_quantity += nsold;
            transaction_volume += nsold * price;
            nsold = 0;
            if (type == 0) { firm_sales++;}
            if (type == 2) { regular_sales++;}
            if (type == 1) { fire_sales++;}
            continue;
        }
        id = buyers_list.array[0].buyer_id;
        cash = buyers_list.array[0].liquidity;
        if (cash >= price) {
            add_bought_housing_message(id, price, 0, 0);
            nsold++;
            
            if (PRINT_DEBUG_MODE) {
                printf("Real Estate Agency Reports: Buyer ID = %d, bought a housing unit for %f right away! \n", id, price);
            }
            remove_hbuyer(&buyers_list, 0);
            continue;
        }
        
        /* The household needs mortgage.
         */
        
        /* No banks in the market is able to give mortgage credit. */
        if (banks_list.size == 0 ){
            if (PRINT_DEBUG_MODE) {
                printf("Real Estate Agency Reports: No more more banks is available to offer mortgage! \n");
            }
            remove_hbuyer(&buyers_list, 0);
            /* It is possible that some other buyers are able to buy without any credit. */
            continue;
        }
        
        /* Check available mortgaging capacity of the bank. */
        bank = buyers_list.array[0].bank_id;
        int i = 0;
        int flag = 0;
        do {
            if (banks_list.array[i].id == bank){
                flag = 1;
                break;
            }
            i++;
        } while (i < banks_list.size);
        if (flag == 0){
            remove_hbuyer(&buyers_list, 0);
            if (PRINT_DEBUG_MODE) {
                printf("Bank of Household ID = %d is not available any more for crediting the mortgage.\n", id);
            }
            continue;
        }
        
        double equity = banks_list.array[i].equity;

        /* Risky asssets before the market has opened. */
        risk = banks_list.array[i].risky_assets;
        /* Risk of bank is evalauted incrementally. */
        risk += banks_list.array[i].amount_mortgaged;
        
        if (equity < CAPITAL_ADEQUECY_RATIO * risk) {
            remove_hbuyer(&buyers_list, 0);
            remove_hbank(&banks_list, i);
            if (PRINT_DEBUG_MODE) {
                printf("Bank ID = %d, was not allowed to give mortgages any more.\n", banks_list.array[i].id);
            }
            continue;
        }
        
        double mortgage_request = 0;
        if (cash > 0) {
            mortgage_request = price - cash;
        } else {
            mortgage_request = price;
        }
        
        /* Risk of the bank is updated temporally/locally and evaluated including the new requested mortgage. */
        risk += mortgage_request;
        
        /* The bank cannot mortgage this buyer. But the bank may be able to credit
         some others waiting in the queue. */
        if (equity < CAPITAL_ADEQUECY_RATIO * risk) {
            remove_hbuyer(&buyers_list, 0);
            if (PRINT_DEBUG_MODE) {
                printf("Bank ID = %d, was not allowed to give mortgages to Household ID = %d. \n", banks_list.array[i].id, id);
            }
            continue;
        }
        /* Here mortgage choice of household could be adopted according to crediting conditions in an endogenous manner.*/

        
        /* Check credibility of the household */
        mortgage_choice = buyers_list.array[0].choice;
        income = buyers_list.array[0].quarterly_income;
        mortgage = buyers_list.array[0].quarterly_mortgage_paid;
        double new_mortgage_cost = 0; // = mortgage_request / annuity;
        double used_interest_rate = 0;

        if (mortgage_choice == 1) {
            d1 = MORTGAGES_INTEREST_RATE/4;
            d2 = d1 * pow((1 + d1), LOANTERM);
            annuity = 1/d1 - 1/d2;
            new_mortgage_cost = mortgage_request / annuity;
        }
        else if (mortgage_choice == 2){
            new_mortgage_cost = (mortgage_request / LOANTERM) + (mortgage_request*MORTGAGES_INTEREST_RATE/4);
        }
        else if (mortgage_choice == 3){
            /* Should we incorporate an expectation of inflation, we could use the inflation target of CB, 0.02? */
            if (USE_FIXED_RATE) {
                used_interest_rate = FIXED_RATE;
            } else {
                used_interest_rate = MORTGAGES_INTEREST_RATE - IIM_RATE_DISCOUNT; 
            }
            d1 = used_interest_rate/4;
            d2 = d1 * pow((1 + d1), LOANTERM);
            annuity = 1/d1 - 1/d2;
            new_mortgage_cost = mortgage_request / annuity;
        }
        else if (mortgage_choice == 4){
            if (USE_FIXED_RATE) {
                used_interest_rate = FIXED_RATE;
            } else {
                used_interest_rate = MORTGAGES_INTEREST_RATE - IIM_RATE_DISCOUNT; 
            }
            /* Should we incorporate an expectation of inflation, we could use the inflation target of CB, 0.02? */
            new_mortgage_cost = (mortgage_request / LOANTERM) + (mortgage_request*used_interest_rate/4);
        }
        else if (mortgage_choice == 5){
            d1 = (MORTGAGES_INTEREST_RATE + 0.01)/4;
            d2 = d1 * pow((1 + d1), LOANTERM);
            annuity = 1/d1 - 1/d2;
            new_mortgage_cost = mortgage_request / annuity;
        }
        else if (mortgage_choice == 6){
            new_mortgage_cost = (mortgage_request / LOANTERM) + (mortgage_request*(MORTGAGES_INTEREST_RATE+0.01)/4);
        }
        else if (mortgage_choice == 7){
            if (USE_FIXED_RATE) {
                used_interest_rate = FIXED_RATE;
            } else {
                used_interest_rate = MORTGAGES_INTEREST_RATE - IIM_RATE_DISCOUNT; 
            }
            /* Should we incorporate an expectation of inflation, we could use the inflation target of CB, 0.02? */
            d1 = used_interest_rate/4;
            d2 = d1 * pow((1 + d1), LOANTERM);
            annuity = 1/d1 - 1/d2;
            new_mortgage_cost = mortgage_request / annuity + mortgage_request * (pow(1+EXPECTED_INFLATION_RATE,1/4)-1);
        }
        else {
            if (WARNING_MODE) {
                printf("Warning @household_housing_debt_writeoff(): Unexpected mortgage choice = %d \n", mortgage_choice);
            }

        }

        
        mortgage += new_mortgage_cost;
        
        if (mortgage > HOUSEHOLD_BUDGET_CONSTRAINT * income) {
            remove_hbuyer(&buyers_list, 0);
            if (PRINT_DEBUG_MODE) {
                printf("Household ID = %d mortgage request is denied by Bank ID = %d \n", id, banks_list.array[i].id);
            }
            continue;
        }
        
        /* Mortgage is used. Cash at hand by household while enetering the market
         assumed to be non-negative.
         */
        add_bought_housing_message(id, cash, mortgage_request, annuity);
        if (PRINT_DEBUG_MODE) {
            printf("Household ID = %d has gotten %f from Bank ID = %d along with her own %f amount of cash. \n", id, mortgage_request, banks_list.array[i].id, cash);
        }
        remove_hbuyer(&buyers_list, 0);
        nsold++;
        /* The mortgage amount of the bank is increased incrementally. */
        banks_list.array[i].amount_mortgaged += mortgage_request;
    } while (1);
    
    if (nsold > 0 && sellers_list.size > 0) {
        id = sellers_list.array[0].seller_id;
        type = sellers_list.array[0].type;
        add_sold_housing_message(id, nsold, price);
        transaction_quantity += nsold;
        transaction_volume += nsold * price;
        if (type == 0) { firm_sales++;}
        if (type == 2) { regular_sales++;}
        if (type == 1) { fire_sales++;}
    }
    
  
    for (int i = 0; i < banks_list.size; i++) {
        add_mortgage_requests_message(banks_list.array[i].id, banks_list.array[i].amount_mortgaged);
        }
    
    HOUSING_TRANSACTIONS.quantity = transaction_quantity;
    if (transaction_quantity > 0) {
        HOUSING_TRANSACTIONS.avg_price = transaction_volume / transaction_quantity;
    }
    
    
    if (DATA_COLLECTION_MODE) {
        char * filename;
        FILE * file1;
        filename = malloc(100*sizeof(char));
        filename[0]=0;
        strcpy(filename, "./outputs/data/REAgency_sales.txt");
        
        file1 = fopen(filename,"a");
        fprintf(file1,"%d %d %d %d %d %d %d\n",IT_NO, fire_seller_attempts, regular_seller_attempts, firm_seller_attempts, fire_sales, regular_sales, firm_sales);
        fclose(file1);
        free(filename);
    }
    
    
    /* Garbage Collection */
    free_hseller_array(&sellers_list);
    free_hbuyer_array(&buyers_list);
    free_hbank_array(&banks_list);
	return 0; /* Returning zero means the agent is not removed */
}

/*
 * \fn: int reagency_housing_summary()
 * \brief:
 */
int reagency_housing_summary()
{
    double price;
    int quantity;
    
    price = HOUSING_TRANSACTIONS.avg_price;
    quantity = HOUSING_TRANSACTIONS.quantity;
    
    add_housing_price_message(price);
    add_housing_transactions_summary_message(quantity, price);
    
    if (DATA_COLLECTION_MODE) {
        char * filename;
        FILE * file1;
        filename = malloc(40*sizeof(char));
        filename[0]=0;
        strcpy(filename, "./outputs/data/REAgency_snapshot.txt");
        
        file1 = fopen(filename,"a");
        fprintf(file1,"%d %d %f\n",IT_NO, quantity, price);
        fclose(file1);
        free(filename);
    }
	return 0; /* Returning zero means the agent is not removed */
}
