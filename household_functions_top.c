#include "header.h"
#include "household_agent_header.h"


/*
 * \fn: int household_init_post_id()
 * \brief:
 */
int household_init_post_id()
{
    /*** Balancesheet Verification */
    char * filename;
    FILE * file1;
    filename = malloc(100*sizeof(char));
    filename[0]=0;
    strcpy(filename, "./outputs/data/Household_ID_Liquidity_Mortgages.txt");
    file1 = fopen(filename,"w");
    //fprintf(file1,"%d %f %f\n", ID, LIQUIDITY, MORTGAGES);
    fclose(file1);
    free(filename);
    
    add_household_jpoffice_id_message(ID);
    
	return 0; /* Returning zero means the agent is not removed */
}

/*
 * \fn: int household_init_employment()
 * \brief:
 */
int household_init_employment()
{
    START_JPOFFICE_HOUSEHOLD_EMPLOYER_MESSAGE_LOOP
    MY_EMPLOYER_ID = jpoffice_household_employer_message->employer_id;
    ISMANAGER = jpoffice_household_employer_message->position;
    FINISH_JPOFFICE_HOUSEHOLD_EMPLOYER_MESSAGE_LOOP
    
    if (MY_EMPLOYER_ID > 0) {WAGE = 5.0;}
    else {WAGE = 0.0;}
    
    for (int i = 0; i < 3; i++) {
        if (PREVIOUS_WAGES[i] == 0) {
            PREVIOUS_BENEFITS[i] = 4;
        } else {
            PREVIOUS_BENEFITS[i] = 1.5;
        }
    }
    
    
    /*** Balancesheet Verification */
    char * filename;
    FILE * file1;
    filename = malloc(100*sizeof(char));
    filename[0]=0;
    strcpy(filename, "./outputs/data/Household_ID_Liquidity_Mortgages.txt");
    file1 = fopen(filename,"a");
    fprintf(file1,"%d %f %f\n", ID, LIQUIDITY, MORTGAGES);
    fclose(file1);
    free(filename);
     

	return 0; /* Returning zero means the agent is not removed */
}


/*
 * \fn: int household_init_balancesheet()
 * \brief: 
 */
