extern "C"
{
  #include <cl_memory.h>
};

#include <Pleasant.h>

cl_error Pleasant::installMemoryRegions(cl_memory_region_t **regions,
                                        unsigned *region_count)
{
  if (!memoryMaps()->descriptors || !memoryMaps()->num_descriptors)
  {
    cl_memory_region_t *new_regions = static_cast<cl_memory_region_t*>(calloc(1, sizeof(cl_memory_region_t)));

    if (!new_regions)
      return CL_ERR_PARAMETER_NULL;

    new_regions[0].base_guest = 0;
    new_regions[0].base_host = reinterpret_cast<uint8_t*>(core()->retro_get_memory_data(RETRO_MEMORY_SYSTEM_RAM));
    new_regions[0].size = core()->retro_get_memory_size(RETRO_MEMORY_SYSTEM_RAM);
    new_regions[0].flags.bits.read = 1;
    new_regions[0].flags.bits.write = 1;
    new_regions[0].endianness = CL_ENDIAN_LITTLE;
    new_regions[0].pointer_length = 4;
    snprintf(new_regions[0].title, sizeof(new_regions[0].title), "%s", "RETRO_MEMORY_SYSTEM_RAM");

    *regions = new_regions;
    *region_count = 1;

    return CL_OK;
  }
  else
  {
    const struct retro_memory_descriptor **descs =
      static_cast<const struct retro_memory_descriptor**>(
        calloc(memoryMaps()->num_descriptors, sizeof(*descs)));
    cl_error error;
    unsigned i;

    CL_UNUSED(regions);
    CL_UNUSED(region_count);

    for (i = 0; i < memoryMaps()->num_descriptors; i++)
      descs[i] = &memoryMaps()->descriptors[i];
    error = cl_init_membanks_libretro(descs, memoryMaps()->num_descriptors);
    free(descs);

    return error;
  }
}

cl_error Pleasant::libraryName(const char **name)
{
  *name = core()->system_info.library_name;

  return CL_OK;
}
