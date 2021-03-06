#include "../header.h"
#include "../household_agent_header.h"

/*
 * \fn: int household_credit_check_interest_rate()
 * \brief: Household hecks quarterly interest rate and
 * sets the mortgage interest rates accordingly.
 */
int household_credit_check_interest_rate()
{

    START_INTEREST_RATE_MESSAGE_LOOP
    /* 2 percent increase determined by the model. */
    MORTGAGES_INTEREST_RATE = interest_rate_message->rate + 0.02;
    
	FINISH_INTEREST_RATE_MESSAGE_LOOP

	return 0; /* Returning zero means the agent is not removed */
}

/*
 * \fn: int household_credit_check_tax_rate()
 * \brief:
 */
int household_credit_check_tax_rate()
{
    START_LABOUR_TAX_RATE_MESSAGE_LOOP
    LABOUR_TAX_RATE = labour_tax_rate_message->value;
	FINISH_LABOUR_TAX_RATE_MESSAGE_LOOP
    
	return 0; /* Returning zero means the agent is not removed */
}

/*
 * \fn: int household_credit_update_mortgage_rates()
 * \brief:
 */
int household_credit_update_mortgage_rates()
{
    int size, i, quarters_left;
    //mortgage mort;
    //init_mortgage(&mort);
    double principal;
    
    size = MORTGAGES_LIST.size;
    i = 0;

    while (i < size) {
        //mort = MORTGAGES_LIST.array[i];
        //principal = mort.principal;
        principal = MORTGAGES_LIST.array[i].principal;
        quarters_left = MORTGAGES_LIST.array[i].quarters_left;
        
        if (principal < 0.1) {
            remove_mortgage(&MORTGAGES_LIST, i);
            size--;
            continue;
        }
        
        if (quarters_left == 0){
            if (principal >= 0.1) {
                MORTGAGES_LIST.array[i].quarters_left = 1;
                if (WARNING_MODE) {
                    printf("Warning @household_credit_update_mortgage_rates(): Mortgage life is over but the debt is not finished! Household = %d, The mortgage = %f\n", ID, principal);
                }
            }
            else{
                remove_mortgage(&MORTGAGES_LIST, i);
                size--;
                continue;
            }
        }
        i++;
    }
    
    size = MORTGAGES_LIST.size;;
    if (size == 0) {
        //free_mortgage(&mort);
        return 0;
    }
    
    double new_quarterly_interest;
    double new_quarterly_principal, quarterly_principal;
    double annuity;
    double d1, d2;
    double rate;
    double principal_change = 0;
    
    new_quarterly_interest = 0;
    new_quarterly_principal = 0;
    EXPECTED_HOUSING_PAYMENT = 0;
    
    for (i = 0; i < size; i++) {
        //mort = MORTGAGES_LIST.array[i];
        //principal = mort.principal;
        //quarters_left = mort.quarters_left - 1;
        //rate = mort.interestrate;
        
        principal = MORTGAGES_LIST.array[i].principal;
        quarters_left = MORTGAGES_LIST.array[i].quarters_left - 1;
        rate = MORTGAGES_LIST.array[i].interestrate;
        quarterly_principal = MORTGAGES_LIST.array[i].quarterly_principal;
        
        
        if (MORTGAGE_CHOICE == 1) {
            d1 = MORTGAGES_INTEREST_RATE/4;
            d2 = d1 * pow((1 + d1), quarters_left);
            annuity = 1/d1 - 1/d2;
            new_quarterly_interest = principal * d1;
            new_quarterly_principal = (principal / annuity) - new_quarterly_interest;

        }
        else if (MORTGAGE_CHOICE == 2){
            new_quarterly_interest = principal * MORTGAGES_INTEREST_RATE / 4;
            new_quarterly_principal = quarterly_principal;
        }
        else if (MORTGAGE_CHOICE == 3){
            d1 = rate/4;
            d2 = d1 * pow((1 + d1), quarters_left);
            annuity = 1/d1 - 1/d2;
            principal_change = principal * QUARTERLY_PRICE_CHANGE;
            principal += principal_change;
            MORTGAGES += principal_change;
            new_quarterly_interest = principal * d1;
            new_quarterly_principal = (principal / annuity) - new_quarterly_interest;
        }
        else if (MORTGAGE_CHOICE == 4){
            principal_change = principal * QUARTERLY_PRICE_CHANGE;
            principal += principal_change;
            MORTGAGES += principal_change;
            new_quarterly_interest = principal * rate/4;
            new_quarterly_principal = principal / quarters_left;
        }
        else if (MORTGAGE_CHOICE == 5){
            d1 = rate/4;
            d2 = d1 * pow((1 + d1), quarters_left);
            annuity = 1/d1 - 1/d2;

            new_quarterly_interest = principal * d1;
            new_quarterly_principal = principal / annuity - new_quarterly_interest;
        }
        else if (MORTGAGE_CHOICE == 6){
            new_quarterly_interest = principal * rate/4;
            new_quarterly_principal = quarterly_principal;
        }
        /* MORTGAGE_CHOICE 7 is a mortgage where indexation is paid right away */
        else if (MORTGAGE_CHOICE == 7){
            d1 = rate/4;
            d2 = d1 * pow((1 + d1), quarters_left);
            annuity = 1/d1 - 1/d2;
            /* Interest payement includes index adjustment */
            new_quarterly_interest = principal * rate/4 + principal * QUARTERLY_PRICE_CHANGE;
            new_quarterly_principal = (principal / annuity) - (principal * rate/4);
        }
        else {
            if (WARNING_MODE) {
                printf("Warning @household_housing_buy(): Unexpected mortgage choice = %d \n", MORTGAGE_CHOICE);
            }
        }
        
        MORTGAGES_LIST.array[i].quarters_left = quarters_left;
        MORTGAGES_LIST.array[i].quarterly_interest = new_quarterly_interest;
        MORTGAGES_LIST.array[i].quarterly_principal = new_quarterly_principal;
        MORTGAGES_LIST.array[i].principal = principal;
        
        EXPECTED_HOUSING_PAYMENT += new_quarterly_interest + new_quarterly_principal;
    }
    
    //free_mortgage(&mort);
	return 0; /* Returning zero means the agent is not removed */
}

