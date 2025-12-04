#ifndef _KMODULE_MANAGER_H_
#define _KMODULE_MANAGER_H_

#pragma once

#include <types.h>

#define MAX_KMODULES 32
#define MAX_KMODULE_NAME 32
#define MAX_KGLOBAL_VARIABLES 16

typedef struct
{
    char name[MAX_KMODULE_NAME];
    uint64_t size;
    void* data;
} kvariable_info_data_t;

typedef struct
{
    char name[MAX_KMODULE_NAME];
} kmodule_info_data_t;

typedef struct
{
    kmodule_info_data_t modules[MAX_KMODULES];
    kvariable_info_data_t variables[MAX_KGLOBAL_VARIABLES];
    uint8_t module_index;
    uint8_t variable_index;
} kmodule_manager_data_t;

void
install_global_variable(char* name, void* data, size_t data_size);

int
require_global_variable(char* name, void* result);

void
install_module(const char* name);

int
require_module(const char* name);

void
init_kmodule_manager(void);

#endif