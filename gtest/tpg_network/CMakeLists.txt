
# ===================================================================
# インクルードパスの設定
# ===================================================================
include_directories(
  )


# ===================================================================
# サブディレクトリの設定
# ===================================================================


# ===================================================================
#  ソースファイルの設定
# ===================================================================


# ===================================================================
#  テスト用のターゲットの設定
# ===================================================================

add_executable ( MakeCnfTest
  MakeCnfTest.cc
  $<TARGET_OBJECTS:satpg_common_d>
  $<TARGET_OBJECTS:satpg_fsim2_d>
  $<TARGET_OBJECTS:satpg_fsim3_d>
  $<TARGET_OBJECTS:ym_base_d>
  $<TARGET_OBJECTS:ym_logic_d>
  $<TARGET_OBJECTS:ym_cell_d>
  $<TARGET_OBJECTS:ym_bnet_d>
  $<TARGET_OBJECTS:ym_sat_d>
  $<TARGET_OBJECTS:ym_mincov_d>
  $<TARGET_OBJECTS:ym_udgraph_d>
  )

target_compile_options ( MakeCnfTest
  PRIVATE "-g"
  )

target_link_libraries ( MakeCnfTest
  ${YM_LIB_DEPENDS}
  pthread
  ${GTEST_BOTH_LIBRARIES}
  )

add_executable ( MakeFaultyCnfTest
  MakeFaultyCnfTest.cc
  $<TARGET_OBJECTS:satpg_common_d>
  $<TARGET_OBJECTS:satpg_fsim2_d>
  $<TARGET_OBJECTS:satpg_fsim3_d>
  $<TARGET_OBJECTS:ym_base_d>
  $<TARGET_OBJECTS:ym_logic_d>
  $<TARGET_OBJECTS:ym_cell_d>
  $<TARGET_OBJECTS:ym_bnet_d>
  $<TARGET_OBJECTS:ym_sat_d>
  $<TARGET_OBJECTS:ym_mincov_d>
  $<TARGET_OBJECTS:ym_udgraph_d>
  )

target_compile_options ( MakeFaultyCnfTest
  PRIVATE "-g"
  )

target_link_libraries ( MakeFaultyCnfTest
  ${YM_LIB_DEPENDS}
  pthread
  ${GTEST_BOTH_LIBRARIES}
  )

add_test ( TpgNetworkTest
  MakeCnfTest
  MakeFaultyCnfTest
  )
