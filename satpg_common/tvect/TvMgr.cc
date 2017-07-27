
/// @file TvMgr.cc
/// @brief TvMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "TvMgr.h"
#include "TpgNetwork.h"
#include "TestVector.h"
#include "InputVector.h"
#include "DffVector.h"


BEGIN_NAMESPACE_YM_SATPG

BEGIN_NONAMESPACE

// @brief ベクタ長からバイトサイズを計算する．
// @param[in] vectlen ベクタ長
inline
ymuint
calc_size(ymuint vectlen)
{
  if ( vectlen == 0 ) {
    vectlen = 1;
  }
  return sizeof(BitVector) + kPvBitLen * (BitVector::block_num(vectlen) - 1);
}

END_NONAMESPACE

// @brief コンストラクタ
// @param[in] network 対象のネットワーク
// @param[in] fault_type 故障のタイプ
TvMgr::TvMgr(const TpgNetwork& network,
	     FaultType fault_type) :
  mFaultType(fault_type),
  mInputNum(network.input_num()),
  mDffNum(network.dff_num()),
  mIvSize(calc_size(mInputNum)),
  mFvSize(calc_size(mDffNum)),
  mTestVectorAlloc(sizeof(TestVector), 1024),
  mInputVectorAlloc(mIvSize, 1024),
  mDffVectorAlloc(mFvSize, 1024)
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
  mTestVectorAlloc.destroy();
  mInputVectorAlloc.destroy();
  mDffVectorAlloc.destroy();
}

// @brief テストベクタを生成する．
TestVector*
TvMgr::new_vector()
{
  InputVector* iv = new_input_vector();
  DffVector* dv = nullptr;
  InputVector* av = nullptr;
  if ( dff_num() > 0 ) {
    dv = new_dff_vector();
  }
  if ( mFaultType == kFtTransitionDelay ) {
    av = new_input_vector();
  }
  void* p = mTestVectorAlloc.get_memory(sizeof(TestVector));
  return new (p) TestVector(iv, dv, av);
}

// @brief テストベクタを削除する．
void
TvMgr::delete_vector(TestVector* vect)
{
  delete_vector(vect->mInputVector);
  if ( vect->mDffVector != nullptr ) {
    delete_vector(vect->mDffVector);
  }
  if ( vect->mAuxInputVector != nullptr ) {
    delete_vector(vect->mAuxInputVector);
  }
  mTestVectorAlloc.put_memory(sizeof(TestVector), (void*)vect);
}

// @brief 新しい入力用ベクタを生成する．
// @return 生成されたパタンを返す．
//
// パタンは0で初期化される．
InputVector*
TvMgr::new_input_vector()
{
  void* p = mInputVectorAlloc.get_memory(mIvSize);
  InputVector* vect = new (p) InputVector(mInputNum);

  return vect;
}

// @brief 入力用ベクタを削除する．
void
TvMgr::delete_vector(InputVector* vect)
{
  mInputVectorAlloc.put_memory(mIvSize, (void*)vect);
}

// @brief 新しいFF用ベクタを生成する．
// @return 生成されたベクタを返す．
//
// パタンは0で初期化される．
DffVector*
TvMgr::new_dff_vector()
{
  void* p = mDffVectorAlloc.get_memory(mFvSize);
  DffVector* vect = new (p) DffVector(mDffNum);

  return vect;
}

// @brief FF用ベクタを削除する．
// @param[in] vect 削除するベクタ
void
TvMgr::delete_vector(DffVector* vect)
{
  mDffVectorAlloc.put_memory(mFvSize, (void*)vect);
}

END_NAMESPACE_YM_SATPG
