/**
 * \copyright Copyright (C) 2023 Realtek Semiconductor Corporation.
 *
 * \file usb_cdc_driver.h
 * \version 1.0
 * \brief Upper application can used the definitions and APIs to implement CDC function instances.
 *        The driver supports virtual port com to communicate with the computer.
 *
 * \note:
 */
#ifndef __USB_CDC_DRIVER_H__
#define __USB_CDC_DRIVER_H__
#include <stdbool.h>
#include <stdint.h>
#include "usb_pipe.h"
/**
 * \addtogroup USB_CDC_Driver
 * \brief The module mainly provide components to implement USB CDC Class.
 * This driver support virtual port com to communicate with computer.
 * @{
  */

/**
 * \defgroup USB_CDC_DRIVER_USAGE How to Implement USB CDC Interface
 * @{
 *
 * \brief This section provides a comprehensive guide on implementing a USB CDC interface,
 *        complete with sample code for your reference.
 *
 * \section USB_CDC_DRIVER_ALLOCATE_INSTANCE Allocate Instance
 * Allocate two function instances by \ref usb_cdc_driver_inst_alloc.
 *
 * \par Example
 * \code
 *      void *demo_instance0 = usb_cdc_driver_inst_alloc();
 *      void *demo_instance1 = usb_cdc_driver_inst_alloc();
 * \endcode
 *
 * \section USB_CDC_DRIVER_CDC_INTERFACES CDC Interfaces
 * Implement CDC interfaces as follows:
 *    - Implement descriptor arrays, and register arrays to the CDC driver.
 *    - Implement pipe operations.
 *
 * \par Example  -- USB CDC
 * \code
 *      T_USB_INTERFACE_DESC demo_std_if_desc =
 *      {
 *          ...init...
 *      };
 *      T_CDC_HEADER_FUNC_DESC demo_header_desc =
 *      {
 *          ...init...
 *      };
 *      T_CM_FUNC_DESC demo_cm_func_desc =
 *      {
 *          ...init...
 *      };
 *      T_CDC_ACM_FUNC_DESC demo_acm_func_desc =
 *      {
 *          ...init...
 *      };
 *      T_CDC_UNION_FUNC_DESC demo_union_desc =
 *      {
 *          ...init...
 *      };
 *      T_USB_ENDPOINT_DESC demo_int_in_ep_desc_fs =
 *      {
 *          ...init...
 *      };
 *      T_USB_ENDPOINT_DESC demo_int_in_ep_desc_hs =
 *      {
 *          ...init...
 *      };
 *      T_USB_INTERFACE_DESC demo_std_data_if_desc =
 *      {
 *          ...init...
 *      };
 *      T_USB_ENDPOINT_DESC demo_bulk_in_ep_desc_fs =
 *      {
 *          ...init...
 *      };
 *      T_USB_ENDPOINT_DESC demo_bulk_in_ep_desc_hs =
 *      {
 *          ...init...
 *      };
 *      T_USB_ENDPOINT_DESC demo_bulk_out_ep_desc_fs =
 *      {
 *          ...init...
 *      };
 *      T_USB_ENDPOINT_DESC demo_bulk_out_ep_desc_hs =
 *      {
 *          ...init...
 *      };
 *
 *      void *demo_if0_descs_fs[] =
 *      {
 *          (void *) &demo_std_if_desc,
 *          (void *) &demo_header_desc,
 *          (void *) &demo_cm_func_desc,
 *          (void *) &demo_acm_func_desc,
 *          (void *) &demo_union_desc,
 *          (void *) &demo_int_in_ep_desc_fs,
 *          NULL,
 *      };
 *      void *demo_if0_descs_hs[] =
 *      {
 *          ...
 *          NULL,
 *      };
 *
 *      void *demo_if1_descs_fs[] =
 *      {
 *          (void *) &demo_std_data_if_desc,
 *          (void *) &demo_bulk_in_ep_desc_fs,
 *          (void *) &demo_bulk_out_ep_desc_fs,
 *          NULL,
 *      };
 *      void *demo_if1_descs_hs[] =
 *      {
 *          ...
 *          NULL,
 *      };
 *
 *      void *demo_data_pipe_open(uint8_t ep_addr, T_USB_CDC_DRIVER_ATTR attr, uint8_t pending_req_num)
 *      {
 *          ...
 *          return usb_cdc_driver_data_pipe_open(ep_addr, attr, pending_req_num);
 *      }
 *      bool demo_data_pipe_send(void *handle, void *buf, uint32_t len, void(*cb)())
 *      {
 *          ...
 *          usb_cdc_driver_data_pipe_send();
 *          return true;
 *      }
 *      int demo_data_pipe_close(void *handle)
 *      {
 *          return usb_cdc_driver_data_pipe_close(handle);
 *      }
 *
 *      usb_cdc_driver_if_desc_register(demo_instance0, (void *)demo_if0_descs_hs,
 *                                      (void *)demo_if0_descs_fs);
 *      usb_cdc_driver_if_desc_register(demo_instance1, (void *)demo_if1_descs_hs,
 *                                      (void *)demo_if1_descs_fs);
 * \endcode
 *
 * \section USB_CDC_DRIVER_INITIALIZE_CDC_DRIVER Initialize CDC Driver
 * Call \ref usb_cdc_driver_init to initialize USB CDC driver.
 */
/** @}*/

/** @defgroup USB_CDC_Driver_Exported_Constants USB CDC Driver Exported Constants
  * @{
  */

