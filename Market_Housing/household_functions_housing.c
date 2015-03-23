#include "../header.h"
#include "../household_agent_header.h"
#include "../library_header.h"
#include <math.h> 

/*
 * \fn: int household_housing_market_role()
 * \brief: Sets the market role of the household. It is a fire seller, determined by financial distress. Otherwise, household plays role of a buyer or seller equally likely. iseseller, 'intention' variable is set accordingly.
 */
int household_housing_market_role()
{
    double quarterly_income;
    
    quarterly_income = LABOUR_INCOME + CAPITAL_INCOME;
    
    /* Checking the financial distress for the fire sale case. */
    if (HOUSING_PAYMENT > FIRE_SALE_THRESHOLD * quarterly_income){
        if (HOUSING_UNITS == 0) { HMARKET_ROLE = 0; } else { HMARKET_ROLE  = 1;}
        return 0;
    }
    
    if (CAPITAL_INCOME > LABOUR_INCOME) {
        /* Seller: */
        if (DELTA_HOUSING_PRICE < 0) {
            HMARKET_ROLE = 2;}
        /* Buyer: */
        else {
            HMARKET_ROLE = 3;
        }
        return 0;
    }


    
    /* Inactive */
    if (random_int(0, 99) < (int)(100 - 2 * HOUSING_MARKET_ENTRANCE_PROB * 100)) {
        HMARKET_ROLE = 0;
        return 0;
    }
    
    /* Seting a regular buyer or seller role, via flipping the coin. */
    if (random_int(0, 99) > 50) {
        /* Regular seller.*/
        if (HOUSING_UNITS == 0) { HMARKET_ROLE = 0;} else { HMARKET_ROLE  = 2;}
    }
    else {
        /* Buyer */
        HMARKET_ROLE = 3;
    }
    
	return 0; /* Returning zero means the agent is not removed */
}

/*
 * \fn: int household_housing_check_wealth()
 * \brief: The househld computes his equity ratio.
 */
int household_housing_check_wealth()
{
    double wealth = 0;
    
    /* Capital income is not in the equation as it is liquidified quarterly as sooon as it is recieved.
     */
    wealth = LIQUIDITY + HOUSING_VALUE;
    
    if (fabs(wealth) <= 0.01){
        EQUITY_RATIO = 0;
    } else {
        EQUITY_RATIO = EQUITY / wealth;
    }
    
    if (EQUITY_RATIO <= 0 ) {
        if (WARNING_MODE) {
            //printf("Warning @household_housing_check_wealth(): Household %d has more mortgages than total assets. Equity = %f, Mortgages = %f \n", ID, EQUITY, MORTGAGES);
        }
    }
    
	return 0; /* Returning zero means the agent is not removed */
}


/*
 * \fn: int household_housing_enter_market()
 * \brief: the household enters the housing market as a buyer
 */
int household_housing_enter_market()
{
    double income, cash;
    
    income = LABOUR_INCOME + CAPITAL_INCOME;
    
    /* Cash rich households are allowed to use cash to finance housing.
     This needs to be checked with the model design.
     
    double cash_allowance;
    cash_allowance = 0.9 * HOUSING_PRICE;
    if (LIQUIDITY > cash_allowance) {
        cash = LIQUIDITY - cash_allowance;
    } else {
        cash = 0;
    }
    */
    
    /* Endogenous mortgage choice mechnism may be implemented at that point.*/
    
    /* Households aim to finance their housing purchase fully via mortgage credits.
     */
    cash = LIQUIDITY;
    //cash = 0;
    add_buy_housing_message(ID, BANK_ID, cash, income, HOUSING_PAYMENT, MORTGAGE_CHOICE);
    
	return 0; /* Returning zero means the agent is not removed */
}

/*
 * \fn: int household_housing_bargain()
 * \brief:
 */
