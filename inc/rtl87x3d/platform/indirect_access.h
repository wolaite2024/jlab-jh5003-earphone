/***************************************************************************
 Copyright (C) Realtek
 ***************************************************************************/
#ifndef _INDIRECT_ACCESS_H_
#define _INDIRECT_ACCESS_H_

/**
    * @brief    Read data from aon register
    * @param    offset: register address
    * @return   data read from register
    */
uint16_t btaon_fast_read(uint16_t offset);

/**
  * @brief  Read 8bits data from aon register.
  * @param  offset: register address.
  * @return   data read from register.
  */
uint8_t btaon_fast_read_8b(uint16_t offset);

/**
    * @brief    Read data from aon register safely
    * @param    offset: register address
    * @return   data read from register
    */
uint16_t btaon_fast_read_safe(uint16_t offset);

/**
  * @brief    Read 8 bits data from aon register safely
  * @param    offset: register address
  * @return   data read from register
  */
uint8_t btaon_fast_read_safe_8b(uint16_t offset);

/**
    * @brief    Write data to aon register
    * @param    offset:  register address
    * @param    data:  data to be writen to register
    * @return
    */
void btaon_fast_write(uint16_t offset, uint16_t data);

/**
  * @brief    Write 8bits data to aon register
  * @param    offset:  register address
  * @param    data:  data to be writen to register
  * @return
  */
void btaon_fast_write_8b(uint16_t offset, uint8_t data);

/**
    * @brief    Write data to aon egister safely
    * @param    offset:  register address
    * @param    data:  data to be writen to register
    * @return
    */
void btaon_fast_write_safe(uint16_t offset, uint16_t data);

/**
  * @brief    Write 8bits data to aon egister safely
  * @param    offset:  register address
  * @param    data:  data to be writen to register
  * @return
  */
void btaon_fast_write_safe_8b(uint16_t offset, uint8_t data);

/**
    * @brief    update data to aon egister safely
    * @param    offset:  register address
    * @param    mask:  the bits mask to be writen to register
    * @param    data: the data of the mask to be written
    * @return
    */
void btaon_fast_update_safe(uint16_t offset, uint16_t mask, uint16_t data);

#endif /* _INDIRECT_ACCESS_H_ */

