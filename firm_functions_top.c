#include "header.h"
#include "firm_agent_header.h"
#include <math.h>

/*
 * \fn: int firm_init_post_id()
 * \brief:
 */
int firm_init_post_id()
{
    add_firm_jpoffice_id_message(ID, ISCONSTRUCTOR);
    
    /*** Balancesheet Verification. */
    if (DATA_COLLECTION_MODE) {
        char * filename;
        FILE * file1;
        filename = malloc(100*sizeof(char));
        filename[0]=0;
        strcpy(filename, "./outputs/data/Firm_ID_Liquidity_Loan.txt");
        file1 = fopen(filename,"w");
        //fprintf(file1,"%d %f %f\n",ID, LIQUIDITY, DEBT);
        fclose(file1);
        free(filename);
    }
    
    
	return 0; /* Returning zero means the agent is not removed */
}

/*
 * \fn: int firm_init_employment()
 * \brief:
 */
int firm_init_employment()
{
    int new_employee, position;
        
    START_JPOFFICE_FIRM_EMPLOYEE_MESSAGE_LOOP
    new_employee = jpoffice_firm_employee_message->employee_id;
    position = jpoffice_firm_employee_message->position;
    if (position > 0) {
        MANAGER = new_employee;
    }
    add_int(&EMPLOYEES, new_employee);
    FINISH_JPOFFICE_FIRM_EMPLOYEE_MESSAGE_LOOP
    
    
    CAPITAL_GOODS_PRICE = 100 * UNIT_GOODS_PRICE;
    
    
    NO_EMPLOYEES = EMPLOYEES.size;
    EMPLOYEES_NEEDED = NO_EMPLOYEES;
    LABOUR_COSTS = NO_EMPLOYEES * WAGE_OFFER;
    DEBT = (NO_EMPLOYEES * WAGE_OFFER * 0.2) / LOANS_INTEREST_RATE;
    TOTAL_INTEREST_PAYMENTS = DEBT * (LOANS_INTEREST_RATE / 4);
    OPERATING_COSTS = LABOUR_COSTS + TOTAL_INTEREST_PAYMENTS;
    EQUITY = DEBT / FIRM_STARTUP_LEVERAGE;
    TOTAL_ASSETS = DEBT + EQUITY;
    LOAN_LIST[0].amount = DEBT;
    
    /* The value is used to set a large (inf) number for regular firms.*/
    CAPITAL_PRODUCTIVITY = (double) 999999;
    
    if (ISCONSTRUCTOR) {
        
        PRODUCTION_CURRENT = (int) (NO_EMPLOYEES * LABOUR_PRODUCTIVITY / 12);
        INVENTORY = PRODUCTION_CURRENT;
        SALES = 2;
        REVENUES = SALES * UNIT_HOUSE_PRICE;
        CAPITAL_GOODS = ceil((TOTAL_ASSETS - UNIT_HOUSE_PRICE * INVENTORY - LIQUIDITY)/CAPITAL_GOODS_PRICE);
        CAPITAL_PRODUCTIVITY = LABOUR_PRODUCTIVITY * NO_EMPLOYEES / (0.7 * CAPITAL_GOODS);
        
    } else {
        
        PRODUCTION_CURRENT = (int) (NO_EMPLOYEES * LABOUR_PRODUCTIVITY);
        INVENTORY = (int) (PRODUCTION_CURRENT / 10);
        SALES = PRODUCTION_CURRENT;
        REVENUES = SALES * UNIT_GOODS_PRICE;
        CAPITAL_GOODS = ceil((TOTAL_ASSETS - UNIT_GOODS_PRICE * INVENTORY - LIQUIDITY)/CAPITAL_GOODS_PRICE);
    }
    
    EBIT = REVENUES - LABOUR_COSTS;
    NET_EARNINGS = EBIT - TOTAL_INTEREST_PAYMENTS;
    LIQUIDITY += NET_EARNINGS;
    
    if (PRINT_DEBUG_MODE) {
        printf("Firm %d --> Size = %d \n", ID, NO_EMPLOYEES);
    }
    
	return 0; /* Returning zero means the agent is not removed */
}

/*
 * \fn: int firm_init_balancesheet()
 * \brief:
 */
int firm_init_balancesheet()
{
   /*The firms are initiliazed loans only with their preferred banks. 
    */
    add_firm_bank_init_loans_message(BANK_ID, LOAN_LIST[0].amount);
    
    add_firm_bank_init_deposit_message(BANK_ID, LIQUIDITY);
    
    /*** Balancesheet Verification. */
    if (DATA_COLLECTION_MODE) {
        char * filename;
        FILE * file1;
        filename = malloc(100*sizeof(char));
        filename[0]=0;
        strcpy(filename, "./outputs/data/Firm_ID_Liquidity_Loan.txt");
        file1 = fopen(filename,"a");
        fprintf(file1,"%d %f %f\n",ID, LIQUIDITY, DEBT);
        fclose(file1);
        free(filename);

    }
    
    
	return 0; /* Returning zero means the agent is not removed */
}

/*
 * \fn: int firm_iterate()
 * \brief:
 */
