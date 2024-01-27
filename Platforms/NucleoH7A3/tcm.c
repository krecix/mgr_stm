/*
 * tcm.c
 *
 *  Created on: 9 sie 2020
 *      Author: krzysztof
 */

#include "string.h"

// Przykład użycia funkcji w ITCM
// void __attribute__((section(".itcm_text"))) SystemClock_Config(void);
// Przykład użycia danych w DTCM
// uint32_t __attribute__((section(".dtcm_data"))) abc[100];

// Load fast-executing code to ITCM RAM
void copyToITCM_RAM(void) {
    /* Load functions into ITCM RAM */
    extern unsigned char itcm_text_start;
    extern unsigned char itcm_text_end;
    extern unsigned char itcm_data;
    memcpy(&itcm_text_start, &itcm_data,
           (int) (&itcm_text_end - &itcm_text_start));
}

// Load fast-executing data to ITCM RAM
void copyToDTCM_RAM(void) {
    /* Load data into DTCM RAM */
    extern unsigned char dtcm_data_start;
    extern unsigned char dtcm_data_end;
    extern unsigned char dtcm_data;
    memcpy(&dtcm_data_start, &dtcm_data,
           (int) (&dtcm_data_end - &dtcm_data_start));
}