int household_housing_buy()
{
    double mortgage_used, liquidity_spent, annuity;
    mortgage_used = 0;
    liquidity_spent = 0;
        
    START_BOUGHT_HOUSING_MESSAGE_LOOP
    /* The message is filtered via xmml. */
    mortgage_used = bought_housing_message->mortgage_used;
    liquidity_spent = bought_housing_message->liquidity_spent;
    annuity = bought_housing_message->annuity;
	FINISH_BOUGHT_HOUSING_MESSAGE_LOOP
    
    if (mortgage_used + liquidity_spent > 0){
        HOUSING_UNITS += 1;
        if (PRINT_DEBUG_MODE) {
            printf("Household ID = %d has bought a new housing unit for %f. \n", ID, mortgage_used + liquidity_spent);
        }
    }
    
    if (liquidity_spent > 0){
        LIQUIDITY -= liquidity_spent;
    }
    
    if (mortgage_used > 0){
        double quarterly_interest;
        double quarterly_principal;
        double d1, d2;
        double used_interest_rate;

        /* Endogenous decision rule on which type of mortgage to take should be done here*/
        MORTGAGE_CHOICE = MORTGAGE_CHOICE;
        
        quarterly_interest = 0;
        quarterly_principal = 0;
        
        if (MORTGAGE_CHOICE == 1) {
            quarterly_interest = mortgage_used * MORTGAGES_INTEREST_RATE / 4;
            quarterly_principal = (mortgage_used / annuity) - quarterly_interest;
            add_mortgage(&MORTGAGES_LIST, BANK_ID, mortgage_used, LOANTERM, quarterly_interest, quarterly_principal, MORTGAGES_INTEREST_RATE, MORTGAGE_CHOICE);
        }
        else if (MORTGAGE_CHOICE == 2){
            quarterly_interest = mortgage_used * MORTGAGES_INTEREST_RATE / 4;
            quarterly_principal = (mortgage_used / LOANTERM);
            add_mortgage(&MORTGAGES_LIST, BANK_ID, mortgage_used, LOANTERM, quarterly_interest, quarterly_principal, MORTGAGES_INTEREST_RATE, MORTGAGE_CHOICE);
        }
        else if (MORTGAGE_CHOICE == 3){
            if (USE_FIXED_RATE) {
                used_interest_rate = FIXED_RATE;
            } else {
                used_interest_rate = MORTGAGES_INTEREST_RATE - IIM_RATE_DISCOUNT; 
            }
            quarterly_interest = mortgage_used * used_interest_rate/4;
            d1 = used_interest_rate/4;
            d2 = d1 * pow((1 + d1), LOANTERM);
            annuity = 1/d1 - 1/d2;
            quarterly_principal = (mortgage_used / annuity) - quarterly_interest;
            add_mortgage(&MORTGAGES_LIST, BANK_ID, mortgage_used, LOANTERM, quarterly_interest, quarterly_principal, used_interest_rate, MORTGAGE_CHOICE);
        }
        else if (MORTGAGE_CHOICE == 4){
            if (USE_FIXED_RATE) {
                used_interest_rate = FIXED_RATE;
            } else {
                used_interest_rate = MORTGAGES_INTEREST_RATE - IIM_RATE_DISCOUNT; 
            }
            quarterly_interest = mortgage_used * used_interest_rate/4;
            quarterly_principal = (mortgage_used / LOANTERM);
            add_mortgage(&MORTGAGES_LIST, BANK_ID, mortgage_used, LOANTERM, quarterly_interest, quarterly_principal, used_interest_rate, MORTGAGE_CHOICE);
        }
        else if (MORTGAGE_CHOICE == 5){
            used_interest_rate = MORTGAGES_INTEREST_RATE + 0.01;
            quarterly_interest = mortgage_used * used_interest_rate/4;
            d1 = used_interest_rate/4;
            d2 = d1 * pow((1 + d1), LOANTERM);
            annuity = 1/d1 - 1/d2;
            quarterly_principal = (mortgage_used / annuity) - quarterly_interest;
            add_mortgage(&MORTGAGES_LIST, BANK_ID, mortgage_used, LOANTERM, quarterly_interest, quarterly_principal, used_interest_rate, MORTGAGE_CHOICE);
        }
        else if (MORTGAGE_CHOICE == 6){
            used_interest_rate = MORTGAGES_INTEREST_RATE + 0.01;
            quarterly_interest = mortgage_used * used_interest_rate / 4;
            quarterly_principal = (mortgage_used / LOANTERM);
            add_mortgage(&MORTGAGES_LIST, BANK_ID, mortgage_used, LOANTERM, quarterly_interest, quarterly_principal, used_interest_rate, MORTGAGE_CHOICE);
        }
        else if (MORTGAGE_CHOICE == 7){
            if (USE_FIXED_RATE) {
                used_interest_rate = FIXED_RATE;
            } else {
                used_interest_rate = MORTGAGES_INTEREST_RATE - IIM_RATE_DISCOUNT; 
            }
            d1 = used_interest_rate/4;
            d2 = d1 * pow((1 + d1), LOANTERM);
            annuity = 1/d1 - 1/d2;
            /* Interest payement includes index adjustment */
            quarterly_interest = mortgage_used * used_interest_rate/4 + mortgage_used * QUARTERLY_PRICE_CHANGE;
            quarterly_principal = (mortgage_used / annuity) - (mortgage_used * (pow(1+EXPECTED_INFLATION_RATE,1/4)-1));
            add_mortgage(&MORTGAGES_LIST, BANK_ID, mortgage_used, LOANTERM, quarterly_interest, quarterly_principal, used_interest_rate, MORTGAGE_CHOICE);
        }
        else {
            if (WARNING_MODE) {
                printf("Warning @household_housing_buy(): Unexpected mortgage choice = %d \n", MORTGAGE_CHOICE);
            }

        }

        /* Updating the cummulative mortgages and total mortgage payments. */
        EXPECTED_HOUSING_PAYMENT += quarterly_interest + quarterly_principal;
        MORTGAGES += mortgage_used;
        if (PRINT_DEBUG_MODE) {
            printf("Household ID = %d a new mortgage debt of %f. \n", ID, mortgage_used);
        }
    }
    
	return 0; /* Returning zero means the agent is not removed */
}


