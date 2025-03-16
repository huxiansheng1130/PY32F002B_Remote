/*********************************************************************************************************
 * @file      ntc_driver.c
 *
 * @details
 *
 * @author    huzhuohuan
 * @date      2025-03-06
 * @version   V_1.0
 ********************************************************************************************************/

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "ntc_driver.h"

#if (NTC_SMAPLING_ENABLE)
/*============================================================================*
 *                              Global Variables
 *============================================================================*/

/*============================================================================*
 *                              Function Definitions
 *============================================================================*/
/**
 * @brief   Configures the ADC to read the NTC thermistor's resistance.
 * @details Sets up the ADC to sample the analog voltage from the NTC thermistor.
 * @param   None
 * @retval  None
 */
static void ntc_adc_config(void)
{
    /* Enable ADC1 clock */
    LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_ADC1);

    /* Enable GPIOB clock */
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);

    /* Configure PB0 pin in analog input mode */
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_0, LL_GPIO_MODE_ANALOG);

    /* Set ADC clock to pclk/8 */
    LL_ADC_SetClock(ADC1, LL_ADC_CLOCK_SYNC_PCLK_DIV64);

    /* Set ADC resolution to 12 bit */
    LL_ADC_SetResolution(ADC1, LL_ADC_RESOLUTION_12B);

    /* ADC conversion data alignment: right aligned */
    LL_ADC_SetDataAlignment(ADC1, LL_ADC_DATA_ALIGN_RIGHT);

    /* No ADC low power mode activated */
    LL_ADC_SetLowPowerMode(ADC1, LL_ADC_LP_MODE_NONE);

    /* Sampling time 239.5 ADC clock cycles */
    LL_ADC_SetSamplingTimeCommonChannels(ADC1, LL_ADC_SAMPLINGTIME_239CYCLES_5);

    /* ADC regular group conversion trigger from external IP: TIM1 TRGO. */
    LL_ADC_REG_SetTriggerSource(ADC1, USER_ADC_SMAP_MODE);

#if (NTC_INTERRUPT_SMAP_ENABLE)
    /* Set Trigger edge to rising edge */
    LL_ADC_REG_SetTriggerEdge(ADC1, LL_ADC_REG_TRIG_EXT_RISING);
#endif

    /* Set ADC conversion mode to single mode: one conversion per trigger */
    LL_ADC_REG_SetContinuousMode(ADC1, LL_ADC_REG_CONV_CONTINUOUS);

    /* ADC regular group behavior in case of overrun: data overwritten */
    LL_ADC_REG_SetOverrun(ADC1, LL_ADC_REG_OVR_DATA_OVERWRITTEN);

    /* Disable ADC regular group sequencer discontinuous mode  */
    LL_ADC_REG_SetSequencerDiscont(ADC1, LL_ADC_REG_SEQ_DISCONT_DISABLE);

    /*
     * PA3:ADC_IN1, PA4:ADC_IN2, PA6:ADC_IN3, PA7:ADC_IN4,
     * PC0:ADC_IN5, PB6:ADC_IN6, PB0:ADC_IN7
     */
    LL_ADC_REG_SetSequencerChannels(ADC1, LL_ADC_CHANNEL_7);

    /* Dose not enable internal conversion channel */
    LL_ADC_SetCommonPathInternalCh(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_PATH_INTERNAL_NONE);

#if (NTC_INTERRUPT_SMAP_ENABLE)
    /* Enable EOC IT */
    LL_ADC_EnableIT_EOC(ADC1);

    NVIC_SetPriority(ADC_COMP_IRQn, 0);
    NVIC_EnableIRQ(ADC_COMP_IRQn);
#endif
}

#if (NTC_INTERRUPT_SMAP_ENABLE)
/**
 * @brief   Initializes the timing-related configurations for the NTC thermistor reading process.
 * @details This function sets up any necessary timers or delays that are required for the NTC reading process.
 * @param   None
 * @retval  None
 */
static void ntc_time_init(void)
{
    /* Enable TIM1 clock */
    LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_TIM1);

    /* Set TIM1 prescale */
    LL_TIM_SetPrescaler(TIM1, 6000);

    /* Set TIM1 auto-reload value */
    LL_TIM_SetAutoReload(TIM1, 4000);

    /* TIM1 Update event is used as trigger output */
    LL_TIM_SetTriggerOutput(TIM1, LL_TIM_TRGO_UPDATE);

    /* Enable TIM1 */
    LL_TIM_EnableCounter(TIM1);
}
#endif

