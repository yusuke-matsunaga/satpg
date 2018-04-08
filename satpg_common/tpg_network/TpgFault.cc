
/// @file TpgFault.cc
/// @brief TpgFault の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2007, 2012-2014, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "TpgFault.h"
#include "TpgFaultBase.h"
#include "TpgStemFault.h"
#include "TpgBranchFault.h"
#include "TpgNode.h"
#include "Val3.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
// クラス TpgFaultBase
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID番号
// @param[in] val 故障値
// @param[in] node 故障位置のノード
// @param[in] name 故障位置のノード名
// @param[in] rep_fault 代表故障
TpgFaultBase::TpgFaultBase(int id,
			   int val,
			   const TpgNode* node,
			   const char* name,
			   TpgFault* rep_fault) :
  mTpgNode(node),
  mNodeName(name),
  mRepFault(rep_fault),
  mFFR(nullptr),
  mMFFC(nullptr)
{
  // val は 0 か 1 のはずだが念の為マスクしておく
  mIdVal = (id << 1) | static_cast<int>(val & 1);
}

// @brief デストラクタ
TpgFaultBase::~TpgFaultBase()
{
}

// @brief ID番号を返す．
int
TpgFaultBase::id() const
{
  return static_cast<int>(mIdVal >> 1);
}

// @brief 故障値を返す．
// @note 返す値は 0 か 1
int
TpgFaultBase::val() const
{
  return static_cast<int>(mIdVal & 1UL);
}

// @brief 代表故障を返す．
// @note 代表故障の時は自分自身を返す．
const TpgFault*
TpgFaultBase::rep_fault() const
{
  return mRepFault;
}

#if 0
// @brief 代表故障を返す．
TpgFault*
TpgFaultBase::_rep_fault()
{
  return mRepFault;
}
#endif

// @brief この故障の属しているFFRを返す．
const TpgFFR&
TpgFaultBase::ffr() const
{
  return *mFFR;
}

// @brief この故障の属しているMFFCを返す．
const TpgMFFC&
TpgFaultBase::mffc() const
{
  return *mMFFC;
}

// @brief 代表故障を設定する．
// @param[in] rep 代表故障
void
TpgFaultBase::set_rep(const TpgFault* rep)
{
  mRepFault = rep;
}

// @brief FFRを設定する．
void
TpgFaultBase::set_ffr(const TpgFFR* ffr)
{
  mFFR = ffr;
}

// @brief MFFCを設定する．
void
TpgFaultBase::set_mffc(const TpgMFFC* mffc)
{
  mMFFC = mffc;
}


//////////////////////////////////////////////////////////////////////
// クラス TpgStemFault
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID番号
// @param[in] val 故障値
// @param[in] node 故障位置のノード
// @param[in] name 故障位置のノード名
// @param[in] rep_fault 代表故障
TpgStemFault::TpgStemFault(int id,
			   int val,
			   const TpgNode* node,
			   const char* name,
			   TpgFault* rep_fault) :
  TpgFaultBase(id, val, node, name, rep_fault)
{
  ASSERT_COND( tpg_inode() != nullptr );
}

// @brief デストラクタ
TpgStemFault::~TpgStemFault()
{
}

// @brief 故障の入力側の TpgNode を返す．
const TpgNode*
TpgStemFault::tpg_inode() const
{
  return tpg_node();
}

// @brief 故障の出力側の TpgNode を返す．
//
// is_branch_fault() == true の時のみ意味を持つ．
const TpgNode*
TpgStemFault::tpg_onode() const
{
  return tpg_node();
}

// @brief ステムの故障の時 true を返す．
bool
TpgStemFault::is_stem_fault() const
{
  return true;
}

// @brief ブランチの入力位置を返す．
int
TpgStemFault::fault_pos() const
{
  ASSERT_NOT_REACHED;

  return 0;
}

// @brief tpg_inode 上の故障位置を返す．
//
// is_input_fault() == true の時のみ意味を持つ．
int
TpgStemFault::tpg_pos() const
{
  ASSERT_NOT_REACHED;

  return 0;
}

// @brief 故障の内容を表す文字列を返す．
string
TpgStemFault::str() const
{
  ostringstream ans;
  ans << node_name() << ":O:";
  if ( val() ) {
    ans <<"1";
  }
  else {
    ans <<"0";
  }
  return ans.str();
}


//////////////////////////////////////////////////////////////////////
// クラス TpgBranchFault
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID番号
// @param[in] val 故障値
// @param[in] onode 出力側の TpgNode
// @param[in] name ノード名
// @param[in] pos 故障の入力位置
// @param[in] inode 入力側の TpgNode
// @param[in] tpg_pos node 上の故障位置
// @param[in] rep_fault 代表故障
TpgBranchFault::TpgBranchFault(int id,
			       int val,
			       const TpgNode* onode,
			       const char* name,
			       int pos,
			       const TpgNode* inode,
			       int tpg_pos,
			       TpgFault* rep_fault) :
  TpgFaultBase(id, val, onode, name, rep_fault),
  mPos(pos),
  mInode(inode),
  mTpgPos(tpg_pos)
{
  ASSERT_COND( tpg_onode() != nullptr );
  ASSERT_COND( tpg_inode() != nullptr );
}

// @brief デストラクタ
TpgBranchFault::~TpgBranchFault()
{
}

// @brief 故障の入力側の TpgNode を返す．
const TpgNode*
TpgBranchFault::tpg_inode() const
{
  return mInode;
}

// @brief 故障の出力側の TpgNode を返す．
const TpgNode*
TpgBranchFault::tpg_onode() const
{
  return tpg_node();
}

// @brief ステムの故障の時 true を返す．
bool
TpgBranchFault::is_stem_fault() const
{
  return false;
}

// @brief ブランチの入力位置を返す．
//
// is_branch_fault() == true の時のみ意味を持つ．
int
TpgBranchFault::fault_pos() const
{
  return mPos;
}

// @brief tpg_inode 上の故障位置を返す．
//
// is_branch_fault() == true の時のみ意味を持つ．
int
TpgBranchFault::tpg_pos() const
{
  return mTpgPos;
}

// @brief 故障の内容を表す文字列を返す．
string
TpgBranchFault::str() const
{
  ostringstream ans;
  ans << node_name() << ":I" << fault_pos() << ":";
  if ( val() ) {
    ans <<"1";
  }
  else {
    ans <<"0";
  }
  return ans.str();
}

END_NAMESPACE_YM_SATPG
