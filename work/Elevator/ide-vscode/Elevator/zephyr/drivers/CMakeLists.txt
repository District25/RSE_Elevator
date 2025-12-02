# SPDX-License-Identifier: Apache-2.0

#add_subdirectory(sensor)

# Out-of-tree drivers for existing driver classes
add_subdirectory_ifdef(CONFIG_SENSOR sensor)

#zephyr_library_sources(sensor/qdec_rse/qdec_rse.c)
