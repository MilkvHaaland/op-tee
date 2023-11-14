/*
 * Copyright (c) 2023, Alibaba Group Holding Limited
 */

#ifndef _SPI_CFG_H_
#define _SPI_CFG_H_

#ifdef __cplusplus
extern "C"{
#endif

#define CSI_DEFAULT_SPI_POLL_TIMEOUT_MS 1000

enum csi_spi_dev_mode {
    CSI_SPI_MODE_MASTER = 0,
    CSI_SPI_MODE_SLAVE,
};

enum csi_spi_format {
    CSI_SPI_FORMAT_CPOL0_CPHA0 = 0,       /* CPOL = 0; CPHA = 0 */
    CSI_SPI_FORMAT_CPOL0_CPHA1,           /* CPOL = 1; CPHA = 0 */
    CSI_SPI_FORMAT_CPOL1_CPHA0,           /* CPOL = 0; CPHA = 1 */
    CSI_SPI_FORMAT_CPOL1_CPHA1,           /* CPOL = 1; CPHA = 1 */
};

struct csi_spi_config {
    enum csi_spi_dev_mode     mode;       /* master or slave, only support master now */
    enum csi_spi_format       format;     /* set CPOL and CPHA */
    unsigned int              speed;      /* set speed */
    unsigned int              wordlen;    /* only support 8bit now*/
    unsigned int              timeout;    /* poll transfer timeout*/
};

#ifdef __cplusplus
}
#endif

#endif /* _SPI_CFG_H_ */
