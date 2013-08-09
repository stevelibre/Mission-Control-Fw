#ifndef _ADXL345_H
#define _ADXL345_H



#define  uint8  short

#define ADXL354_INT_ENABLE_REG 0x2E
#define ADXL354_INT_MAP_REG    0x2F
#define ADXL354_INT_SOURCE_REG 0x30

#define ADXL345_DATA_FORMAT_REG  0x31
#define ADXL345_X_AXIS_DATA_REG0 0x32
#define ADXL345_X_AXIS_DATA_REG1 0x33
#define ADXL345_Y_AXIS_DATA_REG0 0x34
#define ADXL345_Y_AXIS_DATA_REG1 0x35
#define ADXL345_Z_AXIS_DATA_REG0 0x36
#define ADXL345_Z_AXIS_DATA_REG1 0x37
#define ADXL345_FIFO_CONTROL_REG 0x38

#define ADXL345_FIFO_WATERMARK    0x10 

#define ADXL345_DEVICE_ID_REG 0x00

#define ADXL345_TRESH_FREE_FALL_REG 0x28
#define ADXL345_TIME_FREE_FALL_REG 0x29


#define ADXL345_SLAVE_WRITE_ADDR 0xA6  //0x3A
#define ADXL345_SLAVE_READ_ADDR 0xA7  //0x3B

//#define ADXL345_FIFO_BYPASS_MODE  0x00
//#define ADXL345_FIFO_STREAM_MODE  0x8F // stream mode, INT0 and 16 sample bytes to trigger watermark interrupt
//#define ADXL345_FIFO_STD_MODE     0x5F //fifo mode , INT0, 16 Sample bytes to trigger watermark interrupt
//#define ADXL345_FIFO_TRIGGER_MODE 0xDF // trigger mode  mode , INT0, 16 Sample bytes retained in buffer to trigger interrupt

/* ADXL345_DATA_FORMAT Bits */
#define ADXL345_SELF_TEST       (1 << 7)
#define ADXL345_SPI             (1 << 6)
#define ADXL345_INT_INVERT      (1 << 5)
#define ADXL345_FULL_RES        (1 << 3)
#define ADXL345_JUSTIFY         (1 << 2)
#define ADXL345_RANGE(x)        ((x) & 0x3)
#define ADXL345_RANGE_PM_2G     0
#define ADXL345_RANGE_PM_4G     1
#define ADXL345_RANGE_PM_8G     2
#define ADXL345_RANGE_PM_16G    3

/* ADXL345_FIFO_CTL Bits */
#define ADXL345_FIFO_MODE(x)    (((x) & 0x3) << 6)
#define ADXL345_FIFO_BYPASS     0
#define ADXL345_FIFO_FIFO       1
#define ADXL345_FIFO_STREAM     2
#define ADXL345_FIFO_TRIGGER    3
#define ADXL345_TRIGGER         (1 << 5)
#define ADXL345_SAMPLES(x)      ((x) & 0x1F)

/*Power Control register*/
#define ADXL345_REGISTER_PWRCTL (0x2D)
#define ADXL345_PWRCTL_MEASURE (1 << 3)

/* ADXL345_FIFO_STATUS Bits */
#define ADXL345_FIFO_TRIG       (1 << 7)
#define ADXL345_ENTRIES(x)      ((x) & 0x3F)

/*Data format register */
#define ADXL345_ENABLE_SELFTEST (1 << 7)
#define ADXL345_DISABLE_SELFTEST (0 << 7)

extern xQueueHandle USARTQueueHandle;

void ADXL345_init(I2C_TypeDef* I2Cx); //wake the accelerometer
void ADXL345_start_selftest();
uint8_t ADXL345_read_ack(I2C_TypeDef* I2Cx, uint8_t reg, uint8_t devread);
uint8_t ADXL345_read_nack(I2C_TypeDef* I2Cx, uint8_t reg, uint8_t devread);
void ADXL345_read_burst(I2C_TypeDef * I2Cx, uint8_t devread, uint8_t * data_out, uint8_t nBytes);
void ADXL345_write_register(I2C_TypeDef* I2Cx, uint8_t regaddr, uint8_t devread, uint8_t data_in);
void ADXL345_exti0_init();
void ADXL345_detect_freefall(I2C_TypeDef* I2Cx);
void ADXL345_readoutTask(void *pvParameters);

#endif