extern "C"
{
  #include <cl_memory.h>
};

#include <Pleasant.h>

bool Pleasant::installMembanks(void)
{
  if (!memoryMaps()->descriptors || !memoryMaps()->num_descriptors)
  {
    memory.banks = static_cast<cl_membank_t*>(calloc(1, sizeof(cl_membank_t)));
    memory.banks[0].data = reinterpret_cast<uint8_t*>(core()->retro_get_memory_data(RETRO_MEMORY_SYSTEM_RAM));
    memory.banks[0].size = core()->retro_get_memory_size(RETRO_MEMORY_SYSTEM_RAM);
    memory.banks[0].start = 0;
    snprintf(memory.banks[0].title, sizeof(memory.banks[0].title), "%s", "RETRO_MEMORY_SYSTEM_RAM");
    memory.bank_count = 1;

    return true;
  }
  else
  {
    const struct retro_memory_descriptor **descs =
      static_cast<const struct retro_memory_descriptor**>(
        calloc(memoryMaps()->num_descriptors, sizeof(*descs)));
    bool success;
    unsigned i;

    for (i = 0; i < memoryMaps()->num_descriptors; i++)
       descs[i] = &memoryMaps()->descriptors[i];
    success = cl_init_membanks_libretro(descs, memoryMaps()->num_descriptors);
    free(descs);

    return success;
  }
}

const char* Pleasant::libraryName(void)
{
  return core()->system_info.library_name;
}
