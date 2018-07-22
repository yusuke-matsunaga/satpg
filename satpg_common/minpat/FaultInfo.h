#ifndef FAULTINFO_H
#define FAULTINFO_H

/// @file FaultInfo.h
/// @brief FaultInfo のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.

#include "satpg.h"
#include "NodeValList.h"
#include "ym/Expr.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class FaultInfo FaultInfo.h "FaultInfo.h"
/// @brief MinPat 用の故障情報を表すクラス
//////////////////////////////////////////////////////////////////////
class FaultInfo
{
public:

  /// @brief コンストラクタ
  /// @param[in] fault 対象の故障
  /// @param[in] mand_cond 必要条件
  /// @param[in] sufficient_cond 十分条件
  FaultInfo(const TpgFault* fault,
	    const NodeValList& mand_cond,
	    const Expr& sufficient_cond);

  /// @brief デストラクタ
  ~FaultInfo();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障を返す．
  const TpgFault*
  fault() const;

  /// @brief 必要条件を返す．
  const NodeValList&
  mand_cond() const;

  /// @brief 十分条件を返す．
  const Expr&
  sufficient_cond() const;

  /// @brief 衝突している故障を追加する．
  /// @param[in] fi 追加する故障情報
  void
  add_conflict(FaultInfo* fi);

  /// @brief 衝突リストを返す．
  const vector<FaultInfo*>&
  conflict_list() const;

  /// @brief 両立している故障を追加する．
  /// @param[in] fi 追加する故障情報
  void
  add_compatible(FaultInfo* fi);

  /// @brief 両立している故障リストを返す．
  const vector<FaultInfo*>&
  compatible_list() const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 故障
  const TpgFault* mFault;

  // 必要条件
  NodeValList mMandCond;

  // 十分条件
  Expr mSufficientCond;

  // 衝突している故障のリスト
  vector<FaultInfo*> mConflictList;

  // 両立している故障のリスト
  vector<FaultInfo*> mCompatibleList;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] fault 対象の故障
// @param[in] mand_cond 必要条件
// @param[in] sufficient_cond 十分条件
inline
FaultInfo::FaultInfo(const TpgFault* fault,
		     const NodeValList& mand_cond,
		     const Expr& sufficient_cond) :
  mFault(fault),
  mMandCond(mand_cond),
  mSufficientCond(sufficient_cond)
{
}

// @brief デストラクタ
inline
FaultInfo::~FaultInfo()
{
}

// @brief 故障を返す．
inline
const TpgFault*
FaultInfo::fault() const
{
  return mFault;
}

// @brief 必要条件を返す．
inline
const NodeValList&
FaultInfo::mand_cond() const
{
  return mMandCond;
}

// @brief 十分条件の割当リストを返す．
inline
const Expr&
FaultInfo::sufficient_cond() const
{
  return mSufficientCond;
}

// @brief 衝突している故障を追加する．
inline
void
FaultInfo::add_conflict(FaultInfo* fi)
{
  mConflictList.push_back(fi);
}

// @brief 衝突リストを返す．
inline
const vector<FaultInfo*>&
FaultInfo::conflict_list() const
{
  return mConflictList;
}

// @brief 両立している故障を追加する．
// @param[in] fi 追加する故障情報
inline
void
FaultInfo::add_compatible(FaultInfo* fi)
{
  mCompatibleList.push_back(fi);
}

// @brief 両立している故障リストを返す．
inline
const vector<FaultInfo*>&
FaultInfo::compatible_list() const
{
  return mCompatibleList;
}

END_NAMESPACE_YM_SATPG

#endif // FAULTINFO_H