/*
 * \fn: int household_housing_sell()
 * \brief: a regular seller household, going with some price over the average.
 */
int household_housing_sell()
{
    if (HOUSING_UNITS < 3) {
        return 0;
    }
    
    double price_difference, price;
    
    price_difference = (((double)random_int(0, 100)) / 100.0) * HOUSING_PRICE_UP_RATE;
    
    price = HOUSING_PRICE * (1 + price_difference);
    
    add_sell_housing_message(ID, price, 1, HMARKET_ROLE);
    
	return 0; /* Returning zero means the agent is not removed */
}

/*
 * \fn: int household_housing_fire_sell()
 * \brief: a fire seller household, going with some price below  the average.
 */
int household_housing_fire_sell()
{
    if (HOUSING_UNITS < 3) {
        return 0;
    }
    
    double price_difference, price;
    
    price_difference = (((double)random_int(0, 100)) / 100.0) * HOUSING_PRICE_DOWN_RATE;
    
    price = HOUSING_PRICE * (1 - price_difference);
    
    if (price > 0) {
        add_sell_housing_message(ID, price, 1, HMARKET_ROLE);
    }
        
	return 0; /* Returning zero means the agent is not removed */
}

/*
 * \fn: int household_housing_collect_sale_revenue()
 * \brief:
 */
