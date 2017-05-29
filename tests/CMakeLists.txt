
# ===================================================================
# インクルードパスの設定
# ===================================================================
include_directories(
  ${PROJECT_SOURCE_DIR}/satpg_common/tpg_network
  )


# ===================================================================
# サブディレクトリの設定
# ===================================================================


# ===================================================================
#  ソースファイルの設定
# ===================================================================


# ===================================================================
#  TpgNetworkTest
# ===================================================================

set ( TpgNetworkTest_SOURCES
  TpgNetworkTest.cc
  )

add_executable ( TpgNetworkTest
  TpgNetworkTest.cc
  $<TARGET_OBJECTS:satpg_common_d>
  $<TARGET_OBJECTS:ym_base_d>
  $<TARGET_OBJECTS:ym_logic_d>
  $<TARGET_OBJECTS:ym_cell_d>
  $<TARGET_OBJECTS:ym_bnet_d>
  $<TARGET_OBJECTS:ym_sat_d>
  $<TARGET_OBJECTS:ym_mincov_d>
  $<TARGET_OBJECTS:ym_udgraph_d>
  )

target_compile_options ( TpgNetworkTest
  PRIVATE "-g"
  )

target_link_libraries ( TpgNetworkTest
  pthread
  ${GTEST_BOTH_LIBRARIES}
  )


# ===================================================================
#  TpgFaultTest
# ===================================================================

set ( TpgFaultTest_SOURCES
  TpgFaultTest.cc
  )

add_executable ( TpgFaultTest
  TpgFaultTest.cc
  $<TARGET_OBJECTS:satpg_common_d>
  $<TARGET_OBJECTS:ym_base_d>
  $<TARGET_OBJECTS:ym_logic_d>
  $<TARGET_OBJECTS:ym_cell_d>
  $<TARGET_OBJECTS:ym_bnet_d>
  $<TARGET_OBJECTS:ym_sat_d>
  $<TARGET_OBJECTS:ym_mincov_d>
  $<TARGET_OBJECTS:ym_udgraph_d>
  )

target_compile_options ( TpgFaultTest
  PRIVATE "-g"
  )

target_link_libraries ( TpgFaultTest
  pthread
  ${GTEST_BOTH_LIBRARIES}
  )
