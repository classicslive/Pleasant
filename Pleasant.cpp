extern "C"
{
  #include <cl_memory.h>
};

#include <Pleasant.h>

bool Pleasant::installMembanks(void)
{
  if (!memoryMaps()->descriptors || !memoryMaps()->num_descriptors)
  {
    memory.regions = static_cast<cl_memory_region_t*>(calloc(1, sizeof(cl_memory_region_t)));
    memory.regions[0].base_guest = 0;
    memory.regions[0].base_host = reinterpret_cast<uint8_t*>(core()->retro_get_memory_data(RETRO_MEMORY_SYSTEM_RAM));
    memory.regions[0].size = core()->retro_get_memory_size(RETRO_MEMORY_SYSTEM_RAM);
    snprintf(memory.regions[0].title, sizeof(memory.regions[0].title), "%s", "RETRO_MEMORY_SYSTEM_RAM");
    memory.region_count = 1;

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
