
/// @file TvMgr.cc
/// @brief TvMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "TvMgr.h"
#include "TpgNetwork.h"


BEGIN_NAMESPACE_YM_SATPG

// @brief コンストラクタ
TvMgr::TvMgr() :
  mSaAlloc(nullptr),
  mTdAlloc(nullptr),
  mInputNum(0),
  mDffNum(0),
  mSaVectLen(0),
  mSaTvSize(0),
  mTdVectLen(0),
  mTdTvSize(0)
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
  if ( mSaAlloc != nullptr ) {
    delete mSaAlloc;
    mSaAlloc = nullptr;
  }
  if ( mTdAlloc != nullptr ) {
    delete mTdAlloc;
    mTdAlloc = nullptr;
  }

  mInputNum = 0;
  mDffNum = 0;

  mSaVectLen = 0;
  mSaTvSize = 0;

  mTdVectLen = 0;
  mTdTvSize = 0;
}

// @brief 初期化する．
// @param[in] network 対象のネットワーク
//
// 副作用で最初に clear() が呼ばれる．
void
TvMgr::init(const TpgNetwork& network)
{
  clear();

  mInputNum = network.input_num();
  mDffNum = network.dff_num();

  mSaVectLen = mInputNum + mDffNum;
  mTdVectLen = mInputNum + mInputNum + mDffNum;
  if ( mSaVectLen == 0 ) {
    // 0 だとヤバい
    mSaVectLen = 1;
  }
  if ( mTdVectLen == 0 ) {
    // 0 だとヤバイ
    mTdVectLen = 1;
  }

  ymuint sa_nb = TestVector::block_num(mSaVectLen);
  mSaTvSize = sizeof(TestVector) + kPvBitLen * (sa_nb - 1);
  mSaAlloc = new UnitAlloc(mSaTvSize, 1024);

  ymuint td_nb = TestVector::block_num(mTdVectLen);
  mTdTvSize = sizeof(TestVector) + kPvBitLen * (td_nb - 1);
  mTdAlloc = new UnitAlloc(mTdTvSize, 1024);
}

// @brief 新しいパタンを生成する．
// @return 生成されたパタンを返す．
//
// パタンは0で初期化される．
TestVector*
TvMgr::new_sa_vector()
{
  void* p = mSaAlloc->get_memory(mSaTvSize);
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
  void* p = mTdAlloc->get_memory(mTdTvSize);
  TestVector* tv = new (p) TestVector(mInputNum, mDffNum, true);

  return tv;
}

// @brief パタンを削除する．
void
TvMgr::delete_vector(TestVector* tv)
{
  if ( tv->is_sa_mode() ) {
    mSaAlloc->put_memory(mSaTvSize, (void*)tv);
  }
  else {
    mTdAlloc->put_memory(mTdTvSize, (void*)tv);
  }
}

END_NAMESPACE_YM_SATPG
