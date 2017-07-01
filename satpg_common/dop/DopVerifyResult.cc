
/// @file DopVerifyResult.cc
/// @brief DopVerifyResult の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "DopVerifyResult.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
// クラス DopVerifyResult
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
DopVerifyResult::DopVerifyResult()
{
}

// @brief デストラクタ
DopVerifyResult::~DopVerifyResult()
{
}

// @brief 成功結果を追加する．
// @param[in] f 故障
void
DopVerifyResult::add_good(const TpgFault* f)
{
  mGoodList.push_back(f);
}

// @brief エラー結果を追加する．
// @param[in] f 故障
// @param[in] assign_list 値割り当てのリスト
void
DopVerifyResult::add_error(const TpgFault* f,
			   const NodeValList& assign_list)
{
  mErrorList.push_back(ErrorCase(f, assign_list));
}

// @brief 成功回数を得る．
ymuint
DopVerifyResult::good_count() const
{
  return mGoodList.size();
}

// @brief 成功した故障を得る．
// @param[in] pos 位置版号 ( 0 <= pos < good_count() )
const TpgFault*
DopVerifyResult::good_fault(ymuint pos) const
{
  ASSERT_COND( pos < good_count() );
  return mGoodList[pos];
}

// @brief エラー回数を得る．
ymuint
DopVerifyResult::error_count() const
{
  return mErrorList.size();
}

// @brief エラーの故障を得る．
// @param[in] pos 位置版号 ( 0 <= pos < error_count() )
const TpgFault*
DopVerifyResult::error_fault(ymuint pos) const
{
  ASSERT_COND( pos < error_count() );
  return mErrorList[pos].mFault;
}

// @brief エラーとなった割当を得る．
const NodeValList&
DopVerifyResult::error_assign_list(ymuint pos) const
{
  ASSERT_COND( pos < error_count() );
  return mErrorList[pos].mAssignList;
}

DopVerifyResult::ErrorCase::ErrorCase(const TpgFault* f,
				      const NodeValList& assign_list) :
  mFault(f),
  mAssignList(assign_list)
{
}

END_NAMESPACE_YM_SATPG
