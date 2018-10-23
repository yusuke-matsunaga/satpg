
# ===================================================================
# インクルードパスの設定
# ===================================================================
include_directories(
  ../../tests/dtpgtest
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

ym_add_gtest ( DtpgTest
  DtpgTest.cc
  ../../tests/dtpgtest/DtpgTest.cc
  $<TARGET_OBJECTS:satpg_common_ad>
  $<TARGET_OBJECTS:satpg_fsimsa2_ad>
  $<TARGET_OBJECTS:satpg_fsimsa3_ad>
  $<TARGET_OBJECTS:satpg_fsimtd2_ad>
  $<TARGET_OBJECTS:satpg_fsimtd3_ad>
  $<TARGET_OBJECTS:ym_base_ad>
  $<TARGET_OBJECTS:ym_logic_ad>
  $<TARGET_OBJECTS:ym_cell_ad>
  $<TARGET_OBJECTS:ym_bnet_ad>
  $<TARGET_OBJECTS:ym_sat_ad>
  $<TARGET_OBJECTS:ym_combopt_ad>
  DEFINITIONS "-DDATAPATH=\"${CMAKE_CURRENT_SOURCE_DIR}/data/\""
  )