int household_housing_collect_sale_revenue()
{
    double sale_price;
    int sold = 0;
    
    START_SOLD_HOUSING_MESSAGE_LOOP
    /*The message is filtered via xmml. */
    sold = sold_housing_message->quantity_sold;
    sale_price = sold_housing_message->price_sold;
    FINISH_SOLD_HOUSING_MESSAGE_LOOP
    
    if (sold == 0){
        return 0;
    }
    
    /* In current implementation sold is either 0 or 1 */
    HOUSING_UNITS -= sold;
    if (PRINT_DEBUG_MODE) {
        printf("Household ID = %d (%d) has gotten a house unit sold for %f \n", ID, HMARKET_ROLE, sale_price);
    }
    
    if (MORTGAGES_LIST.size == 0) {
        LIQUIDITY += sale_price;
        return 0;
    }
    
//    /* The most recent mortgage is assumed to be sold. */
//    ind = MORTGAGES_LIST.size - 1;
//    mort = MORTGAGES_LIST.array[ind];
//    MORTGAGES -= mort.principal;
//    /* If the sale is lower than the principal amount, the liquid asset is to be used. */
//    add_mortgage_payment_from_sale_message(BANK_ID, mort.principal);
//    LIQUIDITY += sale_price - mort.principal;
//    remove_mortgage(&MORTGAGES_LIST, ind);
//    free_mortgage(&mort);
//    return 0;
    
    //int ind;
    //mortgage mort;
    //init_mortgage(&mort);
    
    double principal, new_principle;
    double quarterly_interest, new_quarterly_interest;
    double quarterly_principal, new_quarterly_principal;
    double annuity;
    double d1, d2;
    double rate;
    int quarters_left;
    
    /* Regular seller: */
    /*if (HMARKET_ROLE == 2){
        ind = MORTGAGES_LIST.size - 1;
        mort = MORTGAGES_LIST.array[ind];
        if (mort.principal <= sale_price){
            add_mortgage_payment_from_sale_message(BANK_ID, mort.principal);
            MORTGAGES -= mort.principal;
            LIQUIDITY += sale_price - mort.principal;
            remove_mortgage(&MORTGAGES_LIST, ind);
            if (PRINT_DEBUG_MODE) {
                printf("Regular Seller = %d has removed a mortgage = %f \n", ID, mort.principal);
            }
        }
        else {
            add_mortgage_payment_from_sale_message(BANK_ID, sale_price);
            MORTGAGES -= sale_price;
            
            new_principle = mort.principal - sale_price;
            d1 = MORTGAGES_INTEREST_RATE/4;
            d2 = d1 * pow((1 + d1), mort.quarters_left);
            annuity = 1/d1 - 1/d2;
            new_quarterly_interest = new_principle * d1;
            new_quarterly_principal = (new_principle / annuity) - new_quarterly_interest;
            MORTGAGES_LIST.array[ind].principal = new_principle;
            MORTGAGES_LIST.array[ind].quarterly_interest = new_quarterly_interest;
            MORTGAGES_LIST.array[ind].quarterly_principal = new_quarterly_principal;
            if (PRINT_DEBUG_MODE) {
                printf("Regular Seller = %d has decreased a mortgage debt by a = %f amount. \n", ID, sale_price);
            }
        }
        free_mortgage(&mort);
        return 0;
    }*/
    
    /* Fire seller:
       Pays mortgages as long as sale revenue is enough to clear additional mortgages.
     */

    while (sale_price > 0) {
        
        if (MORTGAGES_LIST.size == 0){ break;}
        
        /* pays the newest mortgage first! */
        //ind = MORTGAGES_LIST.size - 1;
        //mort = MORTGAGES_LIST.array[ind];
        //EXPECTED_HOUSING_PAYMENT -= mort.quarterly_interest + mort.quarterly_principal;
        
        /* oldest mortgage is removed first. */
        quarterly_interest = MORTGAGES_LIST.array[0].quarterly_interest;
        quarterly_principal = MORTGAGES_LIST.array[0].quarterly_principal;
        principal = MORTGAGES_LIST.array[0].principal;
        EXPECTED_HOUSING_PAYMENT -=  quarterly_principal + quarterly_interest;
        
        if (principal <= sale_price){
            MORTGAGES -= principal;
            add_mortgage_payment_from_sale_message(BANK_ID, principal);
            sale_price -= principal;
            if (PRINT_DEBUG_MODE) {
             printf("Fire Seller = %d has removed the mortgage debt = %f \n", ID, principal);
            }
            remove_mortgage(&MORTGAGES_LIST, 0);
        }
        else {
            quarters_left = MORTGAGES_LIST.array[0].quarters_left;
            rate = MORTGAGES_LIST.array[0].interestrate;

            add_mortgage_payment_from_sale_message(BANK_ID, sale_price);

            MORTGAGES -= sale_price;
            new_principle = principal - sale_price;

            if (PRINT_DEBUG_MODE) {
                printf("Fire Seller = %d decreases a mortgage debt from = %f to %f \n", ID, principal, new_principle);
            }

            new_quarterly_interest = 0;
            new_quarterly_principal = 0;
            if (MORTGAGE_CHOICE == 1) {
                d1 = MORTGAGES_INTEREST_RATE/4;
                d2 = d1 * pow((1 + d1), quarters_left);
                annuity = 1/d1 - 1/d2;
                new_quarterly_interest = new_principle * d1;
                new_quarterly_principal = (new_principle / annuity) - new_quarterly_interest;

            }
            else if (MORTGAGE_CHOICE == 2){
                new_quarterly_interest = new_principle * MORTGAGES_INTEREST_RATE / 4;
                new_quarterly_principal = new_principle / quarters_left;
            }
            else if (MORTGAGE_CHOICE == 3){
                d1 = rate/4;
                d2 = d1 * pow((1 + d1), quarters_left);
                annuity = 1/d1 - 1/d2;

                new_quarterly_interest = new_principle * d1;
                new_quarterly_principal = (new_principle / annuity) - new_quarterly_interest;
            }
            else if (MORTGAGE_CHOICE == 4){
                new_quarterly_interest = new_principle * rate/4;
                new_quarterly_principal = new_principle / quarters_left;
            }
            else if (MORTGAGE_CHOICE == 5){
                d1 = rate/4;
                d2 = d1 * pow((1 + d1), quarters_left);
                annuity = 1/d1 - 1/d2;

                new_quarterly_interest = new_principle * d1;
                new_quarterly_principal = (new_principle / annuity) - new_quarterly_interest;
            }
            else if (MORTGAGE_CHOICE == 6){
                new_quarterly_interest = new_principle * rate/4;
                new_quarterly_principal = new_principle / quarters_left;
            }
            else if (MORTGAGE_CHOICE == 7){
                d1 = rate/4;
                d2 = d1 * pow((1 + d1), quarters_left);
                annuity = 1/d1 - 1/d2;

                new_quarterly_interest = (new_principle * rate/4) + new_principle * QUARTERLY_PRICE_CHANGE;
                new_quarterly_principal = (new_principle / annuity) - (new_principle * rate/4);
            }
            else {
                if (WARNING_MODE) {
                    printf("Warning @household_housing_buy(): Unexpected mortgage choice = %d \n", MORTGAGE_CHOICE);
                }
            }

            MORTGAGES_LIST.array[0].principal = new_principle;
            MORTGAGES_LIST.array[0].quarterly_interest = new_quarterly_interest;
            MORTGAGES_LIST.array[0].quarterly_principal = new_quarterly_principal;
            EXPECTED_HOUSING_PAYMENT += new_quarterly_interest + new_quarterly_principal;
            sale_price = 0;
        }
    }
    
    if (sale_price > 0) {
        LIQUIDITY += sale_price;
    }
    
    //free_mortgage(&mort);
	return 0;
}

