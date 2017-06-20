#ifndef TPGBRANCHFAULT_H
#define TPGBRANCHFAULT_H

/// @file TpgBranchFault.h
/// @brief TpgBranchFault のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2007, 2012-2014 Yusuke Matsunaga
/// All rights reserved.

#include "TpgFault.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class TpgBranchFault TpgBranchFault.h "TpgBranchFault.h"
/// @brief 入力の故障を表すクラス
//////////////////////////////////////////////////////////////////////
class TpgBranchFault :
  public TpgFault
{
public:

  /// @brief コンストラクタ
  /// @param[in] id ID番号
  /// @param[in] name ノード名
  /// @param[in] val 故障値
  /// @param[in] pos 故障の入力位置
  /// @param[in] onode 出力側の TpgNode
  /// @param[in] inode 入力側の TpgNode
  /// @param[in] tpg_pos onode 上の故障位置
  /// @param[in] rep_fault 代表故障
  TpgBranchFault(ymuint id,
		 const char* name,
		 int val,
		 ymuint pos,
		 const TpgNode* onode,
		 const TpgNode* inode,
		 ymuint tpg_pos,
		 TpgFault* rep_fault);

  /// @brief デストラクタ
  virtual
  ~TpgBranchFault();


public:
  //////////////////////////////////////////////////////////////////////
  // read-only のメソッド
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障の入力側の TpgNode を返す．
  virtual
  const TpgNode*
  tpg_inode() const;

  /// @brief 故障の出力側の TpgNode を返す．
  virtual
  const TpgNode*
  tpg_onode() const;

  /// @brief ステムの故障の時 true を返す．
  virtual
  bool
  is_stem_fault() const;

  /// @brief ブランチの入力位置を返す．
  ///
  /// is_branch_fault() == true の時のみ意味を持つ．
  virtual
  ymuint
  fault_pos() const;

  /// @brief tpg_inode 上の故障位置を返す．
  ///
  /// is_branch_fault() == true の時のみ意味を持つ．
  virtual
  ymuint
  tpg_pos() const;

  /// @brief 故障の内容を表す文字列を返す．
  virtual
  string
  str() const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ノード名
  const char* mNodeName;

  // 出力側の TpgNode
  const TpgNode* mOnode;

  // 故障の入力位置
  ymuint mPos;

  // 入力側の TpgNode
  const TpgNode* mInode;

  // mI_TpgNode 上の入力位置
  ymuint mTpgPos;

};

END_NAMESPACE_YM_SATPG

#endif // TPGBRANCHFAULT_H
