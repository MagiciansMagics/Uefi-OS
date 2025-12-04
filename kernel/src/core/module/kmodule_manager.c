#include <module/kmodule_manager.h>
#include <string.h>

local kmodule_manager_data_t kmodule_manager = {0};

void
install_global_variable(char* name, void* data, uint64_t data_size)
{
    if (kmodule_manager.variable_index >= MAX_KGLOBAL_VARIABLES)
        return;

    if (data == NULL || data_size == 0)
        return;

    size_t name_len = strlen(name);

    if (name_len >= MAX_KMODULE_NAME)
        return;

    memcpy(kmodule_manager.variables[kmodule_manager.variable_index].name, name, name_len);
    memcpy(kmodule_manager.variables[kmodule_manager.variable_index].data, data, data_size);

    kmodule_manager.variables[kmodule_manager.variable_index].size = data_size;

    kmodule_manager.variable_index++;
}

int
require_global_variable(char* name, void* result)
{
    for (uint8_t i = 0; i < kmodule_manager.variable_index; i++)
    {
        if (strcmp(kmodule_manager.variables[i].name, name) == 0)
        {
            memcpy(result, kmodule_manager.variables[i].data, kmodule_manager.variables[i].size);
            return 1;
        }
    }
    return 0;
}

void
install_module(const char* name)
{
    if (kmodule_manager.module_index >= MAX_KMODULES)
        return;

    size_t len = strlen(name);

    if (len == 0)
        return;

    if (len >= MAX_KMODULE_NAME - 1) // Leave space for the null terminator
        return;

    memcpy(kmodule_manager.modules[kmodule_manager.module_index].name, name, len);

    kmodule_manager.module_index++;
}

int
require_module(const char* name)
{
    if (strlen(name) >= MAX_KMODULE_NAME)
        return 0;

    for (uint8_t i = 0; i < kmodule_manager.module_index; i++)
    {
        if (strcmp(name, kmodule_manager.modules[i].name) == 0)
            return 1;
    }

    return 0;
}

void
init_kmodule_manager(void)
{
    for (uint8_t i = 0; i < MAX_KMODULES; i++)
    {
        memset(&kmodule_manager.modules[i], 0, sizeof(kmodule_info_data_t));
    }

    for (uint8_t i = 0; i < MAX_KMODULES; i++)
    {
        memset(&kmodule_manager.variables[i], 0, sizeof(kvariable_info_data_t));
    }

    kmodule_manager.module_index = 0;
    kmodule_manager.variable_index = 0;
}