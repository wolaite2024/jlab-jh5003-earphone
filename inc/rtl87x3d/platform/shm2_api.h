/**
 * @copyright Copyright (C) 2022 Realtek Semiconductor Corporation.
 *
 * @file shm2_api.h
 * @version 1.0
 * @brief Shm2-related definitions and APIs to control dsp2 and communicate with mcu.
 *
 * @note:
 */
#ifndef __SHM2_API__
#define __SHM2_API__
#include <stdint.h>


/**
 * @brief There are 3 queues to send data from mcu to dsp2, the \ref T_H2D_DATA_Q_ID defines index of
 *        these queues.
 * @ingroup IPC
 */
typedef enum
{
    H2D_DATA_Q_ID0 = 0,
    H2D_DATA_Q_ID1,
    H2D_DATA_Q_ID2,
} T_H2D_DATA_Q_ID;

/**
 * @brief There are 3 queues to send data from dsp2 to mcu, the \ref T_D2H_DATA_Q_ID defines index of
 *        these queues.
 * @ingroup IPC
 */
typedef enum
{
    D2H_DATA_Q_ID0 = 0,
    D2H_DATA_Q_ID1,
    D2H_DATA_Q_ID2,
} T_D2H_DATA_Q_ID;

/**
 * @brief Define different communication type between dsp2 and mcu
 *
 * @ingroup IPC
 */
typedef enum
{
    SHM2_IOCTL_VERSION,
    SHM2_IOCTL_CALLBACK_CMD_EVENT,
    SHM2_IOCTL_CALLBACK_RX_REQ,
    SHM2_IOCTL_CALLBACK_TX_ACK,
    SHM2_IOCTL_CALLBACK_MAILBOX,

    SHM2_IOCTL_TOTAL
} T_SHM2_IO_CTL;

typedef enum
{
#if 0 // phase out
    H2D_CMD_DSP_DAC_ADC_DATA_TO_MCU = 0x1f, //Command id: 0x1F
    /*
    Bit[ 7: 0]: DAC DATA TO MCU or Not
            0: DSP Don't Send DAC DATA to MCU
            1: DSP Send DAC DATA to MCU
    Bit[15: 8]: ADC DATA TO MCU or Not
            0: DSP Don't Send ADC DATA to MCU
            1: DSP Send ADC DATA to MCU
    */
#endif

    H2D_CMD_DSP2_LOG_OUTPUT_SEL = 0x1B, //Command id: 0x1B
    /*
    Bit[7:0]: DSP2 Log output selection
            0: DSP2 Log disabled
            1: reserved
            2: DSP2 Log is forward to MCU to output
    */

    H2D_CMD_DSP2_UART_CFG = 0x1C,
    /*Bit[7:0]: DSP2 UART Config
            0: DSP2 UART disabled
            1: DSP2 UART tx pin enable
            2: DSP2 UART rx pin enable
            3: DSP2 UART tx/rx pin enable
    */

    IpcCmdDataReq = 0xf2,

    H2D_SPP_DATA_CAPTURE_V2 = 0x7F,
    /*
     *   new spp data capture format in spp_data_capture.h
     *
     *   SppCaptRecvHdr_T and SppCaptRecvConfig_T;
     *
     */

    H2D_CMD_USER_DEFINED = 0X8F,

} T_SHM2_CMD_H2D;

/**
 * @brief Callback definitions indexed by \ref T_SHM2_IO_CTL
 *
 * @ingroup IPC
 */
typedef void (*SHM2_IRQ_CB)(void);
typedef void (*SHM2_MAILBOX_CB)(uint32_t group, uint32_t value);

/**
 * @brief Event type to get from mailbox callback \ref SHM2_IOCTL_CALLBACK_MAILBOX
 *        MAILBOX_EVT_SHM2_READY: This event indicate that dsp2 is ready to send/receive data by cmd& data queues.
 *
 * @ingroup IPC
 */
typedef enum
{
    MAILBOX_EVT_SHM2_READY,
} T_MAILBOX_EVT;

/**
 * @brief initialize shm2. This API is called in framework.
 *
* @ingroup IPC
 */
void shm2_init(void);

