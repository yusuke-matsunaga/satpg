
/// @file TpgFault.cc
/// @brief TpgFault の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2007, 2012-2014 Yusuke Matsunaga
/// All rights reserved.


#include "TpgFault.h"
#include "TpgStemFault.h"
#include "TpgBranchFault.h"
#include "TpgNode.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
// クラス TpgFault
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID番号
// @param[in] val 故障値
// @param[in] rep_fault 代表故障
TpgFault::TpgFault(ymuint id,
		   int val,
		   TpgFault* rep_fault) :
  mRepFault(rep_fault)
{
  // val は 0 か 1 のはずだが念の為マスクしておく
  mIdVal = (id << 1) | static_cast<ymuint>(val & 1);
}

// @brief デストラクタ
TpgFault::~TpgFault()
{
}

// @brief 代表故障を設定する．
// @param[in] rep 代表故障
void
TpgFault::set_rep(TpgFault* rep)
{
  mRepFault = rep;
}

// @brief FFRを設定する．
void
TpgFault::set_ffr(const TpgFFR* ffr)
{
  mFFR = ffr;
}

// @brief MFFCを設定する．
void
TpgFault::set_mffc(const TpgMFFC* mffc)
{
  mMFFC = mffc;
}

// @brief ストリーム出力演算子
// @param[in] s 出力先のストリーム
// @param[in] f 故障
ostream&
operator<<(ostream& s,
	   const TpgFault* f)
{
  return s << f->str();
}


//////////////////////////////////////////////////////////////////////
// クラス TpgStemFault
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID番号
// @param[in] name 故障位置のノード名
// @param[in] val 故障値
// @param[in] node 故障位置のノード
// @param[in] rep_fault 代表故障
TpgStemFault::TpgStemFault(ymuint id,
			   const char* name,
			   int val,
			   const TpgNode* node,
			   TpgFault* rep_fault) :
  TpgFault(id, val, rep_fault),
  mNodeName(name),
  mTpgNode(node)
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
  return mTpgNode;
}

// @brief 故障の出力側の TpgNode を返す．
//
// is_branch_fault() == true の時のみ意味を持つ．
const TpgNode*
TpgStemFault::tpg_onode() const
{
  return mTpgNode;
}

// @brief ステムの故障の時 true を返す．
bool
TpgStemFault::is_stem_fault() const
{
  return true;
}

// @brief ブランチの入力位置を返す．
ymuint
TpgStemFault::fault_pos() const
{
  ASSERT_NOT_REACHED;
  return 0;
}

// @brief tpg_inode 上の故障位置を返す．
//
// is_input_fault() == true の時のみ意味を持つ．
ymuint
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
  ans << mNodeName << ":O:";
  if ( val() ) {
    ans <<"SA1";
  }
  else {
    ans <<"SA0";
  }
  return ans.str();
}


//////////////////////////////////////////////////////////////////////
// クラス TpgBranchFault
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID番号
// @param[in] name ノード名
// @param[in] val 故障値
// @param[in] pos 故障の入力位置
// @param[in] onode 出力側の TpgNode
// @param[in] inode 入力側の TpgNode
// @param[in] tpg_pos node 上の故障位置
// @param[in] rep_fault 代表故障
TpgBranchFault::TpgBranchFault(ymuint id,
			       const char* name,
			       int val,
			       ymuint pos,
			       const TpgNode* onode,
			       const TpgNode* inode,
			       ymuint tpg_pos,
			       TpgFault* rep_fault) :
  TpgFault(id, val, rep_fault),
  mNodeName(name),
  mOnode(onode),
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
  return mOnode;
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
ymuint
TpgBranchFault::fault_pos() const
{
  return mPos;
}

// @brief tpg_inode 上の故障位置を返す．
//
// is_branch_fault() == true の時のみ意味を持つ．
ymuint
TpgBranchFault::tpg_pos() const
{
  return mTpgPos;
}

// @brief 故障の内容を表す文字列を返す．
string
TpgBranchFault::str() const
{
  ostringstream ans;
  ans << mNodeName << ":I" << fault_pos() << ":";
  if ( val() ) {
    ans <<"SA1";
  }
  else {
    ans <<"SA0";
  }
  return ans.str();
}

END_NAMESPACE_YM_SATPG
