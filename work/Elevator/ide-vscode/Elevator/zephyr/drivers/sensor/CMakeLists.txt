# SPDX-License-Identifier: Apache-2.0

#add_subdirectory(qdec_rse)
add_subdirectory_ifdef(CONFIG_QDEC_RSE qdec_rse)