/**
 * @brief   Calibrates the ADC for accurate temperature measurements.
 * @details Calibrates the ADC to ensure that the conversion from resistance values to temperature is accurate.
 *          This function is typically called once during the initialization phase of the application.
 * @param   None
 * @retval  None
 */
static void ntc_adc_calibrate(void)
{
#if (USE_TIMEOUT == 1)
    uint32_t Timeout = 0;
#endif

    if (LL_ADC_IsEnabled(ADC1) == 0)
    {
        /* Enable ADC calibration */
        LL_ADC_StartCalibration(ADC1);

#if (USE_TIMEOUT == 1)
        Timeout = ADC_CALIBRATION_TIMEOUT_MS;
#endif

        while (LL_ADC_IsCalibrationOnGoing(ADC1) != 0)
        {
#if (USE_TIMEOUT == 1)
            /* Detects if the calibration has timed out */
            if (LL_SYSTICK_IsActiveCounterFlag())
            {
                if (Timeout-- == 0)
                {
                }
            }
#endif
        }

        /* The delay between the end of ADC calibration and ADC enablement is at least 4 ADC clocks */
        WaitMs(1);
    }
}

/**
 * @brief   Initializes the NTC thermistor scanning functionality.
 * @details This function sets up the ADC and any necessary peripherals for scanning the NTC thermistor.
 *          It also initializes the variables used to store ADC readings and calculate temperature.
 * @param   None
 * @retval  None
 */
void ntc_smapling_init(void)
{
    ntc_adc_config();

    ntc_adc_calibrate();

    /* Enable ADC */
    LL_ADC_Enable(ADC1);

    /* The delay between ADC enablement and ADC stabilization is at least 8 ADC clocks */
    WaitMs(1);

    // Start ADC regular conversion and wait for next external trigger
    LL_ADC_REG_StartConversion(ADC1);

#if (NTC_INTERRUPT_SMAP_ENABLE)
    ntc_time_init();
#endif
}

/**
 * @brief   Initializes and performs NTC thermistor scanning to calculate temperature.
 * @details Reads ADC values, filters out min and max values,
 *          calculates the average of the remaining values, and then calculates the temperature.
 * @param   temp Pointer to store the calculated temperature.
 * @retval  None.
 */
void ntc_smapling(uint8_t *temp)
{
    volatile uint32_t adc_read = 0;
    volatile uint32_t sum = 0;
    volatile uint32_t adc_store[5] = {0};
    volatile uint16_t min_val = 0;
    volatile uint16_t max_val = 0;

    for (int i = 0; i < 5; i++)
    {
        if (LL_ADC_IsActiveFlag_EOS(ADC1))
        {
            LL_ADC_ClearFlag_EOS(ADC1);
            adc_read = LL_ADC_REG_ReadConversionData12(ADC1);
            adc_store[i] = adc_read;
            sum += adc_read;
            if (i == 0)
            {
                min_val = adc_store[i];
                max_val = adc_store[i];
            }
            else
            {
                if (adc_store[i] < min_val)
                    min_val = adc_store[i];
                else if (adc_store[i] > min_val)
                    max_val = adc_store[i];
            }
            // Greater than ADC single scan period
            WaitUs(400);
        }
    }

    sum -= min_val + max_val;

    uint32_t average = sum / 3;

#if (NTC_HANDLE_ENABLE)
    if (temperature_calc(average, temp))
    {
        // printf("--->>> Average adc_read : %ld, temp: %d.%d\r\n", average, temp[0], temp[1]);
    }
#endif
}

#if (NTC_INTERRUPT_SMAP_ENABLE)
/**
 * @brief   ADC and Comparator interrupt handler function.
 * @details This function handles the ADC conversion complete interrupt and
 *          calculates the temperature using the ADC value.
 * @param   None
 * @retval  None
 */
void ADC_COMP_IRQHandler(void)
{
    uint16_t mVoltage;
    uint32_t adcRead;

    if (LL_ADC_IsActiveFlag_EOC(ADC1) != 0)
    {
        LL_ADC_ClearFlag_EOC(ADC1);

        /* Read ADC conversion result */
        adcRead = LL_ADC_REG_ReadConversionData12(ADC1);
        /* Convert the adc value to voltage value */
        mVoltage = __LL_ADC_CALC_DATA_TO_VOLTAGE(VDDA_APPLI, adcRead, LL_ADC_RESOLUTION_12B);
        printf("Read %ld, voltage %d mV\r\n", adcRead, mVoltage);
    }
}
#endif

#endif