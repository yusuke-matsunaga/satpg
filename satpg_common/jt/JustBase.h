#ifndef JUSTBASE_H
#define JUSTBASE_H

/// @file JustBase.h
/// @brief JustBase のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.


#include "Justifier.h"
#include "TpgNode.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class JustBase JustBase.h "JustBase.h"
/// @brief Justifier の継承クラスの基底クラス
//////////////////////////////////////////////////////////////////////
class JustBase :
  public Justifier
{
protected:

  /// @brief コンストラクタ
  /// @param[in] max_id ID番号の最大値
  JustBase(int max_id);

  /// @brief デストラクタ
  virtual
  ~JustBase();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief justified マークをつけ，mJustifiedNodeList に加える．
  /// @param[in] node 対象のノード
  /// @param[in] time タイムフレーム ( 0 or 1 )
  void
  set_mark(const TpgNode* node,
	   int time);

  /// @brief justified マークを読む．
  /// @param[in] node 対象のノード
  /// @param[in] time タイムフレーム ( 0 or 1 )
  bool
  mark(const TpgNode* node,
       int time) const;

  /// @brief 全てのマークを消す．
  void
  clear_mark();


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 個々のノードのマークを表す配列
  vector<ymuint8> mMarkArray;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] max_id ID番号の最大値
inline
JustBase::JustBase(int max_id) :
  mMarkArray(max_id, 0U)
{
}

// @brief デストラクタ
inline
JustBase::~JustBase()
{
}

// @brief justified マークをつける．
// @param[in] node 対象のノード
// @param[in] time タイムフレーム ( 0 or 1 )
inline
void
JustBase::set_mark(const TpgNode* node,
		   int time)
{
  // 念のため time の最下位ビットだけ使う．
  time &= 1;
  mMarkArray[node->id()] |= (1U << time);
}

// @brief justified マークを読む．
// @param[in] node 対象のノード
// @param[in] time タイムフレーム ( 0 or 1 )
inline
bool
JustBase::mark(const TpgNode* node,
	       int time) const
{
  // 念のため time の最下位ビットだけ使う．
  time &= 1;
  return static_cast<bool>((mMarkArray[node->id()] >> time) & 1U);
}

// @brief 全てのマークを消す．
inline
void
JustBase::clear_mark()
{
  for ( vector<ymuint8>::iterator p = mMarkArray.begin(); p != mMarkArray.end(); ++ p ) {
    *p = 0U;
  }
}

END_NAMESPACE_YM_SATPG

#endif // JUSTBASE_H
