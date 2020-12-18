#include "driver/i2s.h"

class Sound {
    /*
      Men om vi säger att den ska skicka 
    */
    uint8_t i2s_num = I2S_NUM_0;
    i2s_config_t i2s_config = {
        .mode =(i2s_mode_t) (I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN),
        .sample_rate = 44100,
        // Tror att inbyggda DAC endast har 8 bitar men skickar 16 för att den är kul eller nått...
        .bits_per_sample = (i2s_bits_per_sample_t) 16, 
        // Tänker mig att channel format är hur mycket den ska skicka. Så skillnaden mellan ALL_ och ONLY_
        // är att all säger att allt man ger blir _ och only bara den delen av bitarna.
        // Jag har bara en kanal så jag kör all_right
        .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT,//I2S_CHANNEL_FMT_ALL_RIGHT,
        .intr_alloc_flags = 0, // default interrupt priority
        .dma_buf_count = 8,
        .dma_buf_len = 64,
        .use_apll = false
    };
public:
    Sound();
};