/**
 * @brief enable shm2. This API is called in framework.
 *
* @ingroup IPC
 */
void shm2_enable(void);

/**
 * @brief disable shm2. This API is called in framework.
 *
 * @ingroup IPC
 */
void shm2_disable(void);

/**
 * @brief To get informations about DSP2 or register callbacks to communicate with application.
 *
 * @param ioctl defined in \ref T_SHM2_IO_CTL \n
 *              SHM2_IOCTL_VERSION: Get shm2 fw version \n
 *              SHM2_IOCTL_CALLBACK_CMD_EVENT: Register callback to process command received from dsp2 \n
 *              SHM2_IOCTL_CALLBACK_RX_REQ: Register callback to process data received from dsp2 \n
 *              SHM2_IOCTL_CALLBACK_TX_ACK: Register callback to process ACK from dsp2 \n
 *              SHM2_IOCTL_CALLBACK_MAILBOX: Register callback to process mailbox event from dsp2
 *
 * @param para  for SHM2_IOCTL_VERSION, the actual type of \para is (uint32_t*) to get fw version \n
 *              for SHM2_IOCTL_CALLBACK_CMD_EVENT, the actual type of \para is SHM2_IRQ_CB. The para will be called when
 *              DSP2 has sent a command to mcu by command queue. \n
 *              SHM2_IOCTL_CALLBACK_RX_REQ,  the actual type of \para is SHM2_IRQ_CB. The para will be called when DSP2
 *              has sent data to mcu by data queue indexed by \ref T_D2H_DATA_Q_ID. \n
 *              SHM2_IOCTL_CALLBACK_TX_ACK, the actual type of \para is SHM2_IRQ_CB. The para will be called when DSP2 has
 *              received data from mcu by data queue indexed by \ref T_H2D_DATA_Q_ID. \n
 *              SHM2_IOCTL_CALLBACK_MAILBOX, the actual type of \para is SHM2_MAILBOX_CB. The para will be called when mcu
 *              receive mailbox interrupt from DSP2.  The group parameter in SHM2_MAILBOX_CB is defined in \ref T_MAILBOX_EVT
 *
 * @note: callbacks indexed by SHM2_IOCTL_CALLBACK_CMD_EVENT/SHM2_IOCTL_CALLBACK_RX_REQ/SHM2_IOCTL_CALLBACK_TX_ACK/SHM2_IOCTL_CALLBACK_MAILBOX
 *        are called in ISR.
 * @return uint32_t 0: success other: fail
 *
 * @ingroup IPC
 */
uint32_t shm2_io_ctl(uint32_t ioctl, uint32_t para);

/**
 * @brief To send command to DSP2
 *
 * @param buffer content of command
 * @param length length of command
 * @param flush If command has been sent after the function returns. \n
 *              true: command has been put into cmd queue and inform DSP2 about the cmd. \n
 *              false: command has just put into shm queue
 * @return true command is sent successfully
 * @return false command is sent unsuccessfully
 *
 * @note the API SHOULD be called AFTER MAILBOX_EVT_SHM2_READY in \ref T_MAILBOX_EVT \n
 *       before communication between mcu and dsp2, \ref engine_start SHOULD be called to
 *       prevent dsp from power-down
 *
 * @par Example
 * @code
 *          #include "shm2_api.h"
 *          #include "audio_probe.h"
 *          void mailbox_cb(uint32_t evt, uint32_t param)
            {
                switch (evt)
                {
                case MAILBOX_EVT_SHM2_READY:
                    {
                        send msg IPC_READY to IPC_task
                    }
                    break;

                default:
                    break;
                }
            }



            void IPC_task()
            {
                shm2_io_ctl(SHM2_IOCTL_CALLBACK_MAILBOX, mailbox_cb);
                receive msg
                {
                        if IPC_READY
                        {
                            engine_start();
                            uint8_t cmd[64] = {};
                            shm2_cmd_send(cmd, sizeof(cmd), true);
                        }
                }
            }
 * @endcode
 *
 * @ingroup IPC
 *
 */
bool shm2_cmd_send(uint8_t *buffer, uint16_t length, bool flush);