int firm_iterate()
{
    if (DATA_COLLECTION_MODE && COLLECT_FIRM_DATA) {
        
        if (IT_NO == 0)
        {
            
            char * filename;
            FILE * file1;
            filename = malloc(140*sizeof(char));
            
            if (ISCONSTRUCTOR) {
                /* @\fn: firm_production_construction_plan() */
                filename[0]=0;
                strcpy(filename, "./outputs/data/Constructor_Firm_Monthly.txt");
                file1 = fopen(filename,"w");
                fprintf(file1,"%s %s %s %s %s %s %s %s %s %s\n","IT_NO", "ID", "WAGE_OFFER", "NO_EMPLOYEES", "EMPLOYEES_NEEDED", "SALES", "INVENTORY", "PRODUCTION_CURRENT", "PRODUCTION_PLAN", "UNIT_HOUSE_PRICE");
                fclose(file1);
                
                
                /* @\fn: firm_credit_compute_income_statement() */
                filename[0]=0;
                strcpy(filename, "./outputs/data/Constructor_Firm_Quarterly_IncomeStatement.txt");
                file1 = fopen(filename,"w");
                fprintf(file1,"%s %s %s %s %s %s %s %s\n","IT_NO", "ID", "REVENUES", "OPERATING_COSTS", "LABOUR_COSTS", "TOTAL_INTEREST_PAYMENTS", "EBIT", "NET_EARNINGS");
                fclose(file1);
                
                
                /* @\fn: firm_credit_pay_dividends() */
                filename[0]=0;
                strcpy(filename, "./outputs/data/Constructor_Firm_Quarterly_Dividends.txt");
                file1 = fopen(filename,"w");
                fprintf(file1,"%s %s %s %s\n","IT_NO", "ID", "DIVIDENDS_PAID", "DIVIDENDS_TO_BE_PAID");
                fclose(file1);
                
                /* @\fn: firm_credit_do_balance_sheet() */
                filename[0]=0;
                strcpy(filename, "./outputs/data/Constructor_Firm_Quarterly_BalanceSheet.txt");
                file1 = fopen(filename,"w");
                fprintf(file1,"%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s\n","IT_NO", "ID", "ISLIQUIDSHORT", "HASLOAN", "HASINVESTMENT", "LIQUIDITY_NEED", "ISINSOLVENT", "ISILLIQUID", "TOTAL_ASSETS", "LIQUIDITY", "INVENTORY", "UNIT_HOUSE_PRICE", "CAPITAL_GOODS_PRICE", "CAPITAL_GOODS",  "DEBT", "EQUITY");
                fclose(file1);
            }
            else
            {
                /* @\fn: firm_production_plan() */
                filename[0]=0;
                strcpy(filename, "./outputs/data/Firm_Monthly.txt");
                file1 = fopen(filename,"w");
                fprintf(file1,"%s %s %s %s %s %s %s %s %s %s\n","IT_NO", "ID", "WAGE_OFFER", "NO_EMPLOYEES", "EMPLOYEES_NEEDED", "SALES", "INVENTORY", "PRODUCTION_CURRENT", "PRODUCTION_PLAN", "UNIT_GOODS_PRICE");
                fclose(file1);
                
                
                /* @\fn: firm_credit_compute_income_statement() */
                filename[0]=0;
                strcpy(filename, "./outputs/data/Firm_Quarterly_IncomeStatement.txt");
                file1 = fopen(filename,"w");
                fprintf(file1,"%s %s %s %s %s %s %s %s\n","IT_NO", "ID", "REVENUES", "OPERATING_COSTS", "LABOUR_COSTS", "TOTAL_INTEREST_PAYMENTS", "EBIT", "NET_EARNINGS");
                fclose(file1);
                
                
                /* @\fn: firm_credit_pay_dividends() */
                filename[0]=0;
                strcpy(filename, "./outputs/data/Firm_Quarterly_Dividends.txt");
                file1 = fopen(filename,"w");
                fprintf(file1,"%s %s %s %s\n","IT_NO", "ID", "DIVIDENDS_PAID", "DIVIDENDS_TO_BE_PAID");
                fclose(file1);
                
                /* @\fn: firm_credit_do_balance_sheet() */
                filename[0]=0;
                strcpy(filename, "./outputs/data/Firm_Quarterly_BalanceSheet.txt");
                file1 = fopen(filename,"w");
                fprintf(file1,"%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s\n","IT_NO", "ID", "ISLIQUIDSHORT", "HASLOAN", "HASINVESTMENT", "LIQUIDITY_NEED","ISINSOLVENT", "ISILLIQUID", "TOTAL_ASSETS", "LIQUIDITY", "INVENTORY", "UNIT_GOODS_PRICE", "CAPITAL_GOODS_PRICE", "CAPITAL_GOODS", "DEBT", "EQUITY");
                fclose(file1);
            }
            free(filename);
        }
    }
    
    
    IT_NO++;
	return 0; /* Returning zero means the agent is not removed */
}

/*
 * \fn: int firm_update_bank_account()
 * \brief: puts money to deposit account of its prefered bak. */
int firm_update_bank_account()
{
    if (LIQUIDITY > 0) {
        add_firm_bank_update_deposit_message(BANK_ID, LIQUIDITY);
    }
    
    if (PRINT_DEBUG_MODE) {
        printf("Firm ID = %d a liquidity amount = %f has deposited to bank.\n", ID, LIQUIDITY);
    }
    
	return 0; /* Returning zero means the agent is not removed */
}
