
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
calc_vectlen(ymuint val)
{
  if ( val == 0 ) {
    val = 1;
  }
  return val;
}

END_NONAMESPACE

// @brief コンストラクタ
TvMgr::TvMgr(const TpgNetwork& network) :
  mInputNum(network.input_num()),
  mDffNum(network.dff_num()),
  mSaVectLen(calc_vectlen(mInputNum + mDffNum)),
  mSaTvSize(calc_size(mSaVectLen)),
  mSaAlloc(mSaTvSize, 1024),
  mTdVectLen(calc_vectlen(mInputNum * 2 + mDffNum)),
  mTdTvSize(calc_size(mTdVectLen)),
  mTdAlloc(mTdTvSize, 1024)
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
  mSaAlloc.destroy();
  mTdAlloc.destroy();

  mInputNum = 0;
  mDffNum = 0;

  mSaVectLen = 0;
  mSaTvSize = 0;

  mTdVectLen = 0;
  mTdTvSize = 0;
}

// @brief 新しいパタンを生成する．
// @return 生成されたパタンを返す．
//
// パタンは0で初期化される．
TestVector*
TvMgr::new_sa_vector()
{
  void* p = mSaAlloc.get_memory(mSaTvSize);
  TestVector* tv = new (p) TestVector(mInputNum, mDffNum, false);

  return tv;
}

// @brief 新しいパタンを生成する．
// @return 生成されたパタンを返す．
//
// パタンは0で初期化される．
TestVector*
TvMgr::new_td_vector()
{
  void* p = mTdAlloc.get_memory(mTdTvSize);
  TestVector* tv = new (p) TestVector(mInputNum, mDffNum, true);

  return tv;
}

// @brief パタンを削除する．
void
TvMgr::delete_vector(TestVector* tv)
{
  if ( tv->is_sa_mode() ) {
    mSaAlloc.put_memory(mSaTvSize, (void*)tv);
  }
  else {
    mTdAlloc.put_memory(mTdTvSize, (void*)tv);
  }
}

// @brief ベクタ長からバイトサイズを計算する．
// @param[in] vectlen ベクタ長
//
// TestVector::block_num() にアクセスするためにクラスメソッドにしている．
ymuint
TvMgr::calc_size(ymuint vectlen)
{
  return sizeof(TestVector) + kPvBitLen * (TestVector::block_num(vectlen) - 1);
}

END_NAMESPACE_YM_SATPG
