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
  TvMgr(const TpgNetwork& network);

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

  /// @brief 縮退故障用のベクタ長を返す．
  ymuint
  sa_vect_len() const;

  /// @brief 遷移故障用のベクタ長を返す．
  ymuint
  td_vect_len() const;

  /// @brief 新しい縮退故障用のパタンを生成する．
  /// @return 生成されたパタンを返す．
  ///
  /// パタンは0で初期化される．
  TestVector*
  new_sa_vector();

  /// @brief 新しい遷移故障用パタンを生成する．
  /// @return 生成されたパタンを返す．
  ///
  /// パタンは0で初期化される．
  TestVector*
  new_td_vector();

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

  // 対象回路の入力数
  ymuint mInputNum;

  // 対象回路の DFF 数
  ymuint mDffNum;

  // 縮退故障用のテストベクタの長さ
  ymuint mSaVectLen;

  // 縮退故障用のテストベクタの実際のサイズ
  ymuint mSaTvSize;

  // 遷移故障用のテストベクタの長さ
  ymuint mTdVectLen;

  // 遷移故障用のテストベクタの実際のサイズ
  ymuint mTdTvSize;

  // 縮退故障用のテストベクタのメモリ確保用のアロケータ
  UnitAlloc mSaAlloc;

  // 遷移故障用のテストベクタのメモリ確保用のアロケータ
  UnitAlloc mTdAlloc;

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

// @brief 縮退故障用のベクタ長を返す．
inline
ymuint
TvMgr::sa_vect_len() const
{
  return mSaVectLen;
}

// @brief 遷移故障用のベクタ長を返す．
inline
ymuint
TvMgr::td_vect_len() const
{
  return mTdVectLen;
}

END_NAMESPACE_YM_SATPG

#endif // TVMGR_H
