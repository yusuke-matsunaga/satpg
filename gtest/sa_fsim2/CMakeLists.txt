
# ===================================================================
# インクルードパスの設定
# ===================================================================
include_directories(
  ${PROJECT_SOURCE_DIR}/satpg_common/fsim
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

add_executable ( SaFsim2SimNodeTest
  SimNodeTest.cc
  $<TARGET_OBJECTS:satpg_common_d>
  $<TARGET_OBJECTS:satpg_fsim2_d>
  $<TARGET_OBJECTS:ym_base_d>
  $<TARGET_OBJECTS:ym_logic_d>
  $<TARGET_OBJECTS:ym_cell_d>
  $<TARGET_OBJECTS:ym_bnet_d>
  $<TARGET_OBJECTS:ym_sat_d>
  $<TARGET_OBJECTS:ym_mincov_d>
  $<TARGET_OBJECTS:ym_udgraph_d>
  )

target_compile_options ( SaFsim2SimNodeTest
  PRIVATE "-g"
  )

target_compile_definitions ( SaFsim2SimNodeTest
  PRIVATE "-DFSIM_VAL2"
  )

target_link_libraries ( SaFsim2SimNodeTest
  ${YM_LIB_DEPENDS}
  pthread
  ${GTEST_BOTH_LIBRARIES}
  )

add_test ( SaFsim2Test
  SaFsim2SimNodeTest
  )
