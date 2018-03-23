#ifndef DOPVERIFYRESULT_H
#define DOPVERIFYRESULT_H

/// @file DopVerifyResult.h
/// @brief DopVerifyResult のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "satpg.h"
#include "NodeValList.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class DopVerifyResult DopVerifyResult.h "DopVerifyResult.h"
/// @brief DopVerify の結果を格納するクラス
//////////////////////////////////////////////////////////////////////
class DopVerifyResult
{
public:

  /// @brief コンストラクタ
  DopVerifyResult();

  /// @brief デストラクタ
  ~DopVerifyResult();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 成功結果を追加する．
  /// @param[in] f 故障
  void
  add_good(const TpgFault* f);

  /// @brief エラー結果を追加する．
  /// @param[in] f 故障
  /// @param[in] assign_list 値割り当てのリスト
  void
  add_error(const TpgFault* f,
	    const NodeValList& assign_list);

  /// @brief 成功回数を得る．
  int
  good_count() const;

  /// @brief 成功した故障を得る．
  /// @param[in] pos 位置版号 ( 0 <= pos < good_count() )
  const TpgFault*
  good_fault(int pos) const;

  /// @brief エラー回数を得る．
  int
  error_count() const;

  /// @brief エラーの故障を得る．
  /// @param[in] pos 位置版号 ( 0 <= pos < error_count() )
  const TpgFault*
  error_fault(int pos) const;

  /// @brief エラーとなった割当を得る．
  const NodeValList&
  error_assign_list(int pos) const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられるデータ構造
  //////////////////////////////////////////////////////////////////////

  struct ErrorCase
  {
    // コンストラクタ
    ErrorCase(const TpgFault* f,
	      const NodeValList& assign_list);

    // 故障
    const TpgFault* mFault;

    // 値割り当てのリスト
    NodeValList mAssignList;

  };


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 成功した故障のリスト
  vector<const TpgFault*> mGoodList;

  // エラーのリスト
  vector<ErrorCase> mErrorList;

};

END_NAMESPACE_YM_SATPG

#endif // DOPVERIFYRESULT_H
