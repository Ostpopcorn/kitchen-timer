#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "audio_example_file.h"
#include "Sound.h"

#define EXAMPLE_I2S_BUF_DEBUG 1

void i2s_task(void * pv);

Sound::Sound(){
    i2s_driver_install(i2s_num, &i2s_config, 0, NULL);
    i2s_set_pin(i2s_num,NULL); // Sätta den till interna I2S kanalen 
    // Säger att den bara ska använda höger för att den motsvara GPIO25
    i2s_set_dac_mode(I2S_DAC_CHANNEL_RIGHT_EN); 
    xTaskCreate(&i2s_task, "i2s_task", 4096, NULL, 10, NULL);
}
/**
 * @brief Scale data to 16bit/32bit for I2S DMA output.
 *        DAC can only output 8bit data value.
 *        I2S DMA will still send 16 bit or 32bit data, the highest 8bit contains DAC data.
 */
int example_i2s_dac_data_scale(uint8_t* d_buff, uint8_t* s_buff, uint32_t len)
{
    uint32_t j = 0;
#if (EXAMPLE_I2S_SAMPLE_BITS == 16)
    for (int i = 0; i < len; i++) {
        d_buff[j++] = 0;
        d_buff[j++] = s_buff[i];
    }
    return (len * 2);
#elif (EXAMPLE_I2S_SAMPLE_BITS == 8)
    for (int i = 0; i < len; i++) {
        d_buff[j++] = s_buff[i];
    }
    return (len);
#else 
    for (int i = 0; i < len; i++) {
        d_buff[j++] = 0;
        d_buff[j++] = 0;
        d_buff[j++] = 0;
        d_buff[j++] = s_buff[i];
    }
    return (len * 4);
#endif
}
/**
 * @brief debug buffer data
 */
void example_disp_buf(uint8_t* buf, int length)
{
#if EXAMPLE_I2S_BUF_DEBUG
    printf("======\n");
    for (int i = 0; i < length; i++) {
        printf("%02x ", buf[i]);
        if ((i + 1) % 8 == 0) {
            printf("\n");
        }
    }
    printf("======\n");
#endif
}

/**
 * @brief Reset i2s clock and mode
 */
void example_reset_play_mode(void)
{
    i2s_set_clk(I2S_NUM_0, 44100, (i2s_bits_per_sample_t) 16, I2S_CHANNEL_MONO);
}
/**
 * @brief Set i2s clock for example audio file
 */
void example_set_file_play_mode(void)
{
    i2s_set_clk(I2S_NUM_0, 16000, (i2s_bits_per_sample_t) EXAMPLE_I2S_SAMPLE_BITS, I2S_CHANNEL_MONO);
}
// i2s_channel_t

#define EXAMPLE_I2S_READ_LEN      (16 * 1024)
void i2s_task(void * pv){
    int i2s_read_len = EXAMPLE_I2S_READ_LEN;
    uint8_t* i2s_write_buff = (uint8_t*) calloc(i2s_read_len, sizeof(char));
    while (1) {

        size_t bytes_read, bytes_written;

        //4. Play an example audio file(file format: 8bit/16khz/single channel)
        printf("Playing file example: \n");
        int offset = 0;
        int tot_size = sizeof(audio_table);
        example_set_file_play_mode();
        
        while (offset < tot_size) {
            int play_len = ((tot_size - offset) > (4 * 1024)) ? (4 * 1024) : (tot_size - offset);
            int i2s_wr_len = example_i2s_dac_data_scale(i2s_write_buff, (uint8_t*)(audio_table + offset), play_len);
            i2s_write(I2S_NUM_0, i2s_write_buff, i2s_wr_len, &bytes_written, portMAX_DELAY);
            offset += play_len;
            example_disp_buf((uint8_t*)(audio_table + offset), 32);
        }
        // int i2s_wr_len = example_i2s_dac_data_scale(i2s_write_buff, (uint8_t*)(audio_table + offset), play_len);
        // i2s_write(I2S_NUM_0, audio_table, tot_size, &bytes_written, portMAX_DELAY);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        example_reset_play_mode();
    }
}