/**
 * usb_cdc_driver.h
 *
 * \brief   Congestion control.
 *
 * \details If CDC_DRIVER_CONGESTION_CTRL_DROP_CUR is set, the current data to
 *          send will be dropped; otherwise, the first data in the queue will be dropped.
 *
 * \note    Only effective for in endpoint.
 *
 */
#define CDC_DRIVER_CONGESTION_CTRL_DROP_CUR     USB_PIPE_CONGESTION_CTRL_DROP_CUR
/**
 * usb_cdc_driver.h
 *
 * \brief   Congestion control.
 *
 * \details If CDC_DRIVER_CONGESTION_CTRL_DROP_FIRST is set, the first data in the queue will be dropped.
 *
 * \note    only effective for in endpoint.
 *
 */
#define CDC_DRIVER_CONGESTION_CTRL_DROP_FIRST   USB_PIPE_CONGESTION_CTRL_DROP_FIRST

/** End of group USB_CDC_Driver_Exported_Constants
  * @}
  */

/** @defgroup USB_CDC_Driver_Exported_Types USB CDC Driver Exported Types
  * @{
  */

/**
 * usb_cdc_driver.h
 *
 * \brief  The attribute of USB CDC pipe.
 *
 */
typedef T_USB_PIPE_ATTR T_USB_CDC_DRIVER_ATTR;

/**
 * usb_cdc_driver.h
 *
 * \brief   The USB CDC callback.
 *
 */
typedef USB_PIPE_CB USB_CDC_DRIVER_CB;

/** End of group USB_CDC_Driver_Exported_Types
  * @}
  */

/** @defgroup USB_CDC_Driver_Exported_Functions USB CDC Driver Exported Functions
  * @{
  */

/**
 * \brief Allocate the CDC function instance, which possesses the independent function.
 * \return CDC function instance.
 *
 * \par Example
 * Please refer to \ref USB_CDC_DRIVER_USAGE.
 */
void *usb_cdc_driver_inst_alloc(void);

/**
 * \brief Free the CDC function instance, alloacted by \ref usb_cdc_driver_inst_alloc.
 * \param inst The instance alloacted by \ref usb_cdc_driver_inst_alloc.
 * \return Refer to `errno.h`.
 *
 * \par Example
 * Please refer to \ref USB_CDC_DRIVER_USAGE.
 */
int usb_cdc_driver_inst_free(void *inst);

/**
 * usb_cdc_driver.h
 *
 * \brief   Register the CDC interface descriptors.
 *
 * \param  inst The CDC instance returned by \ref usb_cdc_driver_inst_alloc.
 * \param  hs_desc The CDC interface descriptor of high speed.
 * \param  fs_desc The CDC interface descriptor of full speed.
 *
 * \return Refer to `errno.h`.
 *
 * \par Example
 * Please refer to \ref USB_CDC_DRIVER_USAGE.
 */
int usb_cdc_driver_if_desc_register(void *inst, void *hs_desc, void *fs_desc);

/**
 * usb_cdc_driver.h
 *
 * \brief  Unregister the CDC interface descriptor.
 *
 * \param inst The CDC instance returned in \ref usb_cdc_driver_inst_alloc.
 *
 * \return Refer to `errno.h`.
 *
 * \par Example
 * Please refer to \ref USB_CDC_DRIVER_USAGE.
 */
int usb_cdc_driver_if_desc_unregister(void *inst);

/**
 * usb_cdc_driver.h
 *
 * \brief   Open CDC data pipe.
 *
 * \param  ep_addr The endpoint address.
 * \param  attr The pipe attribute of \ref T_USB_CDC_DRIVER_ATTR.
 * \param  pending_req_num Supported pending request number.
 * \param  cb The application callback of \ref USB_CDC_DRIVER_CB, which will be called after data transmission is completed.
 *
 * \return CDC handle.
 *
 * \par Example
 * Please refer to \ref USB_CDC_DRIVER_USAGE.
 */
void *usb_cdc_driver_data_pipe_open(uint8_t ep_addr, T_USB_CDC_DRIVER_ATTR attr,
                                    uint8_t pending_req_num, USB_CDC_DRIVER_CB cb);

/**
 * usb_cdc_driver.h
 *
 * \brief   Close CDC data pipe.
 *
 * \param  handle Return value of \ref usb_cdc_driver_data_pipe_open.
 *
 * \return Refer to `errno.h`.
 *
 */
int usb_cdc_driver_data_pipe_close(void *handle);

/**
 * usb_cdc_driver.h
 *
 * \brief   CDC pipe send data.
 *
 * \param  handle The return value of \ref usb_cdc_driver_data_pipe_open.
 * \param  buf The data to be sent.
 * \param  len The length of data.
 *
 * \return Refer to `errno.h`.
 *
 * \par Example
 * Please refer to \ref USB_CDC_DRIVER_USAGE.
 */
int usb_cdc_driver_data_pipe_send(void *handle, void *buf, uint32_t len);

/**
 * usb_cdc_driver.h
 *
 * \brief   Initalize USB CDC interfaces.
 *
 * \return Refer to `errno.h`.
 *
 * \par Example
 * Please refer to \ref USB_CDC_DRIVER_USAGE.
 */
int usb_cdc_driver_init(void);

/**
 * usb_cdc_driver.h
 *
 * \brief   Deinit USB CDC interfaces.
 *
 * \return Refer to `errno.h`.
 */
int usb_cdc_driver_deinit(void);

/** @} */ /* End of group USB_CDC_Driver_Exported_Functions */
/** @}*/
#endif