int household_init_balancesheet()
{
    /* The firms are initiliazed loans only with their preferred banks.
     */
    double d1, d2, annuity;
    double total_income = LABOUR_INCOME + CAPITAL_INCOME;
    //double mortgage_cost_income_ratio = 0.15;

    if (MORTGAGE_CHOICE == 1) {
        d1 = MORTGAGES_LIST.array[0].interestrate/4;
        d2 = d1 * pow((1 + d1), 160);
        annuity = 1/d1 - 1/d2;
        //MORTGAGES = total_income * mortgage_cost_income_ratio * annuity;
        MORTGAGES = TOTAL_ASSETS / 3;
        for (int i = 0; i < MORTGAGES_LIST.size ; i++) { 
            MORTGAGES_LIST.array[i].principal = MORTGAGES / MORTGAGES_LIST.size;
        }
    }
    else if (MORTGAGE_CHOICE == 2){
        //MORTGAGES = (total_income * mortgage_cost_income_ratio) / (MORTGAGES_LIST.array[0].interestrate / 4 + 1/160);
        MORTGAGES = TOTAL_ASSETS / 3;
        for (int i = 0; i < MORTGAGES_LIST.size ; i++) { 
            MORTGAGES_LIST.array[i].principal = MORTGAGES / MORTGAGES_LIST.size;
        }
    }
    else if (MORTGAGE_CHOICE == 3){
        d1 = 0.02/4;
        d2 = d1 * pow((1 + d1), 160);
        annuity = 1/d1 - 1/d2;
        //MORTGAGES = total_income * mortgage_cost_income_ratio * annuity;
        MORTGAGES = TOTAL_ASSETS / 3;
        for (int i = 0; i < MORTGAGES_LIST.size ; i++) { 
            MORTGAGES_LIST.array[i].principal = MORTGAGES / MORTGAGES_LIST.size;
            MORTGAGES_LIST.array[i].interestrate = 0.02;
        }
    }
    else if (MORTGAGE_CHOICE == 4){
        //MORTGAGES = (total_income * mortgage_cost_income_ratio) / (0.02 / 4 + 1/160);
        MORTGAGES = TOTAL_ASSETS / 3;
        for (int i = 0; i < MORTGAGES_LIST.size ; i++) { 
            MORTGAGES_LIST.array[i].principal = MORTGAGES / MORTGAGES_LIST.size;
            MORTGAGES_LIST.array[i].interestrate = 0.02;
        }
    }
    else if (MORTGAGE_CHOICE == 5){
        d1 = (MORTGAGES_LIST.array[0].interestrate + 0.01)/4;
        d2 = d1 * pow((1 + d1), 160);
        annuity = 1/d1 - 1/d2;
        //MORTGAGES = total_income * mortgage_cost_income_ratio * annuity;
        MORTGAGES = TOTAL_ASSETS / 3;
        for (int i = 0; i < MORTGAGES_LIST.size ; i++) { 
            MORTGAGES_LIST.array[i].principal = MORTGAGES / MORTGAGES_LIST.size;
            MORTGAGES_LIST.array[i].interestrate += 0.01;
        }
    }
    else if (MORTGAGE_CHOICE == 6){
        //MORTGAGES = (total_income * mortgage_cost_income_ratio) / ((MORTGAGES_LIST.array[0].interestrate + 0.01) / 4 + 1/160);
        MORTGAGES = TOTAL_ASSETS / 3;
        for (int i = 0; i < MORTGAGES_LIST.size ; i++) { 
            MORTGAGES_LIST.array[i].principal = MORTGAGES / MORTGAGES_LIST.size;
            MORTGAGES_LIST.array[i].interestrate += 0.01;
        }
    }
    else if (MORTGAGE_CHOICE == 7){
        d1 = 0.02/4;
        d2 = d1 * pow((1 + d1), 160);
        annuity = 1/d1 - 1/d2;
        //MORTGAGES = total_income * mortgage_cost_income_ratio * annuity / (1 + (annuity * 0.02/4));
        MORTGAGES = TOTAL_ASSETS / 3;
        for (int i = 0; i < MORTGAGES_LIST.size ; i++) { 
            MORTGAGES_LIST.array[i].principal = MORTGAGES / MORTGAGES_LIST.size;
            MORTGAGES_LIST.array[i].interestrate = 0.02;
        }
    }
    else {
        if (WARNING_MODE) {
            printf("Warning @household_housing_debt_writeoff(): Unexpected mortgage choice = %d \n", MORTGAGE_CHOICE);
        }

    }


    EQUITY = TOTAL_ASSETS - MORTGAGES;
    EQUITY_RATIO = EQUITY / TOTAL_ASSETS;

    add_household_bank_init_mortgages_message(BANK_ID, MORTGAGES);
    add_household_bank_init_deposit_message(BANK_ID, LIQUIDITY);

    //if (MORTGAGE_CHOICE == 3 || MORTGAGE_CHOICE == 4 || MORTGAGE_CHOICE == 7){
    //    for (int i = 0; i < MORTGAGES_LIST.size ; i++) { 
    //    MORTGAGES_LIST.array[i].interestrate = 0.02;
    //    }
    //}
    //else if (MORTGAGE_CHOICE == 5 || MORTGAGE_CHOICE == 6) {
    //    for (int i = 0; i < MORTGAGES_LIST.size; i++) { 
    //    MORTGAGES_LIST.array[i].interestrate += 0.01;
    //    }
    //}

	return 0; /* Returning zero means the agent is not removed */
}

/*
 * \fn: int household_iterate()
 * \brief: Resumes its regular functions.
 */
