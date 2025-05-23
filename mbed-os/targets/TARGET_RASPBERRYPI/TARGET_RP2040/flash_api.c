/* mbed Microcontroller Library
 * Copyright (c) 2024, Arm Limited and affiliates.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/******************************************************************************
 * INCLUDE
 ******************************************************************************/
#if DEVICE_FLASH

#include "mbed_assert.h"
#include "mbed_critical.h"
#include "hal/flash_api.h"
#include "hardware/flash.h"

/******************************************************************************
 * CONSTANT
 ******************************************************************************/



/******************************************************************************
 * FUNCTION DEFINITION
 ******************************************************************************/

int32_t flash_init(flash_t *obj)
{
    (void)(obj);

    return 0;
}

int32_t flash_free(flash_t *obj)
{
    (void)(obj);

    return 0;
}

int32_t flash_erase_sector(flash_t *obj, uint32_t address)
{
    (void)(obj);

    address = address - XIP_BASE;

    if ((address % FLASH_SECTOR_SIZE) != 0) {
        return -1;
    }

    core_util_critical_section_enter();
    flash_range_erase(address, FLASH_SECTOR_SIZE);
    core_util_critical_section_exit();

    return 0;
}

int32_t flash_read(flash_t *obj, uint32_t address, uint8_t *data, uint32_t size)
{
    (void)(obj);

    const uint8_t *flash_target_contents = (const uint8_t *) (address);

    for (uint32_t i = 0; i < size; i++) {
        data[i] = flash_target_contents[i];
    }
    return 0;
}

int32_t flash_program_page(flash_t *obj, uint32_t address, const uint8_t *data, uint32_t size)
{
    (void)(obj);

    address = address - XIP_BASE;

    if ((address % FLASH_PAGE_SIZE) != 0) {
        return -1;
    }

    size_t pages = size/FLASH_PAGE_SIZE;
    if (size%FLASH_PAGE_SIZE != 0) {
        pages += 1;
    }

    core_util_critical_section_enter();
    flash_range_program(address, data, FLASH_PAGE_SIZE * pages);
    core_util_critical_section_exit();

    return 0;

}

uint32_t flash_get_size(const flash_t *obj)
{
    (void)(obj);

    return MBED_ROM_BANK_QSPI_FLASH_SIZE;
}

uint32_t flash_get_sector_size(const flash_t *obj, uint32_t address)
{
    if (address < flash_get_start_address(obj) || address >= flash_get_start_address(obj) + flash_get_size(obj)) {
        return MBED_FLASH_INVALID_SIZE;
    }

    return FLASH_SECTOR_SIZE;
}

uint32_t flash_get_page_size(const flash_t *obj)
{
    (void)(obj);

    return FLASH_PAGE_SIZE;
}

uint32_t flash_get_start_address(const flash_t *obj)
{
    (void)obj;

    return XIP_BASE;
}

uint8_t flash_get_erase_value(const flash_t *obj)
{
    (void)obj;

    return 0xFF;

}

#endif