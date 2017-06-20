#ifndef TPGDFF_H
#define TPGDFF_H

/// @file TpgDff.h
/// @brief TpgDff のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "satpg.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class TpgDff TpgDff.h "TpgDff.h"
/// @brief DFF を表すクラス
//////////////////////////////////////////////////////////////////////
class TpgDff
{
  friend class TpgNetwork;

public:

  /// @brief コンストラクタ
  TpgDff();

  /// @brief デストラクタ
  ~TpgDff();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ID番号を返す．
  ymuint
  id() const;

  /// @brief 入力端子のノードを返す．
  TpgNode*
  input() const;

  /// @brief 出力端子のノードを返す．
  TpgNode*
  output() const;

  /// @brief クロック端子のノードを返す．
  TpgNode*
  clock() const;

  /// @brief クリア端子のノードを返す．
  TpgNode*
  clear() const;

  /// @brief プリセット端子のノードを返す．
  TpgNode*
  preset() const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ID番号
  ymuint mId;

  // 入力端子
  TpgNode* mInput;

  // 出力端子
  TpgNode* mOutput;

  // クロック端子
  TpgNode* mClock;

  // クリア端子
  TpgNode* mClear;

  // プリセット端子
  TpgNode* mPreset;

};

END_NAMESPACE_YM_SATPG

#endif // TPGDFF_H