/**
 * @brief To receive command from DSP2
 *
 * @param buffer content of command
 * @param length expected of command, the \length MUST be no less than the actual command length
 * @return uint32_t actual command length
 *
 * @note  refer to \ref shm2_cmd_send
 *
 * @par Example
 * @code
 *
 *          #include "shm2_api.h"
 *          #include "audio_probe.h"
 *          void d2h_evt_cb(void)
            {

                send msg D2H_CMD_MSG to IPC_task

            }



            void IPC_task()
            {
                shm2_io_ctl(SHM2_IOCTL_CALLBACK_CMD_EVENT, d2h_evt_cb);
                receive msg
                {
                        if D2H_CMD_MSG
                        {
                            engine_start();
                            uint32_t cmd[0x100];
                            uint32_t actual = shm2_cmd_recv(cmd, sizeof(cmd));
                        }
                }
            }
 * @endcode
 *
 * @ingroup IPC
 *
 */
uint32_t shm2_cmd_recv(uint8_t *buffer, uint16_t length);

/**
 * @brief To send data to DSP2
 *
 * @param buffer data to send
 * @param length length 0f data to send
 * @param flush If data has been sent after the function returns. \n
 *              true: command has been put into data queue and inform DSP2 about the data. \n
 *              false: command has just put into data queue
 * @param type user-defined parameter. Typical usage is to indicate data type in \buffer
 * @param seq user-defined parameter. Typical usage is to indicate data sequence.
 * @param queue_id \ref T_D2H_DATA_Q_ID
 * @return true data is sent successfully
 * @return false data is sent unsuccessfully
 *
 * @note refer to \ref shm2_cmd_send
 *
 * @par Example
 * @code
 *          #include "shm2_api.h"
 *          #include "audio_probe.h"
 *          void mailbox_cb(uint32_t evt, uint32_t param)
            {
                switch (evt)
                {
                case MAILBOX_EVT_SHM2_READY:
                    {
                        send msg IPC_READY to IPC_task
                    }
                    break;

                default:
                    break;
                }
            }



            void IPC_task()
            {
                shm2_io_ctl(SHM2_IOCTL_CALLBACK_MAILBOX, mailbox_cb);
                receive msg
                {
                        if IPC_READY
                        {
                            engine_start();
                            uint8_t data[64] = {};
                            shm2_data_send(data, sizeof(data), true, 0, 0, 0);
                        }
                }
            }
 * @endcode
 *
 * @ingroup IPC
 *
 */
bool shm2_data_send(uint8_t *buffer, uint16_t length, bool flush, uint8_t type, uint8_t seq,
                    uint32_t queue_id);

/**
 * @brief To receive data from DSP2
 *
 * @param buffer content of data
 * @param length expected length of data, the \length MUST be no less than the actual data length
 * @param p_type actual data type
 * @param queue_id actual queue id from which data has been received \ref T_D2H_DATA_Q_ID
 * @return uint32_t actual received data length
 *
 * @note  refer to \ref shm2_cmd_send
 *
 * @par Example
 * @code
 *
 *          #include "shm2_api.h"
 *          #include "audio_probe.h"
 *          void data_ind_cb(void)
            {

                send msg D2H_DATA_MSG to IPC_task

            }

            void IPC_task()
            {
                shm2_io_ctl(SHM2_IOCTL_CALLBACK_RX_REQ, data_ind_cb);
                receive msg
                {
                        if D2H_DATA_MSG
                        {
                            uint32_t data[0x100];
                            uint8_t type;
                            uint8_t queue_id;
                            uint32_t actual = shm2_data_recv(data, sizeof(data), &type, &queue_id);
                        }
                }
            }
 *
 * @endcode
 *
 * @ingroup IPC
 *
 */
uint32_t shm2_data_recv(uint8_t *buffer, uint16_t length, uint8_t *p_type, uint32_t *queue_id);

/**
 * @brief To peek data length from DSP2
 *
 * @param queue_id actual queue id from which data has been received \ref T_D2H_DATA_Q_ID
 * @return uint32_t uint32_t actual received data length
 */
uint32_t shm2_d2h_data_length_peek(uint32_t queue_id);

#endif
