
/// @file MatrixGen.cc
/// @brief MatrixGen の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.


#include "MatrixGen.h"
#include "TpgNetwork.h"
#include "TpgFault.h"
#include "TestVector.h"
#include "Fsim.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_YM_SATPG

// @brief コンストラクタ
// @param[in] fault_list 故障のリスト
// @param[in] tv_list テストパタンのリスト
// @param[in] network ネットワーク
// @param[in] fault_type 故障の種類
MatrixGen::MatrixGen(const vector<const TpgFault*>& fault_list,
		     const vector<TestVector>& tv_list,
		     const TpgNetwork& network,
		     FaultType fault_type) :
  mFaultList(fault_list),
  mTvList(tv_list),
  mRowIdMap(network.max_fault_id(), -1)
{
  for ( auto row_id: Range(mFaultList.size()) ) {
    auto fault = mFaultList[row_id];
    mRowIdMap[fault->id()] = row_id;
  }

  mFsim.init_fsim3(network, fault_type);
}

// @brief デストラクタ
MatrixGen::~MatrixGen()
{
}

// @brief 被覆行列を作る．
std::unique_ptr<McMatrix>
MatrixGen::generate()
{
  mMatrix = static_cast<std::unique_ptr<McMatrix>>(new McMatrix(mFaultList.size(), mTvList.size()));

  int wpos = 0;
  mFsim.clear_patterns();
  int tv_base = 0;
  for ( auto tv: mTvList ) {
    mFsim.set_pattern(wpos, tv);
    ++ wpos;
    if ( wpos == kPvBitLen ) {
      do_fsim(tv_base, wpos);
      mFsim.clear_patterns();
      wpos = 0;
      tv_base += kPvBitLen;
    }
  }
  if ( wpos > 0 ) {
    do_fsim(tv_base, wpos);
  }

  return std::move(mMatrix);
}

// @brief 故障シミュレーションを行う．
void
MatrixGen::do_fsim(int tv_base,
		   int num)
{
  int ndet = mFsim.ppsfp();
  vector<int> det_list[kPvBitLen];
  for ( auto i: Range(ndet) ) {
    const TpgFault* fault = mFsim.det_fault(i);
    PackedVal dbits = mFsim.det_fault_pat(i);
    int row_id = mRowIdMap[fault->id()];
    for ( auto bit: Range(num) ) {
      if ( dbits & (1UL << bit) ) {
	int tvid = tv_base + bit;
	mMatrix->insert_elem(row_id, tv_base + bit);
      }
    }
  }
}

END_NAMESPACE_YM_SATPG
