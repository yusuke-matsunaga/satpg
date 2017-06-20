#ifndef TPGMFFC_H
#define TPGMFFC_H

/// @file TpgMFFC.h
/// @brief TpgMFFC のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.

#include "satpg.h"
#include "ym/Alloc.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class TpgMFFC TpgMFFC.h "TpgMFFC.h"
/// @brief TpgNetwork の MFFC(Maximal Fanout Free Cone) の情報を表すクラス
/// @sa TpgNetwork
/// @sa TpgNode
/// @sa TpgFault
/// @sa TpgFFR
///
/// 具体的には以下の情報を持つ．
/// - MFFC の根のノード
/// - MFFC に含まれる FFR のリスト
/// - MFFC に含まれる代表故障のリスト
/// 一度設定されたら不変のオブジェクトとなる．
//////////////////////////////////////////////////////////////////////
class TpgMFFC
{
public:

  /// @brief コンストラクタ
  TpgMFFC();

  /// @brief デストラクタ
  ~TpgMFFC();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 根のノードを返す．
  const TpgNode*
  root() const;

  /// @brief このMFFCに含まれるFFR数を返す．
  ymuint
  elem_num() const;

  /// @brief このMFFCに含まれるFFRを返す．
  /// @param[in] pos 位置番号 ( 0 <= pos < elem_num() )
  const TpgFFR*
  elem(ymuint pos) const;

  /// @brief このMFFCに含まれる代表故障の数を返す．
  ymuint
  fault_num() const;

  /// @brief このMFFCに含まれる代表故障を返す．
  /// @param[in] pos 位置番号 ( 0 <= pos < fault_num() )
  const TpgFault*
  fault(ymuint pos) const;


public:
  //////////////////////////////////////////////////////////////////////
  // 設定用の関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 内容を設定する．
  /// @param[in] root 根のノード
  /// @param[in] ffr_list このMFFCに含まれるFFRのリスト
  /// @param[in] fault_list このMFFCに含まれる故障のリスト
  /// @param[in] alloc メモリアロケータ
  void
  set(TpgNode* root,
      const vector<const TpgFFR*>& ffr_list,
      const vector<TpgFault*>& fault_list,
      Alloc& alloc);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 根のノード
  const TpgNode* mRoot;

  // FFR数
  ymuint mElemNum;

  // FFRの配列
  const TpgFFR** mElemList;

  // 故障数
  ymuint mFaultNum;

  // 故障の配列
  TpgFault** mFaultList;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
inline
TpgMFFC::TpgMFFC()
{
  mRoot = nullptr;
  mElemNum = 0;
  mElemList = nullptr;
  mFaultNum = 0;
  mFaultList = nullptr;
}

// @brief デストラクタ
inline
TpgMFFC::~TpgMFFC()
{
}

// @brief 根のノードを返す．
inline
const TpgNode*
TpgMFFC::root() const
{
  return mRoot;
}

// @brief このMFFCに含まれるFFR数を返す．
inline
ymuint
TpgMFFC::elem_num() const
{
  return mElemNum;
}

// @brief このMFFCに含まれるFFRを返す．
// @param[in] pos 位置番号 ( 0 <= pos < elem_num() )
inline
const TpgFFR*
TpgMFFC::elem(ymuint pos) const
{
  ASSERT_COND( pos < elem_num() );

  return mElemList[pos];
}

// @brief このFFRに含まれる代表故障の数を返す．
inline
ymuint
TpgMFFC::fault_num() const
{
  return mFaultNum;
}

// @brief このFFRに含まれる代表故障を返す．
// @param[in] pos 位置番号 ( 0 <= pos < fault_num() )
inline
const TpgFault*
TpgMFFC::fault(ymuint pos) const
{
  ASSERT_COND( pos < fault_num() );

  return mFaultList[pos];
}

END_NAMESPACE_YM_SATPG

#endif // TPGMFFC_H