/*
 * \fn: int household_credit_collect_shares()
 * \brief: Collecting shares from the firms via Equity Fund.
 */
int household_credit_collect_shares()
{
    
    CAPITAL_INCOME = 0;
    
    START_HOUSEHOLD_SHARE_MESSAGE_LOOP
    CAPITAL_INCOME += N_SHARES * household_share_message->amount;
	FINISH_HOUSEHOLD_SHARE_MESSAGE_LOOP
    
	return 0; /* Returning zero means the agent is not removed */
}

FILE * hh_q;
int hh_q_open = 0;
/*
 * \fn: int household_credit_do_balance_sheet()
 * \brief:
 */
int household_credit_do_balance_sheet()
{
    /* Updating value of housing assets.
     It is up to date. Update is done monthly.
     */
    // HOUSING_VALUE = HOUSING_UNITS * HOUSING_PRICE;
    
    /* Liquidity contains labour incomes. */
    TOTAL_ASSETS = LIQUIDITY +  HOUSING_VALUE + CAPITAL_INCOME;
    EQUITY = TOTAL_ASSETS - MORTGAGES;

    if (DATA_COLLECTION_MODE && COLLECT_HOUSEHOLD_DATA) {
        if(hh_q_open == 0)
        {
            char * filename;
            filename = malloc(100*sizeof(char));
            filename[0]=0;
            strcpy(filename, "./outputs/data/Household_Quarterly.txt");
            
            hh_q = fopen(filename,"a");
            free(filename);
            hh_q_open = 1;
        }
        fprintf(hh_q,"%d %d %f %f %f %f %f %f %f %f %f\n",IT_NO, ID, TOTAL_ASSETS, LIQUIDITY, HOUSING_VALUE, LABOUR_INCOME, GOVERNMENT_BENEFITS, CAPITAL_INCOME, MORTGAGES, HOUSING_PAYMENT, EQUITY);
        // fclose(file1);
    }
    
    /* Shares are liquidified. */
    LIQUIDITY += CAPITAL_INCOME;
    
	return 0; /* Returning zero means the agent is not removed */
}

FILE * hh_mlast;
int hh_mlast_open = 0;
/*
 * \fn: int household_credit_collect_benefits()
 * \brief: Collect general transfer benefits and/or unemployment benefits from the government.
 */
int household_credit_collect_benefits()
{
    double general_benefit = 0;
    
    START_GENERAL_BENEFIT_MESSAGE_LOOP
    general_benefit = general_benefit_message->amount;
    LIQUIDITY += general_benefit;
    GOVERNMENT_BENEFITS = general_benefit;
	FINISH_GENERAL_BENEFIT_MESSAGE_LOOP

    double unemployment_benefit = 0;
    
    if (MY_EMPLOYER_ID == 0) {
        START_UNEMPLOYMENT_BENEFIT_MESSAGE_LOOP
        unemployment_benefit = unemployment_benefit_message->amount;
        LIQUIDITY += unemployment_benefit;
        GOVERNMENT_BENEFITS += unemployment_benefit;
        FINISH_UNEMPLOYMENT_BENEFIT_MESSAGE_LOOP
    }
    
    /* Updating history of benefits */
    PREVIOUS_BENEFITS[2] = PREVIOUS_BENEFITS[1];
    PREVIOUS_BENEFITS[1] = PREVIOUS_BENEFITS[0];
    PREVIOUS_BENEFITS[0] = GOVERNMENT_BENEFITS;
    
    if (DATA_COLLECTION_MODE && COLLECT_HOUSEHOLD_DATA) {
        if(hh_mlast_open == 0) {
            char * filename;
            filename = malloc(100*sizeof(char));
            filename[0]=0;
            strcpy(filename, "./outputs/data/Household_Monthly_LastDay.txt");
            hh_mlast = fopen(filename,"a");
            free(filename);
            hh_mlast_open = 1;
        }
        fprintf(hh_mlast,"%d %d %d %f %f %f\n",IT_NO, ID, MY_EMPLOYER_ID, WAGE, unemployment_benefit, general_benefit);
        // fclose(file1);
        
    }
    
	return 0; /* Returning zero means the agent is not removed */
}

/*
 * \fn: int household_credit_mortgage_update()
 * \brief: Update mortgage principal of mortgage type 3 and 4 due to inflation
 */
int household_credit_mortgage_update()
{
    double principal_change = 0;
    double principal;
    int size = MORTGAGES_LIST.size; 

    for (int i = 0; i < size; i++) {
        
        if (MORTGAGE_CHOICE == 3 || MORTGAGE_CHOICE == 4){
            principal = MORTGAGES_LIST.array[i].principal;
            principal_change = principal * QUARTERLY_PRICE_CHANGE;
            add_mortgage_update_message(BANK_ID, principal_change);
        }
    }    
    return 0; /* Returning zero means the agent is not removed */
}