/*
 * \fn: int household_housing_update_market_price()
 * \brief: Avg market price of housing is updated.
 */
int household_housing_update_market_price()
{
    double pre_value, pre_price;
    
    pre_value = HOUSING_UNITS * HOUSING_PRICE;
    pre_price = HOUSING_PRICE;
    
    START_HOUSING_PRICE_MESSAGE_LOOP
    HOUSING_PRICE = housing_price_message->price;
	FINISH_HOUSING_PRICE_MESSAGE_LOOP
    
    HOUSING_VALUE = HOUSING_UNITS * HOUSING_PRICE;
    DELTA_HOUSING_VALUE = HOUSING_VALUE - pre_value;
    DELTA_HOUSING_PRICE = HOUSING_PRICE - pre_price;
    
	return 0; /* Returning zero means the agent is not removed */
}

/*
 * \fn: int household_housing_pay_mortgages()
 * \brief: mortgage payments are sent to bank.
 */
int household_housing_pay_mortgages()
{
    int size;
    size = MORTGAGES_LIST.size;
    if (size == 0){ return 0;}
    
    //mortgage mort;
    //init_mortgage(&mort);
    
    int i, bankid;
    double total_interest_paid = 0;
    double total_principal_paid = 0;
    double interest_paid;
    double principal_paid, principal_left;
    
    for (i = 0; i < size; i++) {
        //mort = MORTGAGES_LIST.array[i];
        if (MORTGAGES_LIST.array[i].principal < 0.001) { continue;}
        bankid = MORTGAGES_LIST.array[i].bank_id;
        interest_paid = MORTGAGES_LIST.array[i].quarterly_interest / 3;
        principal_paid = MORTGAGES_LIST.array[i].quarterly_principal / 3;
        if (principal_paid > MORTGAGES_LIST.array[i].principal) {
            principal_paid = MORTGAGES_LIST.array[i].principal;
        }
        principal_left = MORTGAGES_LIST.array[i].principal - principal_paid;
        MORTGAGES_LIST.array[i].principal = principal_left;
        add_mortgage_payment_message(bankid, interest_paid, principal_paid);
        total_interest_paid += interest_paid;
        total_principal_paid += principal_paid;
    }
    
    MORTGAGE_COSTS[2] = MORTGAGE_COSTS[1];
    MORTGAGE_COSTS[1] = MORTGAGE_COSTS[0];
    MORTGAGE_COSTS[0] = total_principal_paid + total_interest_paid;
    
    MORTGAGES -= total_principal_paid;
    LIQUIDITY -= MORTGAGE_COSTS[0];
    
    
    HOUSING_PAYMENT = 0;
    for (i = 0; i < 3; i++) {
        HOUSING_PAYMENT += MORTGAGE_COSTS[i];
    }
    
    /* Finish off very little amount of mortgages.
    if (MORTGAGES < LIQUIDITY) {
        total_principal_paid = 0;
        for (i = 0; i < size; i++) {
            mort = MORTGAGES_LIST.array[i];
            principal_paid = mort.principal;
            add_mortgage_payment_message(mort.bank_id, 0, principal_paid);
            total_principal_paid += principal_paid;
        }
        LIQUIDITY -= MORTGAGES;
        MORTGAGES = 0;
        MORTGAGE_COSTS[0] += total_principal_paid;
        HOUSING_PAYMENT += total_principal_paid;
        for (i = 0; i < size; i++) {remove_mortgage(&MORTGAGES_LIST, 0);}
    }
    */
    
    //free_mortgage(&mort);
	return 0; /* Returning zero means the agent is not removed */
}

