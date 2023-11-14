/*
 * Copyright (c) 2023, Alibaba Group Holding Limited
 */

#ifndef _I2C_CFG_H_
#define _I2C_CFG_H_

#ifdef __cplusplus
extern "C"{
#endif

/**
  \enum        i2c_mode
  \brief       iic work in master/slave mode
 */
enum csi_i2c_mode {
    CSI_I2C_MODE_MASTER = 0,
    CSI_I2C_MODE_SLAVE
};

/**
  \enum        i2c_speed
  \brief       iic speed mode
 */
enum csi_i2c_speed_mode {
	CSI_IC_SPEED_MODE_STANDARD	= 0,
	CSI_IC_SPEED_MODE_FAST,
	CSI_IC_SPEED_MODE_FASTPLUS,
	CSI_IC_SPEED_MODE_MAX,
};

/**
  \enum        i2c_addr_mode
  \brief       iic address mode
 */
enum csi_i2c_addr_mode {
    CSI_I2C_ADDRESS_7BIT = 0,
    CSI_I2C_ADDRESS_10BIT
};

/**
  \enum        i2c_mem_addr_size
  \brief       iic memory address size
 */
enum csi_i2c_mem_addr_size {
    CSI_I2C_MEM_ADDR_SIZE_8BIT = 1,    /* IIC e2prom  8bit address mode */
    CSI_I2C_MEM_ADDR_SIZE_16BIT        /* IIC e2prom  16bit address mode */
};

struct csi_i2c_config {
	enum csi_i2c_mode 			  mode;
	enum csi_i2c_speed_mode 	  speed;
	enum csi_i2c_addr_mode 		  addr_mode;
};

struct csi_i2c_msg {
	unsigned int 	            addr;
	unsigned int 	            len;
	unsigned int 	            offset;
  enum csi_i2c_mem_addr_size    mem_addr_size;
};


#ifdef __cplusplus
}
#endif

#endif