int household_iterate()
{
    if (DATA_COLLECTION_MODE && COLLECT_HOUSEHOLD_DATA) {
        if (IT_NO == 0) {
            char * filename;
            FILE * file1;
            filename = malloc(100*sizeof(char));
            
            /* Writing the column names. Make sure that an household with that ID does exist. */
            if (ID == 542)  {
                
                /* @\fn: int household_consumption_recieve_goods() */
                filename[0]=0;
                strcpy(filename, "./outputs/data/Household_Weekly.txt");
                file1 = fopen(filename,"w");
                fprintf(file1,"%s %s %s %s %s %s %s\n","IT_NO", "ID", "LIQUIDITY", "WEEKLY_CONSUMPTION_BUDGET", "money_to_spend", "money_spent", "quantity_bought");
                //fprintf(file1,"%d %d %f %f %f %f %d\n",IT_NO, ID, LIQUIDITY, WEEKLY_CONSUMPTION_BUDGET, 0.0, 0.0, 0);
                fclose(file1);
                
                /* @\fn: household_housing_debt_writeoff() */
                filename[0]=0;
                strcpy(filename, "./outputs/data/Household_Monthly_FirstDay.txt");
                file1 = fopen(filename,"w");
                fprintf(file1,"%s %s %s %s %s %s %s %s\n", "IT_NO", "ID", "MORTGAGES", "MORTGAGE_COST", "HOUSING_UNITS", "HOUSING_VALUE", "EQUITY_RATIO", "LIQUIDITY");
                //fprintf(file1,"%d %d %f %f %d %f %f %f\n",IT_NO, ID, MORTGAGES, MORTGAGE_COSTS[0], HOUSING_UNITS, HOUSING_VALUE, EQUITY_RATIO, LIQUIDITY);
                fclose(file1);
                
                /* @\fn: int household_credit_collect_benefits() */
                filename[0]=0;
                strcpy(filename, "./outputs/data/Household_Monthly_LastDay.txt");
                file1 = fopen(filename,"w");
                fprintf(file1,"%s %s %s %s %s %s\n","IT_NO", "ID", "MY_EMPLOYER_ID", "WAGE", "unemployment_benefit", "general_benefit");
                //fprintf(file1,"%d %d %d %f %f %f\n",IT_NO, ID, MY_EMPLOYER_ID, WAGE, 0.0, 0.0);
                fclose(file1);
                
                /* @\fn: int household_credit_do_balance_sheet() */
                filename[0]=0;
                strcpy(filename, "./outputs/data/Household_Quarterly.txt");
                file1 = fopen(filename,"w");
                fprintf(file1,"%s %s %s %s %s %s %s %s %s %s %s\n","IT_NO", "ID", "TOTAL_ASSETS", "LIQUIDITY", "HOUSING_VALUE", "LABOUR_INCOME", "BENEFITS", "CAPITAL_INCOME", "MORTGAGES", "HOUSING_PAYMENT", "EQUITY");
                //fprintf(file1,"%d %d %f %f %f %f %f %f %f %f %f\n",IT_NO, ID, TOTAL_ASSETS, LIQUIDITY, HOUSING_VALUE, LABOUR_INCOME, GOVERNMENT_BENEFITS, CAPITAL_INCOME, MORTGAGES, HOUSING_PAYMENT, EQUITY);
                fclose(file1);
                
                free(filename);
            }
        }
    }
    
    IT_NO++;
	return 0; /* Returning zero means the agent is not removed */
}

/*
 * \fn: int household_update_bank_account()
 * \brief: puts money to deposit account of its prefered bak. */
int household_update_bank_account()
{
    if (LIQUIDITY > 0) {
        add_household_bank_update_deposit_message(BANK_ID, LIQUIDITY);
    }
    
    if (PRINT_DEBUG_MODE) {
        if (ID > 40 && ID < 60) {
            printf("Household ID = %d has a liquidity amount = %f deposited to bank.\n", ID, LIQUIDITY);
        }
        
    }
	return 0; /* Returning zero means the agent is not removed */
}