FILE * hh_mfirst;
int hh_mfirst_open = 0;
/*
 * \fn: household_housing_debt_writeoff()
 * \brief: if household pays a significant amount of his income
 * to mortgages a debt write off occures and its mortgage
 * is restructured.
 */
int household_housing_debt_writeoff()
{
    int size, ind;
    double total_income;
    double pre_mortgages, writeoff;
    double quarterly_principal, quarterly_interest;
    
    size = MORTGAGES_LIST.size;
    if (size == 0){
        if (DATA_COLLECTION_MODE && COLLECT_HOUSEHOLD_DATA) {
            if(hh_mfirst_open == 0) {
                char * filename;
                filename = malloc(100*sizeof(char));
                filename[0]=0;
                strcpy(filename, "./outputs/data/Household_Monthly_FirstDay.txt");
                hh_mfirst = fopen(filename,"a");
                free(filename);
                hh_mfirst_open = 1;
            }
            double mcost = MORTGAGE_COSTS[0];
            fprintf(hh_mfirst,"%d %d %f %f %d %f %f %f\n",IT_NO, ID, MORTGAGES, mcost, HOUSING_UNITS, HOUSING_VALUE, EQUITY_RATIO, LIQUIDITY);
            /* Would be good to know when to close the file */
            //fclose(file1);
        }
        return 0;
    }
    
    //mortgage mort;
    //init_mortgage(&mort);
    
    total_income = LABOUR_INCOME + CAPITAL_INCOME;
    
    
    if (EXPECTED_HOUSING_PAYMENT > HOUSEHOLD_MORTGAGE_WRITEOFF_HIGH * total_income)
    {
        pre_mortgages = MORTGAGES;
        double annuity, d1, d2;
        double used_interest_rate;

        for (ind = 0; ind < size; ind++) {
            remove_mortgage(&MORTGAGES_LIST, 0);
        }
        
        quarterly_interest = 0;
        quarterly_principal = 0;
        if (MORTGAGE_CHOICE == 1) {
            d1 = MORTGAGES_INTEREST_RATE/4;
            d2 = d1 * pow((1 + d1), LOANTERM);
            annuity = 1/d1 - 1/d2;
            MORTGAGES = total_income * HOUSEHOLD_MORTGAGE_WRITEOFF_LOW * annuity;
            quarterly_interest = MORTGAGES * MORTGAGES_INTEREST_RATE / 4;
            quarterly_principal = (MORTGAGES / annuity) - quarterly_interest;
            add_mortgage(&MORTGAGES_LIST, BANK_ID, MORTGAGES, LOANTERM, quarterly_interest, quarterly_principal, MORTGAGES_INTEREST_RATE, MORTGAGE_CHOICE);
        }
        else if (MORTGAGE_CHOICE == 2){
            MORTGAGES = (total_income * HOUSEHOLD_MORTGAGE_WRITEOFF_LOW) / (MORTGAGES_INTEREST_RATE / 4 + 1.0/LOANTERM);
            quarterly_interest = MORTGAGES * MORTGAGES_INTEREST_RATE / 4;
            quarterly_principal = (MORTGAGES / LOANTERM);
            add_mortgage(&MORTGAGES_LIST, BANK_ID, MORTGAGES, LOANTERM, quarterly_interest, quarterly_principal, MORTGAGES_INTEREST_RATE, MORTGAGE_CHOICE);
        }
        else if (MORTGAGE_CHOICE == 3){
            if (USE_FIXED_RATE) {
                used_interest_rate = FIXED_RATE;
            } else {
                used_interest_rate = MORTGAGES_INTEREST_RATE - IIM_RATE_DISCOUNT; 
            }
            d1 = used_interest_rate/4;
            d2 = d1 * pow((1 + d1), LOANTERM);
            annuity = 1/d1 - 1/d2;
            MORTGAGES = total_income * HOUSEHOLD_MORTGAGE_WRITEOFF_LOW * annuity;
            quarterly_interest = MORTGAGES * used_interest_rate/4; 
            quarterly_principal = (MORTGAGES / annuity) - quarterly_interest;
            add_mortgage(&MORTGAGES_LIST, BANK_ID, MORTGAGES, LOANTERM, quarterly_interest, quarterly_principal, used_interest_rate, MORTGAGE_CHOICE);
        }
        else if (MORTGAGE_CHOICE == 4){
            if (USE_FIXED_RATE) {
                used_interest_rate = FIXED_RATE;
            } else {
                used_interest_rate = MORTGAGES_INTEREST_RATE - IIM_RATE_DISCOUNT; 
            }
            MORTGAGES = (total_income * HOUSEHOLD_MORTGAGE_WRITEOFF_LOW) / (used_interest_rate / 4 + 1.0/LOANTERM);
            quarterly_interest = MORTGAGES * used_interest_rate/4;
            quarterly_principal = (MORTGAGES / LOANTERM);
            add_mortgage(&MORTGAGES_LIST, BANK_ID, MORTGAGES, LOANTERM, quarterly_interest, quarterly_principal, used_interest_rate, MORTGAGE_CHOICE);
        }
        else if (MORTGAGE_CHOICE == 5){
            d1 = (MORTGAGES_INTEREST_RATE + 0.01)/4;
            d2 = d1 * pow((1 + d1), LOANTERM);
            annuity = 1/d1 - 1/d2;
            used_interest_rate = MORTGAGES_INTEREST_RATE + 0.01;
            MORTGAGES = total_income * HOUSEHOLD_MORTGAGE_WRITEOFF_LOW * annuity;
            quarterly_interest = MORTGAGES * used_interest_rate / 4;
            quarterly_principal = (MORTGAGES / annuity) - quarterly_interest;
            add_mortgage(&MORTGAGES_LIST, BANK_ID, MORTGAGES, LOANTERM, quarterly_interest, quarterly_principal, used_interest_rate, MORTGAGE_CHOICE);
        }
        else if (MORTGAGE_CHOICE == 6){
            used_interest_rate = MORTGAGES_INTEREST_RATE + 0.01;
            MORTGAGES = (total_income * HOUSEHOLD_MORTGAGE_WRITEOFF_LOW) / (used_interest_rate / 4 + 1.0/LOANTERM);
            quarterly_interest = MORTGAGES * (MORTGAGES_INTEREST_RATE + 0.01)/4;
            quarterly_principal = (MORTGAGES / LOANTERM);
            add_mortgage(&MORTGAGES_LIST, BANK_ID, MORTGAGES, LOANTERM, quarterly_interest, quarterly_principal, used_interest_rate, MORTGAGE_CHOICE);
        }
        else if (MORTGAGE_CHOICE == 7){
            if (USE_FIXED_RATE) {
                used_interest_rate = FIXED_RATE;
            } else {
                used_interest_rate = MORTGAGES_INTEREST_RATE - IIM_RATE_DISCOUNT; 
            }
            d1 = used_interest_rate/4;
            d2 = d1 * pow((1 + d1), LOANTERM);
            annuity = 1/d1 - 1/d2;
            /* The new principal is gotten by adjusting the principal of a new annuity with respect to the current inflation. */
            MORTGAGES = total_income * HOUSEHOLD_MORTGAGE_WRITEOFF_LOW * annuity / (1 + (annuity * QUARTERLY_PRICE_CHANGE));
            quarterly_interest = MORTGAGES * used_interest_rate/4 + MORTGAGES * QUARTERLY_PRICE_CHANGE;
            quarterly_principal = (MORTGAGES / annuity) - (MORTGAGES * (pow(1+EXPECTED_INFLATION_RATE, 1/4)-1));
            add_mortgage(&MORTGAGES_LIST, BANK_ID, MORTGAGES, LOANTERM, quarterly_interest, quarterly_principal, used_interest_rate, MORTGAGE_CHOICE);
        }
        else {
            if (WARNING_MODE) {
                printf("Warning @household_housing_debt_writeoff(): Unexpected mortgage choice = %d \n", MORTGAGE_CHOICE);
            }

        }
        /* Quarterly_Interest + Quarterly_Prinicipal = Quarterly_Income * Writeoff_Low
         */
        writeoff = pre_mortgages - MORTGAGES;
        EXPECTED_HOUSING_PAYMENT = quarterly_interest + quarterly_principal;

        if (WARNING_MODE) {
            if (writeoff < 0 || MORTGAGES < 0) {
                printf("Warning @household_housing_debt_writeoff(): Unexpected negative values detected, Household = %d, Total income = %f, Mortgage Costs= %f, Writeoff = %f, Pre Mortgage = %f New Mortgage = %f, interest = %f. 50 percent of the mortgage cost will be written off. \n", ID, total_income, HOUSING_PAYMENT, writeoff, pre_mortgages, MORTGAGES, MORTGAGES_INTEREST_RATE);
            }
            
            if (writeoff < 0) {
                MORTGAGES = pre_mortgages * 0.5;
                writeoff = pre_mortgages * 0.5;
            }

        }

        /* All mortgages are acquired from the same bank. */
        add_mortgage_writeoff_message(BANK_ID, writeoff);
        
        if (DATA_COLLECTION_MODE) {
            char * filename;
            FILE * file1;
            filename = malloc(100*sizeof(char));
            filename[0]=0;
            strcpy(filename, "./outputs/data/BankruptcyInspection.txt");
            file1 = fopen(filename,"a");
            fprintf(file1,"%d %d %s %s %d %f\n",IT_NO, ID, "Household", "Mortgage", BANK_ID, writeoff);
            fclose(file1);
            free(filename);
        }
        
        if (PRINT_DEBUG_MODE) {
            printf("Household ID = %d, Debts writeoff: %d mortgages affected. Loss on Bank ID = %d amounts to %f \n", ID, size, BANK_ID, writeoff);
        }
        

        /*if (MORTGAGES > 0) {
            double quarterly_principal, quarterly_interest;
            quarterly_interest = MORTGAGES * MORTGAGES_INTEREST_RATE / 4;
            quarterly_principal = MORTGAGES / annuity - quarterly_interest;
            add_mortgage(&MORTGAGES_LIST, BANK_ID, MORTGAGES, 160, quarterly_interest, quarterly_principal);
            EXPECTED_HOUSING_PAYMENT = quarterly_interest + quarterly_principal;
        }*/
    }
    
    if (DATA_COLLECTION_MODE && COLLECT_HOUSEHOLD_DATA) {
        if(hh_mfirst_open == 0) {
            char * filename;
            filename = malloc(100*sizeof(char));
            filename[0]=0;
            strcpy(filename, "./outputs/data/Household_Monthly_FirstDay.txt");
            hh_mfirst = fopen(filename,"a");
            free(filename);
            hh_mfirst_open = 1;
        }
        double mcost = MORTGAGE_COSTS[0];
        fprintf(hh_mfirst,"%d %d %f %f %d %f %f %f\n",IT_NO, ID, MORTGAGES, mcost, HOUSING_UNITS, HOUSING_VALUE, EQUITY_RATIO, LIQUIDITY);
//        fclose(file1);
    }
    
    //free_mortgage(&mort);
    
    
	return 0; /* Returning zero means the agent is not removed */
}

