#ifndef TVMGR_H
#define TVMGR_H

/// @file TvMgr.h
/// @brief TvMgr のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.

#include "satpg.h"
#include "TestVector.h"
#include "ym/UnitAlloc.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class TvMgr TvMgr.h "TvMgr.h"
/// @brief テストベクタを管理するクラス
//////////////////////////////////////////////////////////////////////
class TvMgr
{
public:

  /// @brief コンストラクタ
  /// @param[in] network 対象のネットワーク
  /// @param[in] fault_type 故障の種類
  TvMgr(const TpgNetwork& network,
	FaultType fault_type);

  /// @brief デストラクタ
  ///
  /// このオブジェクトが確保したすべてのテストベクタを開放する．
  ~TvMgr();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 内容を空にする．
  ///
  /// このオブジェクトが生成した全てのテストベクタが開放される．
  void
  clear();

  /// @brief 入力数を返す．
  ymuint
  input_num() const;

  /// @brief DFF数を返す．
  ymuint
  dff_num() const;

  /// @brief ベクタ長を返す．
  ymuint
  vect_len() const;

  /// @brief 新しいパタンを生成する．
  /// @return 生成されたパタンを返す．
  ///
  /// パタンは0で初期化される．
  TestVector*
  new_vector();

  /// @brief 縮退故障用のパタンを削除する．
  void
  delete_vector(TestVector* tv);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ベクタ長からバイトサイズを計算する．
  /// @param[in] vectlen ベクタ長
  ///
  /// TestVector::block_num() にアクセスするためにクラスメソッドにしている．
  static
  ymuint
  calc_size(ymuint vectlen);


private:
  //////////////////////////////////////////////////////////////////////
  // 使用禁止にするためのプライベート宣言
  //////////////////////////////////////////////////////////////////////

  /// @brief コピーコンストラクタ
  /// @note 使用禁止なので実装しない．
  TvMgr(const TvMgr& src);

  /// @brief 代入演算子
  /// @note 使用禁止なので実装しない．
  const TvMgr&
  operator=(const TvMgr& src);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 故障の種類
  FaultType mFaultType;

  // 対象回路の入力数
  ymuint mInputNum;

  // 対象回路の DFF 数
  ymuint mDffNum;

  // テストベクタの長さ
  ymuint mVectLen;

  // テストベクタの実際のバイトサイズ
  ymuint mTvSize;

  // テストベクタのメモリ確保用のアロケータ
  UnitAlloc mAlloc;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief 入力数を返す．
inline
ymuint
TvMgr::input_num() const
{
  return mInputNum;
}

// @brief DFF数を返す．
inline
ymuint
TvMgr::dff_num() const
{
  return mDffNum;
}

// @brief ベクタ長を返す．
inline
ymuint
TvMgr::vect_len() const
{
  return mVectLen;
}

END_NAMESPACE_YM_SATPG

#endif // TVMGR_H
