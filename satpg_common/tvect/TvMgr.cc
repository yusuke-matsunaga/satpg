
/// @file TvMgr.cc
/// @brief TvMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "TvMgr.h"
#include "TpgNetwork.h"
#include "TestVector.h"


BEGIN_NAMESPACE_YM_SATPG

BEGIN_NONAMESPACE

inline
ymuint
calc_vectlen(ymuint input_num,
	     ymuint dff_num,
	     FaultType fault_type)
{
  ymuint ans = 0;
  if ( fault_type == kFtStuckAt ) {
    ans = input_num + dff_num;
  }
  else if ( fault_type == kFtTransitionDelay ) {
    ans = input_num * 2 + dff_num;
  }
  else {
    ASSERT_NOT_REACHED;
  }
  return ans;
}

END_NONAMESPACE

// @brief コンストラクタ
// @param[in] network 対象のネットワーク
// @param[in] fault_type 故障の種類
TvMgr::TvMgr(const TpgNetwork& network,
	     FaultType fault_type) :
  mFaultType(fault_type),
  mInputNum(network.input_num()),
  mDffNum(network.dff_num()),
  mVectLen(calc_vectlen(mInputNum, mDffNum, fault_type)),
  mTvSize(calc_size(mVectLen)),
  mAlloc(mTvSize, 1024)
{
}

// @brief デストラクタ
TvMgr::~TvMgr()
{
  clear();
}

/// @brief 内容を空にする．
void
TvMgr::clear()
{
  mAlloc.destroy();
}

// @brief 新しいパタンを生成する．
// @return 生成されたパタンを返す．
//
// パタンは0で初期化される．
TestVector*
TvMgr::new_vector()
{
  void* p = mAlloc.get_memory(mTvSize);
  TestVector* tv = new (p) TestVector(mInputNum, mDffNum, mFaultType);

  return tv;
}

// @brief パタンを削除する．
void
TvMgr::delete_vector(TestVector* tv)
{
  mAlloc.put_memory(mTvSize, (void*)tv);
}

// @brief ベクタ長からバイトサイズを計算する．
// @param[in] vectlen ベクタ長
//
// TestVector::block_num() にアクセスするためにクラスメソッドにしている．
ymuint
TvMgr::calc_size(ymuint vectlen)
{
  if ( vectlen == 0 ) {
    vectlen = 1;
  }
  return sizeof(TestVector) + kPvBitLen * (TestVector::block_num(vectlen) - 1);
}

END_NAMESPACE_YM_SATPG
