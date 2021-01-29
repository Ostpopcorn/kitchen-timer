#include "battery_monitor.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "string.h"

static void print_char_val_type(esp_adc_cal_value_t val_type)
{
    if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
        printf("Characterized using Two Point Value\n");
    } else if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
        printf("Characterized using eFuse Vref\n");
    } else {
        printf("Characterized using Default Vref\n");
    }
}

static void check_efuse(void)
{
#if CONFIG_IDF_TARGET_ESP32
    //Check if TP is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) == ESP_OK) {
        printf("eFuse Two Point: Supported\n");
    } else {
        printf("eFuse Two Point: NOT supported\n");
    }
    //Check Vref is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_VREF) == ESP_OK) {
        printf("eFuse Vref: Supported\n");
    } else {
        printf("eFuse Vref: NOT supported\n");
    }
#elif CONFIG_IDF_TARGET_ESP32S2
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) == ESP_OK) {
        printf("eFuse Two Point: Supported\n");
    } else {
        printf("Cannot retrieve eFuse Two Point calibration values. Default calibration values will be used.\n");
    }
#else
#error "This example is configured for ESP32/ESP32S2."
#endif
}


Battery::Battery()
{

}

Battery::Battery(adc1_channel_t battery_analog_1_channel, gpio_num_t battery_monitor_enable)
{
    set_pins(battery_analog_1_channel,battery_monitor_enable);
    
}

Battery::~Battery()
{

}


void Battery::enable_monitor()
{
    if(monitor_enable_pin == GPIO_NUM_NC)
    {
        return;
    }
    gpio_set_level(monitor_enable_pin,1);
}

void Battery::disable_monitor()
{
    if(monitor_enable_pin == GPIO_NUM_NC)
    {
        return;
    }
    gpio_set_level(monitor_enable_pin,0);
}

void Battery::set_pins(adc1_channel_t battery_analog_1_channel, gpio_num_t battery_monitor_enable)
{
    check_efuse();
    adc_chars = (esp_adc_cal_characteristics_t *) calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(unit, atten, width, default_vref, adc_chars);
    print_char_val_type(val_type);

    this->monitor_enable_pin = battery_monitor_enable;
    this->analog_1_channel = battery_analog_1_channel;
    // Set GPIO modes
    if(monitor_enable_pin != GPIO_NUM_NC){

        gpio_config_t io_conf = {
            .pin_bit_mask = (1ULL<<battery_monitor_enable),
            .mode = GPIO_MODE_OUTPUT,
            .pull_up_en = GPIO_PULLUP_DISABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_DISABLE,
        };
        gpio_config(&io_conf);
    }
    //adc1_pad_get_io_num();
    if (analog_1_channel != ADC1_CHANNEL_MAX){

        adc1_config_width(width);
        adc1_config_channel_atten(analog_1_channel,atten);
    }else{
        //ERROR
    }

}
void Battery::register_callback(battery_callback_t new_callback){
    if (new_callback != NULL)
    {
        callback = new_callback;
    }
}

void Battery::measure()
{
    int adc_reading{};

    enable_monitor();
    for (size_t i = 0; i < number_of_sample; i++)
    {
        adc_reading += adc1_get_raw(analog_1_channel);
    }
    disable_monitor();

    printf("Read: %i\n", adc_reading);

    if(adc_reading == 0){
        return;
    }
    adc_reading /= number_of_sample;


    int voltage = static_cast<int>(esp_adc_cal_raw_to_voltage(adc_reading, adc_chars));

    last_measure = voltage;
    if(callback != NULL){
        callback(this);
    }
    
    return;
}

int Battery::get_last_mesurement()
{
    return last_measure;
}

/* 
std::string Battery::get_last_mesurement_as_string() 
{
    char text[5];
    text[0] = (last_measure/1000)%10;
    text[1] = ':';
    text[2] = (last_measure/100)%10;
    text[3] = (last_measure/10)%10;
    text[4] = '\n';
    return std::string(text); 
    //model->put_new_entry(ScreenModelEntry::ENTRY_BATTERY_VOLTAGE,text);

}
*/